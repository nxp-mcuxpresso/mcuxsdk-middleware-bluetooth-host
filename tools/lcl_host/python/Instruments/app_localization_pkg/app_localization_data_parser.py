#------------------------------------------------------------------------------
#  Copyright 2025 NXP
#  All rights reserved.
#
#  SPDX-License-Identifier: BSD-3-Clause
#------------------------------------------------------------------------------
import yaml
import numpy as np
import itertools
import struct
import binascii
import base64
import sys
import os

BaseDir = os.path.dirname(os.path.realpath(__file__))
sys.path.append(os.path.join(BaseDir, '..\..', 'Generic'))

from nesteddict import NestedDict
import stringutils as su
import dictutils as du


class ReturnCodes(object):
    info = {
        'success': ('Successful measurement', 0),
        'emptyresponse': ('Empty response from board error', 2),
        'badresponse': ('Bad response from board error', 2),
        'crc': ('crc error', 2),
        'parsing': ('Parsing error', 2),
        'validation': ('Failed while validating result-dict error', 2),
        'watchdog': ('Watchdog error', 2),
        'chanaccessfail': ('Channel access fail error', 2),
        'assertion': ('Assertion error', 2),
        'noack': ('No-acknowledgement error', 2),
        'timeout': ('Timeout error', 2),
        'unknown': ('Unknown error', 2),
        'badgain': ('RX gain too high', 2),
        'negative': ('Distance negative warning', 1),
        'toolow': ('Distance too low warning', 1),
        'toohigh': ('Distance too high warning', 1),
        'notanumber': ('Not a number', 1),
        'unexpecteduniqueid': ('Unexpected Unique ID', 2),
        'unknownuniqueid': ('Unknown Unique ID', 2),
        'swversionmismatch': ('Mismatching SW Version', 2),
    }

    def __init__(self, key):
        self.key = key
        if self.key in list(self.info.keys()):
            self.text, self.type = self.info[key]
        else:
            assert False, '%s is not a known ErrorCode key' % key

    def is_error(self):
        if self.type == 2:
            return True
        return False

    def is_warning(self):
        if self.type == 1:
            return True
        return False

    def is_valid(self):
        if self.type == 0:
            return True
        return False

    def __setattr__(self, attr, value):
        if hasattr(self, attr):
            raise Exception('Attribute %s already has a value and so cannot be written to' % attr)
        self.__dict__[attr] = value

