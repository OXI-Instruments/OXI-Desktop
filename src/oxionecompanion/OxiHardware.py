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
    UPDATE_READY = bytearray([0xf0, 0x7f, 0x43])

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
        self.cancel = False

    @QtCore.Slot()
    def run(self) -> None:
        try:
            with mido.open_input(self.port_name) as port:
                while True:
                    for msg in port.iter_pending():
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
                    if self.cancel:
                        print("We should return...")
                        return
        except Exception as e:
            print(e)
            print("port open failed!")


class MidiWorkerSignals(QtCore.QObject):
    progress = QtCore.Signal(float)
    finished = QtCore.Signal()


class MidiDumpWorker(QtCore.QRunnable):

    def __init__(self, port_name: str, data: list):
        super().__init__()
        self.port_name = port_name
        self.data = data
        self.progress = 0
        self.signals = MidiWorkerSignals()

    @QtCore.Slot()
    def run(self) -> None:
        try:
            with mido.open_output(self.port_name) as port:
                sent = 0
                for pkg in self.data:
                    print(f"sending {sent} of {len(self.data)-1} messages")
                    port.send(pkg)
                    time.sleep(.2)
                    sent += 1
                    self.progress = 1/(len(self.data)-1)*sent
                    self.signals.progress.emit(self.progress)
                self.signals.finished.emit()
        except Exception as e:
            print("Failed to send data")
            print(e)


class OxiHardware(QtCore.QObject):

    def __init__(self, port: str = None):
        QtCore.QObject.__init__(self)
        self.continuous_device_detection = QtCore.QTimer()
        self.continuous_device_detection.timeout.connect(lambda: self.detectDevice())
        self.continuous_device_detection.start(800)
        self.device_search_name = port or "OXI"
        self.wait_for_device = True
        self.port = port
        self.update_file = None
        self.thread_pool = None
        self.midi_loop = None
        self.connected = False
        self.progress = 0
        self.isConnectedSignal.connect(lambda state: self.__manage_midi_loop(state))

    isConnectedSignal = QtCore.Signal(bool)
    versionSignal = QtCore.Signal(str)
    inUpdateModeSignal = QtCore.Signal(bool)
    updateStartSignal = QtCore.Signal()
    progressSignal = QtCore.Signal(float)
    updateFinishedSignal = QtCore.Signal()

    def __manage_midi_loop(self, device_connected):
        if device_connected:
            if not self.thread_pool:
                self.thread_pool = QtCore.QThreadPool()
            if self.thread_pool.activeThreadCount() == 0:
                self.midi_loop = MidiLoop(self.port)
                if self.wait_for_device:
                    self.midi_loop.signals.inUpdateMode.connect(self.__send_update_data)
                self.midi_loop.signals.version.connect(self.versionSignal.emit)
                # self.midi_loop.signals.version.connect(lambda ver: self.__emitVersion(ver))
                self.thread_pool.start(self.midi_loop)
        elif self.thread_pool:
            self.midi_loop.cancel = True
            while self.thread_pool.activeThreadCount() > 0:
                time.sleep(.01)
            del self.midi_loop

    @QtCore.Slot(str)
    def __emitVersion(self, version: str):
        self.versionSignal.emit(version)

    @QtCore.Slot(float)
    def __emitProgress(self, progress: float):
        self.progressSignal.emit(progress)

    @QtCore.Slot(bool)
    def detectDevice(self):
        for device in mido.get_ioport_names():
            if self.device_search_name in device:
                if not self.port:
                    self.port = device
                if not self.connected:
                    self.connected = True
                    self.isConnectedSignal.emit(True)
                return
        if self.connected:
            self.connected = False
            self.isConnectedSignal.emit(False)

    def start_backup(self):
        print("start_backup is a stub!")
        pass

    def restore_backup(self, data: bytearray):
        print("restore_backup is a stub!")
        pass

    @QtCore.Slot(str)
    def start_update(self, file_path):
        if not self.connected:
            raise HardwareDisconnectException
        self.continuous_device_detection.stop()
        self.update_file = file_path
        if not self.wait_for_device:
            self.__send_update_data()
        # TODO: put oxi into update mode

    @QtCore.Slot()
    def __send_update_data(self):
        self.updateStartSignal.emit()
        if self.update_file:
            pass
            data = mido.read_syx_file(self.update_file)
            # skip ack for now
            worker = MidiDumpWorker(self.port, data)
            worker.signals.progress.connect(self.progressSignal)
            # worker.signals.progress.connect(lambda progress: self.__emitProgress(progress))
            worker.signals.finished.connect(lambda: self.__cleanup_after_update())
            self.thread_pool.start(worker)
            #TODO detect last package -> why?
            #TODO: startup message?

    @QtCore.Slot()
    def __cleanup_after_update(self):
        print("Update cleanup started")
        self.continuous_device_detection.start(800)
        self.update_file = None

    def get_version(self):
        with mido.open_output(self.port) as midi_out:
            msg = mido.Message("sysex", data=RequestMessages.GET_VERSION)
            midi_out.send(msg)

    @QtCore.Slot()
    def stop_communication(self):
        if self.midi_loop:
            self.midi_loop.cancel = True


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
