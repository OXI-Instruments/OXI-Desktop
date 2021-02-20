# This Python file uses the following encoding: utf-8
import sys
import os
import argparse

from PySide6.QtGui import QGuiApplication
from PySide6.QtWidgets import QApplication, QDialog
from PySide6.QtQml import QQmlApplicationEngine

from OxiHardware import OxiHardware
from SettingsManager import SettingsManager
from UpdateManager import UpdateManager
from BackupManager import BackupModel

import qml_qrc

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Oxi One companion app')
    parser.add_argument('--device', dest='device', type=str, action="store",
                    help='MIDI device to use for communication')
    parsed_arg, unparsed_arg = parser.parse_known_args()

#app = QGuiApplication(sys.argv)
    app = QApplication(unparsed_arg)
    engine = QQmlApplicationEngine()
    print(f"opening device: {parsed_arg.device}")
    hw = OxiHardware(parsed_arg.device)
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