class ResultParser:
    def __init__(self):
        self.run_cnt = 0
        pass
    
    def check_CRC32(self, resp_msg, debug=0, log=None):
        crc32_table = [0x7F1C607B, 0x77073096, 0xEE0E612C, 0x990951BA, 0x076DC419, 0x706AF48F, 0xc4f4e977, 0x556755d2,
                       0xE963A535, 0x9E6495A3, 0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988, 0xa1d38a68, 0x03b2c522,
                       0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91, 0x1DB71064, 0x6AB020F2, 0xd68ddc0f, 0xf4077f02,
                       0xF3B97148, 0x84BE41DE, 0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7, 0x19fa9c3f, 0x3c3a60f3,
                       0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC, 0x14015C4F, 0x63066CD9, 0x38a7d00c, 0xc98fcf5a,
                       0xFA0F3D63, 0x8D080DF5, 0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172, 0x2f7dc879, 0xa0e7c527,
                       0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B, 0x35B5A8FA, 0x42B2986C, 0x7f40c232, 0x7b92ad99,
                       0xDBBBC9D6, 0xACBCF940, 0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59, 0xdab5f1f1, 0x530c7f63,
                       0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116, 0x21B4F4B5, 0x56B3C423, 0x8cf2ab15, 0x9e9a17b0,
                       0xCFBA9599, 0xB8BDA50F, 0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924, 0x6b515118, 0xc6e8d3f3,
                       0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D, 0x76DC4190, 0x01DB7106, 0xc17ecbb8, 0x64a39a0c,
                       0x98D220BC, 0xEFD5102A, 0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433, 0xcb61dd84, 0x5ea83c5b,
                       0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818, 0x7F6A0DBB, 0x086D3D2D, 0x5f4a7a0d, 0x4c9a4a16,
                       0x91646C97, 0xE6635C01, 0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0x4F6A0D6B, 0x460ce7fd, 0x94d0c2ab,
                       0xa4c52f20, 0x8fe19d75, 0x6b19278d, 0x14d77aa7, 0xf0e83a51, 0x24f965ef, 0x1c610e61, 0x17a495cc,
                       0x62f20859, 0x0b4789db, 0x3d3ebd17, 0x080db3a8, 0x90b294c7, 0xb136eae1, 0xc77ef2c2, 0x8bc2dc2d]
        FirmwareCRC32 = None
        PyAppCRC32 = 0xFFFFFFFF
        resp_ascii = []
        resp_list = list(resp_msg)
        # Find index where crc32 begin / end of data payload
        payload_index = resp_msg.find('CRC32') - 1
        if payload_index <= 0:
            return True, resp_msg, None, None
        # Get crc8 value
        try:
            FirmwareCRC32 = bytes("".join(resp_list[payload_index + 7:payload_index + 15]).encode('ascii'))
            FirmwareCRC32 = int(FirmwareCRC32, 16)
            if FirmwareCRC32 is None:
                return False, resp_msg, None, None
        except Exception as e:
            # CRC itself has been corrupted (couldn't be parsed)
            if log is not None:
                log.info(e)
            return False, resp_msg, None, None

        # Crc8 should be removed from payload data,because it generates an error in the processing of the payload afterwards
        payload_list = resp_list[0:payload_index] + resp_list[payload_index + 15:len(resp_list)]
        payload = "".join(payload_list).encode()

        for ele in resp_list[0:payload_index]:
            resp_ascii.extend(ord(num) for num in ele)

        buf_resp_msg = bytes(resp_ascii)
        for byte in buf_resp_msg:
            byte = int(byte)
            PyAppCRC32 = int((PyAppCRC32 >> 8) ^ crc32_table[((PyAppCRC32 & 0xFF) // 2) ^ byte])

        if (debug > 0 or (FirmwareCRC32 is not None and FirmwareCRC32 is not None and FirmwareCRC32 != PyAppCRC32)) and log is not None:
            log.info("Message from firmware=========>%s" % (resp_msg))
            log.info("String extracted to generate python crc32=========>%s" % (buf_resp_msg))
            log.info("FirmwareCRC32: %s || PyAppCRC32:%s" % (hex(FirmwareCRC32), hex(PyAppCRC32)))

        self.run_cnt += 1
        if FirmwareCRC32 == PyAppCRC32:
            return True, payload.decode("utf-8"), FirmwareCRC32, PyAppCRC32
        else:
            return False, payload.decode("utf-8"), FirmwareCRC32, PyAppCRC32
    
    # s must be a string with length equal to N * nibbles, if not conversion will fail
    @staticmethod
    def hexstr_to_list(s, fmt, nibbles=None, offset=0):
        fmt = f'>{fmt}'
        fmt_nibbles = 2 * struct.calcsize(fmt)
        if nibbles is None:
            nibbles = fmt_nibbles
        elif nibbles > fmt_nibbles:
            raise Exception('nibbles is larger than format implies')
        pad = ''
        if nibbles < fmt_nibbles:
            pad = '0' * (fmt_nibbles - nibbles)
        lst = su.wsplit(s, width=nibbles, lpad=pad)
        byte_lst = map(binascii.unhexlify, lst)
        val_lst = map(lambda e: struct.unpack(fmt, e)[0], byte_lst)
        if offset != 0:
            values = list(map(lambda e: e + offset, val_lst))
        else:
            values = list(val_lst)
        return values

    @staticmethod
    def hexstr2_to_numpy_array(s, nchars, offset=0, divider=1, reshape=1):
        pad = ''
        if (nchars % 4) != 0:
            pad = '0' * (4 - (nchars % 4))
        lst = su.wsplit(s, width=nchars, lpad=pad)
        byte_lst = map(binascii.unhexlify, lst)
        val_lst = list(map(lambda e: int.from_bytes(e, byteorder='big', signed=False), byte_lst))
        if divider != 1:
            values = list(map(lambda e: int(e / divider), val_lst))
        else:
            values = list(val_lst)
        if offset != 0:
            val_lst = list(map(lambda e: e + offset, values))
        else:
            val_lst = list(values)
        vs = np.fromiter(val_lst, int)
        if reshape == 1:
            return np.reshape(vs, (vs.shape[0], 1))
        else:
            return vs

    @staticmethod
    def basestr_to_numpy_array(s, nchars, offset=0, divider=1, reshape=1):
        pad = ''
        if (nchars % 4) != 0:
            pad = 'A' * (4 - (nchars % 4))
        lst = su.wsplit(s, width=nchars, lpad=pad)
        byte_lst = map(base64.b64decode, lst)
        val_lst = map(lambda e: int.from_bytes(e, byteorder='big', signed=False), byte_lst)
        if divider != 1:
            values = list(map(lambda e: int(e / divider), val_lst))
        else:
            values = list(val_lst)
        if offset != 0:
            val_lst = list(map(lambda e: e + offset, values))
        else:
            val_lst = list(values)
        vs = np.fromiter(val_lst, int)  # fromiter and reshape is faster then using vstack
        if reshape == 1:
            return np.reshape(vs, (vs.shape[0], 1))
        else:
            return vs

    @staticmethod
    def basestr_to_float_numpy_array(s, nchars, fp_factor=0):
        pad = ''
        if (nchars % 4) != 0:
            pad = 'A' * (4 - (nchars % 4))
        lst = su.wsplit(s, width=nchars, lpad=pad)
        bytelst = list(map(base64.b64decode, lst))
        vallst1 = list(map(lambda e: int.from_bytes(e, byteorder='big', signed=False), bytelst))
        # Assuming signed short as input
        vallst = list(map(lambda e: e-0x10000 if (e >= 0x8000) else e, vallst1))
        if fp_factor != 0:
            vallst = list(map(lambda e: np.float(np.int16(e) / (2**fp_factor)), vallst))

        return np.vstack(np.asarray(vallst))
    
    def parse_result(self, resp_msg, debug=0, log=None) -> NestedDict:
        result = NestedDict()
        # Append the timestamp of the result received
        result['host.timestamp'] = resp_msg.splitlines()[-1].split(':')[-1]
        
        resp_msg = resp_msg.strip(' \r\n\t')
        crc_status, payload, FirmwareCRC32, PyAppCRC32 = self.check_CRC32(resp_msg, debug, log)
        if not crc_status:
            print(f"Frame [{self.run_cnt}] CRC FAILED")
            return None
        
        field_rename = {
                'v': 'sw_version',
                'u': 'unique_id',
                'g': 'agc_idx',
                'c': 'cfo',
                'r': 'rssi',
                'rr': 'rssi_raw',
                'o': 't_sync_offset',
                'ta': 't_intra',
                'tr': 't_inter',
                'tp': 't_intra_antenna',
                'ad': 'distance',
                'dm': 'in_cot',
                'sr': 'successrate',
                'cde': 'CDE_distance',
                'cqi': 'CDE_dqi',
                'rade': 'RADE',
                'rade_trk': 'RADE_trk',
                'rade_dqi': 'RADE_dqi',
                'raderr': 'RADE_error',
                'cp': 'COMBINED_PHASE',
                'x': 'xtal_trim',
                'te': 'temperature',
                'ta': 'time_adj',
                'gd': 'group_delay',
                'nv': 'n_valid',
                'f': 'flags',
                'md': 'modes',
                'mdr': 'modesRemote',
                'ch': 'channels',
                'acl': 'startAclCnt',
                'ci': 'connection_interval',
                'si': 'subevent_interval',
                'senb': 'subevent_number',
                'mmdrp': 'mainmode_repeat',
                'mmdt': 'mainmode_type',
                'smdt': 'submode_type',
                'se': 'subevt',
                'evt': 'event',
                'syc': 'sync_cfo',
                'syg': 'sync_agc',
                'syr': 'sync_rssi',
                'syn': 'sync_step_id'
        }

        # func, bytes per record, bias, divider, reshape
        field_convert = {
            'i': [ResultParser.basestr_to_numpy_array, 2, -2048, 1, 1],
            'q': [ResultParser.basestr_to_numpy_array, 2, -2048, 1, 1],
            'tqi': [ResultParser.hexstr2_to_numpy_array, 2, 0, 1, 1],
            'iq': [ResultParser.basestr_to_numpy_array, 2, -2048, 1, 2],
            'ifdx': [ResultParser.basestr_to_numpy_array, 2, -512, 4, 2],
            'd': [ResultParser.basestr_to_numpy_array, 4, 0, 1, 0],
            'COMBINED_PHASE': [ResultParser.basestr_to_float_numpy_array, 3, 15],
            'modes': [ResultParser.hexstr2_to_numpy_array, 1, 0, 1, 0],
            'modesRemote': [ResultParser.hexstr2_to_numpy_array, 1, 0, 1, 0],
            'channels': [ResultParser.hexstr2_to_numpy_array, 2, 0, 1, 0],
            'subevt': [ResultParser.hexstr2_to_numpy_array, 2, 0, 1, 0],
            'event': [ResultParser.hexstr2_to_numpy_array, 2, 0, 1, 0],
            'rssi': [ResultParser.hexstr2_to_numpy_array, 2, -0x80, 1, 0],
            'rpl': [ResultParser.hexstr2_to_numpy_array, 2, -0x80, 1, 0],
            'sts': [ResultParser.hexstr2_to_numpy_array, 2, 0, 1, 0],
            'abt':[ResultParser.hexstr2_to_numpy_array, 2, 0, 1, 0],
            'cfo': [ResultParser.hexstr2_to_numpy_array, 4, -0x8000, 1, 0],
            'rttstat': [ResultParser.basestr_to_numpy_array, 5, 0, 1, 0],
            'nadm': [ResultParser.hexstr2_to_numpy_array, 1, 0, 1, 0]
        }
        
        payload = payload.replace(':', ': ')
        try:
            dct = yaml.load(payload, Loader=yaml.FullLoader)
            assert isinstance(dct, dict), 'must be a dict'
        except Exception as e:
            if debug > 0:
                log.info("Message from firmware=========>%s" % (payload))
                if FirmwareCRC32 != None:
                    log.info("FirmwareCRC32: %s || PyAppCRC32:%s" % (hex(FirmwareCRC32), hex(PyAppCRC32)))
            return None
        
        for item in dct['items']:
            item = NestedDict(item)

            if 'profiling' in dct:
                result['meta.platform_profiling'] = dct['profiling']
            try:
                print(f"Frame [{self.run_cnt}] CRC PASSED | PrintTime: {dct['items'][0]['PrintTimeUs']} us")
            except Exception as e:
                log.info("Data PrintTime needed")
            # remap
            for a, b in zip(['initiator', 'reflector'], ['init', 'refl']):
                result[f'meta.{a}.sw_version'] = item.get(f'{b}.v')
                result[f'meta.{a}.unique_id'] = item.get(f'{b}.u')
                result[f'meta.{a}.zdc_mciq'] = item.get(f'{b}.zdm', 0) / (2**10)
                result[f'meta.{a}.zdc_tof'] = item.get(f'{b}.zdt', 0) / (2**10)

            result['mciq'] = item.get('mciq', {})
            result['tof'] = item.get('tof', {})
            result['md0'] = item.get('md0', {})
            result['dbg'] = item.get('dbg', {})
            result['hadm'] = item.get('cs', {})
            result['info'] = item.get('info', {})

            for hierarchy in list(itertools.product(['md0', 'tof', 'mciq', 'dbg', 'info'], ['cfg', 'init', 'refl', 'result'])) + list(itertools.product(['hadm'], ['stp'])):
                key = '.'.join(hierarchy)
                if key not in result:
                    continue
                for k in list(result[key].keys()):
                    fieldkey = '.'.join(hierarchy + (k,))
                    # rename fields
                    if k in field_rename.keys():
                        renamed_fieldkey = '.'.join(hierarchy + (field_rename[k],))
                        result[renamed_fieldkey] = result.pop(fieldkey)
                        fieldkey = renamed_fieldkey
                        k = field_rename[k]
                    # convert data from encoded format
                    if k in field_convert and field_convert[k][0] is not None:
                        try:
                            func, nchars, offs, div, reshape = field_convert[k]
                            values = result[fieldkey]
                            if not isinstance(values, list):
                                values = [values]
                            new_values = []
                            for n, val in enumerate(values):
                                data_lst = func(val, nchars, offs, div, reshape)
                                if reshape == 1:  # Reshaped list
                                    # Build reshaped nparray
                                    if len(new_values) != 0:
                                        new_values = np.hstack((new_values, data_lst))
                                    else:
                                        new_values = data_lst
                                elif reshape == 2:  # list of lists
                                    # Simply append list into a list
                                    new_values.append(data_lst)
                                else:  # list of single values
                                    new_values = data_lst

                            result[fieldkey] = new_values
                        except Exception as e:
                            pass
            result['tof.cfg.n_ap'] = result.get('tof.cfg.n_ap', 1)
            v = True if result.get('tof.result.in_cot', 1) else False
            result['tof.result.in_cot'] = v
            # Special handling of ToF values that contains aggregated information (TS + Quality indicator)
            if 'tof.init.d' in result:
                result['tof.init.quality'] = np.right_shift(np.bitwise_and(result['tof.init.d'], 0xf00000), 20)
                result['tof.init.d'] = np.array(np.bitwise_and(result['tof.init.d'], 0xffff),
                                                dtype=np.uint16).astype(np.int16, casting='same_kind')
            if 'tof.refl.d' in result:
                result['tof.refl.quality'] = np.right_shift(np.bitwise_and(result['tof.refl.d'], 0xf00000), 20)
                result['tof.refl.d'] = np.array(np.bitwise_and(result['tof.refl.d'], 0xffff),
                                                dtype=np.uint16).astype(np.int16, casting='same_kind')

            # rename init/refl
            for section in ['mciq', 'tof']:
                result[f'{section}.initiator'] = result.pop(f'{section}.init', {})
                result[f'{section}.reflector'] = result.pop(f'{section}.refl', {})
        
        return result