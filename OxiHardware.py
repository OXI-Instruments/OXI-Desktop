# This Python file uses the following encoding: utf-8
from PySide6 import QtCore
import mido


class HardwareDisconnectException(Exception):
    def __init__(self, expression, message):
        self.expression = expression
        self.message = "Hardware disconnected"
        pass


class OxiHardware(QtCore.QObject):

    class Messages:
        GET_VERSION = b"\xf0\xc0\xfe\x00\x10"
        HEARTBEAT = b"\xf0"


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

    def startBackup(self):
        if not self.isConnected:
            raise HardwareDisconnectException

    def restoreBackup(self, data: bytearray):
        if not self.isConnected:
            raise HardwareDisconnectException

    def startUpdate(self, filePath):
        if not self.isConnected:
            raise HardwareDisconnectException
        self.timer.stop()
        data = mido.read_syx_file(filePath)
        port = mido.open_output(self.port)
        port.send(data)

    def getVersion(self):
        port = mido.open_ioport(self.port)
        msg = mido.Message("sysex", data=self.Messages.GET_VERSION)
        for rec_msg in port.iter_pending():
            if rec_msg.type == "sysex" and rec_msg.bin[4] == b"\x01":
                version = rec_msg.bin
                break
        port.send(msg)
        print(version)


