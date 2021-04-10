import sqlite3
import os

from PySide6 import QtCore


class SettingsManager(QtCore.QObject):

    DB_FILE = "oxi.db"
    DB_UPDATE_SCHEMA = """
        CREATE table updates (id, date, version, maturity, source, last_compatible, data)
    """

    DB_BACKUP_SCHEMA = """
        create table backups
        (
            id                 INTEGER
                constraint backups_pk
                    primary key autoincrement,
            version            TEXT,
            date               TEXT,
            name               TEXT,
            compatible_version TEXT,
            cause              INTEGER,
            restored           INTEGER,
            restored_date      TEXT,
            data               BLOB
        );
    """
    DB_BACKUP_PK = """
        create unique index backups_id_uindex
        on backups (id);
    """

    DB_SETTING_SCHEMA = """
        CREATE table settings (key, value)
    """

    DB_DEFAULT_SETTINGS = [
        ("automatic_updates", True),
        ("ask_for_backups", True)
    ]

    def __init__(self):
        QtCore.QObject.__init__(self)
        if not os.path.isfile(self.DB_FILE):
            self.__initialize_database()

    def __initialize_database(self):
        db_connection = sqlite3.connect(self.DB_FILE)
        db_cursor = db_connection.cursor()
        db_cursor.execute(self.DB_SETTING_SCHEMA)
        db_cursor.execute(self.DB_UPDATE_SCHEMA)
        db_cursor.execute(self.DB_BACKUP_SCHEMA)
        db_cursor.execute(self.DB_BACKUP_PK)
        db_cursor.executemany("INSERT into settings(key, value) values (?, ?)", self.DB_DEFAULT_SETTINGS)
        db_connection.commit()
        db_connection.close()
        pass

    def __get_setting_by_key(self, key):
        db_connection = sqlite3.connect(self.DB_FILE)
        db_cursor = db_connection.cursor()
        db_cursor.execute("SELECT value FROM settings where key = (?)", key)
        data = db_cursor.fetchone()
        db_connection.close()
        return data[0]

    @property
    @QtCore.Slot(bool)
    def automatic_update(self):
        return bool(self.__get_setting_by_key("enable_automatic_updates"))

    @property
    @QtCore.Slot(bool)
    def ask_for_backups(self):
        #TODO: create cleaner mapping to object
        return bool(self.__get_setting_by_key("ask_for_backups"))

    @QtCore.Slot(str, result=str)
    def url_to_path(self, url):
        qurl = QtCore.QUrl(url)
        return qurl.path()
