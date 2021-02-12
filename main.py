# This Python file uses the following encoding: utf-8
import sys
import os

from PySide6.QtGui import QGuiApplication
from PySide6.QtWidgets import QApplication, QDialog
from PySide6.QtQml import QQmlApplicationEngine

from OxiHardware import OxiHardware
from SettingsManager import SettingsManager
from UpdateManager import UpdateManager
from BackupManager import BackupModel

import qml_qrc

if __name__ == "__main__":
    #app = QGuiApplication(sys.argv)
    app = QApplication(sys.argv)
    engine = QQmlApplicationEngine()
    hw = OxiHardware()
    settings = SettingsManager()
    # update_mgr = UpdateManager("oxi.db")
    backup_model = BackupModel()

#    qmlRegisterSingletonType(QUrl("qrc:ui/style/Theme.qml"), "com.oxiinstruments.theme", 1, 0, "Style")
    engine.rootContext().setContextProperty("hw", hw)
    engine.rootContext().setContextProperty("settings", settings)
    engine.rootContext().setContextProperty("backup_model", backup_model)
    engine.load(os.path.join(os.path.dirname(__file__), "ui/main.qml"))

    if not engine.rootObjects():
        sys.exit(-1)
    sys.exit(app.exec_())
