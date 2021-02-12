from PySide6 import QtCore
from PySide6.QtCore import Qt
import typing
from dataclasses import dataclass
from enum import IntEnum, auto
from pprint import pprint
import sqlite3


class BackupModel(QtCore.QAbstractListModel):
    version_role = Qt.UserRole + 1
    date_role = Qt.UserRole + 2
    name_role = Qt.UserRole + 3
    compatible_version_role = Qt.UserRole + 4
    cause_role = Qt.UserRole + 5
    restored_role = Qt.UserRole + 6
    dt_restored_role = Qt.UserRole + 7
    data_role = Qt.UserRole + 8

    def __insert_testdata(self):
        db_conn = sqlite3.connect("oxi.db")
        c = db_conn.execute("select count(*) from backups limit 1")
        res = c.fetchone()
        if not res[0]:
            data = [
                ["1.0.1", "2020-12-20", "Before 1.2.0 update", "1.0.0", Backup.Cause.FW_UPDATE, False, "", bytearray("foo", "utf-8")],
                ["1.2.0", "2020-12-24", "Before 1.2.4 update", "1.0.0", Backup.Cause.FW_UPDATE, False,  "", bytearray("bar", "utf-8")],
                ["1.2.4", "2020-12-31", "NYE gig", "1.0.0", Backup.Cause.MANUAL, False,  "", bytearray(u"moo", "utf-8")],
                ["1.2.4", "2020-01-01", "Before 1.3 update", "1.0.0", Backup.Cause.FW_UPDATE, True,  "2020-01-02", bytearray("mar", "utf-8")],
                ["1.3", "2020-01-13", "Before 1.4 update", "1.0.0", Backup.Cause.FW_UPDATE,  True, "2020-01-13",  bytearray("lasd;af", "utf-8")],
            ]
            db_conn.executemany("INSERT INTO backups VALUES (NULL, ?, ?, ?, ?, ?, ?, ?, ?)", data)
            db_conn.commit()
            db_conn.close()


    def __init__(self, *args, backups=None, **kwargs):
        super(BackupModel, self).__init__(*args, **kwargs)
        self.__insert_testdata()
        db_conn = sqlite3.connect("oxi.db")
        c = db_conn.execute("SELECT * FROM backups")
        self.backups = []
        for row in c.fetchall():
            self.backups.append(Backup(*row))

    def data(self, index: QtCore.QModelIndex, role: int = ...) -> typing.Any:
        return f"{getattr(self.backups[index.row()], self.roleNames()[role].decode())}"
        # if role == Qt.DisplayRole:
        #     pprint(self.backups(index.row()))
        #     return self.backups(index.row())

    def rowCount(self, parent: QtCore.QModelIndex = ...) -> int:
        return len(self.backups)

    def roleNames(self) -> typing.Dict:
        return {
            BackupModel.version_role: b"version",
            BackupModel.date_role: b"date",
            BackupModel.name_role: b"name",
            BackupModel.compatible_version_role: b"compatible_version",
            BackupModel.cause_role: b"cause",
            BackupModel.restored_role: b"restored",
            BackupModel.dt_restored_role: b"date_restored",
            BackupModel.data_role: b"data"
        }

    @QtCore.Slot(int, result='QVariant')
    def get(self, row):
        if 0 <= row < self.rowCount():
            return self.backups[row]



@dataclass
class Backup:

    class Cause(IntEnum):
        MANUAL = auto()
        FW_UPDATE = auto()
        APP_UPDATE = auto()

    id: int
    version: str
    date: str
    name: str
    compatible_version: str
    cause: Cause
    restored: bool
    date_restored: str
    data: bytearray
