# This Python file uses the following encoding: utf-8
from PySide6 import QtCore
import mido


class OxiHardware(QtCore.QObject):
    def __init__(self):
        QtCore.QObject.__init__(self)
        self.timer = QtCore.QTimer()
        self.timer.timeout.connect(lambda: self.detectDevice())
        self.timer.start(800)
        self.deviceName = "Deluge"
        self.port = ""

    isConnected = QtCore.Signal(bool)

    @QtCore.Slot(bool)
    def detectDevice(self):
        for device in mido.get_ioport_names():
            if self.deviceName in device:
                self.port = device
                self.isConnected.emit(True)
                return
        self.isConnected.emit(True)

    def startBackup(self, file):
        if not self.isConnected:
            return False

    def startUpdate(self, filePath):
        if not self.isConnected:
            return False
        self.timer.stop()
        data = mido.read_syx_file(filePath)
        port = mido.open_output(self.port)
        port.send(data)

