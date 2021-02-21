# This Python file uses the following encoding: utf-8
import time

from PySide6 import QtCore
import mido
import sys
from dataclasses import dataclass


class RequestMessages:
    GET_VERSION = b"\xf0\xc0\xfe\x00\x10"
    HEARTBEAT = b"\xf0"


class ReceiveMessages:
    VERSION = bytearray([144, 62, 64])
    UPDATE_READY = bytearray([0xf0, 0x43])

    @staticmethod
    def parse_version(ver: bytearray):
        return "1.4.42"


class HardwareDisconnectException(Exception):
    def __init__(self, expression, message):
        self.expression = expression
        self.message = "Hardware disconnected"
        pass


class MidiLoopSignals(QtCore.QObject):
    inUpdateMode = QtCore.Signal()
    version = QtCore.Signal(str)


class MidiLoop(QtCore.QRunnable):

    def __init__(self, port_name: str):
        super().__init__()
        self.port_name = port_name
        self.signals = MidiLoopSignals()

    @QtCore.Slot()
    def run(self) -> None:
        try:
            with mido.open_input(self.port_name) as port:
                for msg in port:
                    if msg.type == "sysex":
                        print(msg.bin())
                        if msg.bin().startswith(ReceiveMessages.UPDATE_READY):
                            self.signals.inUpdateMode.emit()
                    elif msg.type == "note_on":
                        print(msg.bytes())
                        if msg.bin().startswith(ReceiveMessages.VERSION):
                            version = ReceiveMessages.parse_version(msg.bytes)
                            self.signals.version.emit(version)
                            sys.stdout.flush()
                        elif msg.bytes()[0:2] == ReceiveMessages.MIDI_SEND_CLOCK:
                            pass
        except Exception as e:
            print("port open failed!")


class OxiHardware(QtCore.QObject):

    def __init__(self, port: str = None):
        QtCore.QObject.__init__(self)
        self.timer = QtCore.QTimer()
        self.timer.timeout.connect(lambda: self.detectDevice())
        self.timer.start(800)
        self.deviceSearchName = port or "Deluge"
        self.port = port
        self.updateFile = None

        self.thread_pool = QtCore.QThreadPool()
        self.midi_loop = MidiLoop(self.port)
        self.midi_loop.signals.inUpdateMode.connect(self.__send_update_data)
        self.midi_loop.signals.version.connect(lambda x: self.__setVersion(x))
        # self.th.finished.connect(app.quit) TODO: can this be used for clean exit?
        self.thread_pool.start(self.midi_loop)

    isConnected = QtCore.Signal(bool)
    version = QtCore.Signal(str)
    inUpdateMode = QtCore.Signal(bool)

    @QtCore.Slot(str)
    def __setVersion(self, version):
        self.version.emit(version)

    @QtCore.Slot(bool)
    def detectDevice(self):
        for device in mido.get_ioport_names():
            if self.deviceSearchName in device:
                if not self.port:
                    self.port = device
                self.isConnected.emit(True)
                return
        self.isConnected.emit(True)

    def start_backup(self):
        print("start_backup is a stub!")
        pass

    def restore_backup(self, data: bytearray):
        print("restore_backup is a stub!")
        pass

    @QtCore.Slot(str)
    def start_update(self, file_path):
        if self.isConnected == False:
            raise HardwareDisconnectException
        self.timer.stop()
        self.updateFile = file_path
        # TODO: put oxi into update mode

    @QtCore.Slot()
    def __send_update_data(self):
        print("totally sending the update!!1")
        if self.updateFile:
            pass
            data = mido.read_syx_file(self.updateFile)
            port = mido.open_output(self.port)
            # skip ack for now
            for pkg in data:
                port.send(pkg)
                time.sleep(.1)
            self.timer.start(800)
            self.updateFile = None
            # #detect last package

    def get_version(self):
        with mido.open_output(self.port) as midi_out:
            msg = mido.Message("sysex", data=RequestMessages.GET_VERSION)
            midi_out.send(msg)


@dataclass
class OxiSeqPreset:
    is_empty: bool
    color_variation: int


@dataclass
class OxiProject:
    name: str
    number: int
    tempo: int
    seq_presets: [OxiSeqPreset]
