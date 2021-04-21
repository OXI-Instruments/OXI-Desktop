# This Python file uses the following encoding: utf-8
from PySide6 import QtCore
from PySide6.QtCore import Qt
from http.client import HTTPConnection
import json
import sqlite3
import typing
from dataclasses import dataclass
from enum import IntEnum, auto


class UpdateManager(QtCore.QObject):

    def __init__(self, db_file=None):
        QtCore.QObject.__init__(self)
        self.updateURL = "https://oxiinstruments.com"
        self.updateDir = "update/oxi_one"
        self.updateMetaData = "latest.json"
        self.db_file = db_file
        self.updates = []
        self.downloadedUpdate = None

    updateAvailableSignal = QtCore.Signal(bool)

    def get_update_history(self):
        db_conn = sqlite3.connect(self.db_file)
        db_cursor = db_conn.cursor()
        db_cursor.execute("SELECT version, date, maturity, source from updates")
        for upd in db_cursor.fetchall():
            self.updates.append(Update(version=upd[0], maturity=upd[1], date=upd[2], source=upd[3]))
        db_conn.close()
        return self.updates

    @QtCore.Slot(result=dict)
    def check_for_update(self):
        try:
            conn = HTTPConnection(self.updateURL)
            conn.request("GET", f"/{self.updateDir}/{self.updateMetaData}")
            res = conn.getresponse()
            dat = json.loads(res.read())
        except HTTPConnection as e:
            self.updateAvailableSignal.emit(False)
            return None
        self.updateAvailableSignal.emit(True)
        return dat["latest"], dat["latest_beta"]

    def download_update(self, version):
        conn = HTTPConnection(self.updateURL)
        conn.request("GET", f"/{self.updateDir}/{version}/oxi_one_{version}.update")
        res = conn.getresponse()
        return res.read()


class UpdateModel(QtCore.QAbstractListModel):
    def __init__(self, *args, updates, **kwargs):
        super(UpdateModel, self).__init__(*args, **kwargs)
        self.updates = updates or [
            ("1.0.1", Update.maturity.STABLE, "2020-12-12", Update.source.FILE),
            ("1.2.0", Update.maturity.STABLE, "2020-12-20", Update.source.FILE),
            ("1.2.4", Update.maturity.BETA, "2020-12-24", Update.source.FILE),
            ("1.3", Update.maturity.STABLE, "2021-01-01", Update.source.ONLINE),
            ("1.4", Update.maturity.STABLE, "2021-01-13", Update.source.ONLINE),
        ]

    def data(self, index: QtCore.QModelIndex, role: int = ...) -> typing.Any:
        if role == Qt.DisplayRole:
            version, maturity, date, source = self.updates[index.row()]
            return version, maturity, date, source

    def rowCount(self, parent: QtCore.QModelIndex = ...) -> int:
        return len(self.updates)


@dataclass
class Update:

    class Maturity(IntEnum):
        BETA = auto()
        STABLE = auto()

    class Source(IntEnum):
        FILE = auto()
        ONLINE = auto()

    version: str
    maturity: Maturity
    source: Source
    date: str
    data: bytearray = None
