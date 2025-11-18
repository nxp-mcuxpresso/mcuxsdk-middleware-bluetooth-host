#------------------------------------------------------------------------------
# Copyright : Stichting imec Nederland (http://www.imec.nl)
#             *** IMEC CONFIDENTIAL ***
#------------------------------------------------------------------------------
# NXP Proprietary
#


import sqlite3
import logging
import shutil
import os

log = logging.getLogger()

class sql_db(object):
    def __init__(self, filename, defaults):
        self._cnt = 0
        self.conn = None
        self.values = defaults
        self.modified = {}
        for k,v in list(self.values.items()):
            self.modified[k] = False
        self.open(filename)

    def count(self):
        return self._cnt

    def insert(self, returncode, info, msg):
        self._cnt += 1
        self.execute("INSERT INTO measurement VALUES (%d, '%s', '%s', '%s')" % (self._cnt, returncode, info, msg))

    def get(self, name):
        if name in self.values:
            return self.values[name]
        return None

    def ismodified(self, name):
        if name in self.values:
            return self.modified[name]
        return False

    def isanymodified(self):
        return any(self.modified.values())

    def set(self, name, value):
        if not name in self.values or self.conn is None:
            log.warning("Unable to update db (key=%s)" % name)
            return False
        if type(value) == type(self.values[name]):
            try:
                if isinstance(value, list):
                    value = '|'.join(map(str,value))
                value = str(value).lower()
                self.execute("UPDATE config SET value = '%s' WHERE name = '%s';" % (value, name))
            except sqlite3.Error as e:
                log.error("SQLERROR: %s" % (e.args[0]))
            self.reload()
        else:
            log.warning("Unable to update db, bad type (key=%s)" % name)
        return True

    def reload(self):
        def convert_value(default, value):
            try:
                if isinstance(default, bool):
                    if value == 'true':
                        value = True
                    else:
                        value = False
                elif isinstance(default, int):
                    value = int(value)
                elif isinstance(default, float):
                    value = float(value)
            except:
                pass
            return value

        if self.conn is None:
            return
        for row in self.cursor.execute('SELECT * FROM config'):
            name = str(row[0])
            v = str(row[1])
            if not name in self.values:
                continue
            if isinstance(self.values[name], list):
                v = v.split('|')
                d = [self.values[name][-1]] * len(v)
                v = list(map(convert_value, d, v))
            else:
                v = convert_value(self.values[name], v)
            self.modified[name] = True if v != self.values[name] else False
            self.values[name] = v

    def open(self, filename):
        self.filename = filename
        if filename is None:
            self.conn = None
            self.cursor = None
        elif self.conn is None:
            self.conn = sqlite3.connect(filename)
            self.cursor = self.conn.cursor()
            self.migrate()
            self.reload()
            self.cursor.execute('SELECT * FROM measurement ORDER BY id DESC LIMIT 1')
            result = self.cursor.fetchone()
            if result is None:
                self._cnt = 0
            else:
                self._cnt = result[0]

    def execute(self, query):
        try:
            self.cursor.execute(query)
            self.conn.commit()
        except sqlite3.Error as e:
            log.error("SQLERROR: %s" % (e.args[0]))

    def select(self, query):
        return self.cursor.execute(query)

    def migrate(self):
        try:
            self.execute('''CREATE TABLE config (name TEXT, value TEXT)''')
        except sqlite3.Error as e:
            log.error("SQLERROR: %s" % (e.args[0]))

        try:
            self.execute('''CREATE TABLE measurement (id INTEGER, returncode TEXT, info TEXT,  msg TEXT)''')
        except sqlite3.Error as e:
            log.error("SQLERROR: %s" % (e.args[0]))

        for name, v in list(self.values.items()):
            try:
                self.cursor.execute('SELECT * FROM config WHERE name=?', (name,))
                result = self.cursor.fetchone()
                if result is None:
                    if isinstance(v, list):
                        v = '|'.join(map(str,v))
                    v = str(v).lower()
                    self.execute("INSERT INTO config VALUES ('%s', '%s')" % (name, v))
            except sqlite3.Error as e:
                log.error("SQLERROR: %s" % (e.args[0]))

    def close(self):
        if not self.conn is None:
            self.conn.close()
        self.conn = None

    def rotate(self):
        for e in range(256):
            fn = self.filename + '.' + str(e)
            if not os.path.isfile(fn):
                shutil.copy2(self.filename, fn)
                break
        self.close()
        with open(self.filename, 'wb') as fh:
            fh.truncate()
        self.open(self.filename)

    def __repr__(self):
        s = ""
        s += "CONFIG:\n"
        for k, v in list(self.values.items()):
            s += " - NAME: %s, VALUE: %s\n" % (k, str(v))
        s += "Measurements: %d\n" % self.count()
        return s
    __str__ = __repr__

    def dump_data(self):
        for row in self.cursor.execute('SELECT * FROM measurement'):
            print(("id: %d, returncode: %s, info: %s, msg: %s" % (row[0], row[1], row[2], row[3])))
    def write_log(self, filename):
        with open(filename, 'wb') as fh:
            for row in self.cursor.execute('SELECT id, msg FROM measurement ORDER BY id ASC'):
                fh.write('\n=== %d ===\n' % row[0])
                fh.write(row[1])

if __name__ == '__main__':
    ch = logging.StreamHandler()
    ch.setLevel(logging.INFO)

    # add ch to logger
    log.addHandler(ch)

    filename = 'ranging.db'
    defaults = {
        'board_number_lst': list(range(8)),
        'board': 'kw36',
        'retry_delay': 1.0,
        'activity_depth' : 3,
        'remote_board_number': 1,
        'unit_feet': False,
        'plot_raw': False,
        'max_values': 40,
        'y_axis_range': 0.0,
        'processing_enable': True,
        'processing_offset': 1.20,
        'processing_max': 20.0,
        'processing_jump_threshold': 2.0,
        'logging_level': 0,
    }
    db = sql_db(filename, defaults)
    #print db
    #db.set('remote_board_number', 5)
    #db.set('plot_raw', True)
    #db.insert('ok', 1.0, 2.0, '')
    #print db
    #db.dump_data()
    db.write_log('raw.log')