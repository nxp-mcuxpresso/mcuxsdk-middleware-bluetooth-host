# Copyright : Stichting imec Nederland (http://www.imec.nl)
#------------------------------------------------------------------------------
#  Copyright 2020-2024 NXP
#  All rights reserved.
#
#  SPDX-License-Identifier: BSD-3-Clause
#------------------------------------------------------------------------------


class NestedDict:
    SEP = '.'

    def _keys_(self, key):
        if key == '':
            return []
        return key.split(self.SEP)

    def __str__(self):
        return str(self.__dict__)
    __repr__ = __str__

    def __init__(self, *args):
        self.__dict__ = dict()
        if len(args) and isinstance(args[0], dict):
            for key, value in args[0].items():
                self[key] = value

    def clear(self):
        self.__dict__ = dict()

    def pop(self, key, *default):
        try:
            value = self.__getitem__(key)
            self.__delitem__(key)
            return value
        except (KeyError, TypeError):
            if len(default) == 0:
                raise KeyError
            return default[0]

    def setdefault(self, key, default=None):
        try:
            return self.__getitem__(key)
        except (KeyError, TypeError):
            pass
        try:
            self.__setitem__(key, default)
        except (KeyError, TypeError):
            pass
        return default

    def get(self, key, default=None):
        try:
            value = self.__getitem__(key)
        except (KeyError, TypeError):
            value = default
        return value

    def __setitem__(self, key, value):
        keys = self._keys_(key)
        if len(keys) == 0:
            self.__dict__ = value
            return
        dct = self.__dict__
        for k in keys[:-1]:
            if k not in dct:
                dct[k] = dict()
            elif not isinstance(dct[k], dict):  # overwrite
                dct[k] = dict()
            dct = dct[k]
        dct[keys[-1]] = value

    def __getitem__(self, key):
        try:
            dct = self.__dict__
            keys = self._keys_(key)
            for k in keys:
                dct = dct[k]
            return dct
        except (TypeError, KeyError):
            raise KeyError

    def __delitem__(self, key):
        try:
            dct = self.__dict__
            keys = self._keys_(key)
            for k in keys[:-1]:
                dct = dct[k]
            del dct[keys[-1]]
        except (TypeError, KeyError):
            raise KeyError

    def __contains__(self, key):
        try:
            self.__getitem__(key)
            return True
        except (TypeError, KeyError):
            return False

    def update(self, key, others):
        prefix = '' if key == '' else f'{key}{self.SEP}'
        for k, v in others.items():
            self.__setitem__(f'{prefix}{k}', v)

    def items(self, *key):
        if len(key) == 0:
            prefix = ''
            dct = self.__dict__
        else:
            prefix = key[0] + self.SEP
            dct = self[key[0]]
        for k in dct.keys():
            if not isinstance(dct[k], dict):
                yield prefix+k, dct[k]
            else:
                yield from self.items(prefix+k)

    def keys(self, *key):
        if len(key) == 0:
            prefix = ''
            dct = self.__dict__
        else:
            prefix = key[0] + self.SEP
            dct = self[key[0]]
        for k in dct.keys():
            if not isinstance(dct[k], dict):
                yield prefix+k
            else:
                yield from self.keys(prefix+k)

if __name__ == '__main__':
    dct = {
        'init': {
            'v': 'v0.129.3640',
            'u': '0x00000060373881784E4557527101010C'
        },
        'refl': {
            'v': 'v0.129.3640',
            'u': '0x00000060373880684E45575271010157'
        },
        'profiling': {
            't_print': 24166,
            'systick': 4165776
        },
        'marker': ['DONE']
    }
    x = NestedDict(dct)
    x['test'] = 'new'
    x['test1.a'] = 'new'
    x['test1']['a'] = 'new'
    du.rset(x, 'test1', 'a')
    x['test1.b'] = 'new'
    x['test2.a.a'] = 'new'
    x['test2.a.b'] = 'new'
    x['dtest'] = {'bra': 3, 'cra': 5}
    x['dtest'] = {'bra': 9, 'cra': 10, }
    x['dtest'] = {}
    print(x['profiling.t_print'])
    x['profiling.systick'] = 12
    # print(x['profiling.systick1'])
    del x['profiling.t_print']
    print(x.get('t.t', None))
    # x['profiling.b']
    x.update('', {'xx': 0})
    # print('test2.a.ax' in x)
    print(list(x.keys()))
    x['']={}

    print(x)