# This Python file uses the following encoding: utf-8
from PySide6 import QtCore
import mido
import sys
from dataclasses import dataclass


class RequestMessages:
    GET_VERSION = b"\xf0\xc0\xfe\x00\x10"
    HEARTBEAT = b"\xf0"


class ReceiveMessages:
    VERSION = bytearray([144, 62, 64])
    MIDI_SEND_CLOCK = [144, 64, 64]

    @staticmethod
    def parse_version(ver: bytearray):
        return "1.4.42"


class HardwareDisconnectException(Exception):
    def __init__(self, expression, message):
        self.expression = expression
        self.message = "Hardware disconnected"
        pass


class MidiLoop(QtCore.QThread):
    def __init__(self, port_name: str, hw):
        super(MidiLoop, self).__init__()
        self.port_name = port_name
        self.hw = hw
        self.mode = 0

    def __del__(self):
        self.wait()
        # self.quit()

    def run(self) -> None:
        try:
            with mido.open_input(self.port_name) as port:
                for msg in port:
                    if msg.type == "note_on":
                        # print(msg.bytes())
                        if msg.bin() == ReceiveMessages.VERSION:
                            version = ReceiveMessages.parse_version(msg.bytes)
                            self.hw.version.emit(version)
                            print(f"it should emit {version}")
                            sys.stdout.flush()
                        elif msg.bytes()[0:2] == ReceiveMessages.MIDI_SEND_CLOCK:
                            pass
        except Exception as e:
            print("port open failed!")


class OxiHardware(QtCore.QObject):

    def __init__(self):
        QtCore.QObject.__init__(self)
        self.timer = QtCore.QTimer()
        self.timer.timeout.connect(lambda: self.detectDevice())
        self.timer.start(800)
        self.deviceName = "Deluge"
        self.port = ""
        self.midi_loop = MidiLoop("Deluge MIDI 1", self)
        self.midi_loop.start()

    isConnected = QtCore.Signal(bool)
    version = QtCore.Signal(str)

    @QtCore.Slot(bool)
    def detectDevice(self):
        for device in mido.get_ioport_names():
            if self.deviceName in device:
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

    def start_update(self, file_path):
        if not self.isConnected:
            raise HardwareDisconnectException
        self.timer.stop()
        data = mido.read_syx_file(file_path)
        port = mido.open_output(self.port)
        port.send(data)

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
