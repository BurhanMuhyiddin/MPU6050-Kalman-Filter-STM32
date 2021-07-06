from collections import deque, namedtuple
from threading import Thread, Lock
import time
import struct
import serial
import matplotlib.pyplot as plt
import matplotlib.animation as animation


class SerialPlotter:
    def __init__(self, scStruct, spStruct):
        self.port = scStruct.portName
        self.baud = scStruct.baudRate
        self.dataNumBytes = scStruct.dataNumBytes
        self.plotMaxLength = spStruct.maxPlotLength
        self.numPlots = spStruct.numPlots

        self.rawData = bytearray(scStruct.dataNumBytes)
        self.data = []
        for i in range(spStruct.numPlots):
            self.data.append(
                deque([0] * spStruct.maxPlotLength, maxlen=spStruct.maxPlotLength))

        self.isRun = True
        self.isReceiving = False
        self.thread = None
        self.serialConnection = None

        print('Connect to: ' + scStruct.portName +
              ' at ' + str(scStruct.baudRate) + ' Baud...')
        try:
            self.serialConnection = serial.Serial(
                scStruct.portName, scStruct.baudRate, timeout=4)
            print('Connected to:' + scStruct.portName +
                  ' at ' + str(scStruct.baudRate) + ' Baud...')
        except:
            print('Failed to connect to:' + scStruct.portName +
                  ' at ' + str(scStruct.baudRate) + ' Baud...')

    def readSerialStart(self):
        if self.thread == None:
            self.thread = Thread(target=self.backgroundThread)
            self.thread.start()
            while self.isReceiving != True:
                time.sleep(0.1)

    def backgroundThread(self):
        time.sleep(1.0)
        self.serialConnection.reset_input_buffer()
        while self.isRun:
            self.serialConnection.readinto(self.rawData)
            self.isReceiving = True

    def getSerialData(self, frame, lines, lineLabel):
        value = struct.unpack('ffffff', self.rawData)
        print(value)
        for i in range(self.numPlots):
            self.data[i].append(value[i+self.numPlots])
            lines[i].set_data(range(self.plotMaxLength), self.data[i])

    def close(self):
        self.isRun = False
        self.thread.join()
        self.serialConnection.close()
        print('Disconnected...')


def main():
    SeriaConnectionStruct = namedtuple(
        "scStruct", "portName, baudRate, dataNumBytes")
    SerialPlotterStruct = namedtuple("spStruct", "numPlots, maxPlotLength")

    sc1 = SeriaConnectionStruct(
        portName='/dev/ttyUSB0', baudRate=115200, dataNumBytes=24)
    sp1 = SerialPlotterStruct(numPlots=3, maxPlotLength=100)

    s = SerialPlotter(sc1, sp1)
    s.readSerialStart()

    FigurePropertiesStruct = namedtuple(
        "fpStruct", "figTitle, xTitle, yTitle, legend, xRange, yRange, figSize")
    fp1 = FigurePropertiesStruct(
        figTitle="Tilt Angle", xTitle="t", yTitle="acc angle(rad)", legend=('roll', 'pitch', 'yaw'),
        xRange=(0, sp1.maxPlotLength), yRange=(-3.14, 3.14), figSize=(10, 8))

    fig = plt.figure(figsize=fp1.figSize)
    ymin = fp1.yRange[0]
    ymax = fp1.yRange[1]
    ax = plt.axes(xlim=fp1.xRange, ylim=(
        float(ymin-(ymax-ymin)/10), float(ymax+(ymax-ymin)/10)))
    ax.set_title(fp1.figTitle)
    ax.set_xlabel(fp1.xTitle)
    ax.set_ylabel(fp1.yTitle)

    lineLabel = ['roll', 'pitch', 'yaw']
    style = ['r-', 'c-', 'b-']  # linestyles for the different plots
    lines = []
    for i in range(sp1.numPlots):
        lines.append(ax.plot([], [], style[i], label=lineLabel[i])[0])
    anim = animation.FuncAnimation(fig, s.getSerialData, fargs=(
        lines, lineLabel), interval=10)

    try:
        plt.legend(loc="upper left")
        plt.show()
    except:
        s.close()


if __name__ == '__main__':
    main()
