from collections import deque
from threading import Thread, Lock
import time
import struct
import serial
import matplotlib.pyplot as plt
import matplotlib.animation as animation


class SerialPlotter:
    def __init__(self, portName='/dev/ttyUSB0', baudRate=115200, maxPlotLength=100, dataNumBytes=24, numPlots=1):
        self.port = portName
        self.baud = baudRate
        self.plotMaxLength = maxPlotLength
        self.dataNumBytes = dataNumBytes
        self.numPlots = numPlots
        self.rawData = bytearray(dataNumBytes)
        self.data = []
        for i in range(numPlots):
            self.data.append(deque([0] * maxPlotLength, maxlen=maxPlotLength))
        self.isRun = True
        self.isReceiving = False
        self.thread = None
        self.serialConnection = None
        self.lock = Lock()

        print('Connect to: ' + portName + ' at ' + str(baudRate) + ' Baud...')
        try:
            self.serialConnection = serial.Serial(
                portName, baudRate, timeout=4)
            print('Connected to:' + portName +
                  ' at ' + str(baudRate) + ' Baud...')
        except:
            print('Failed to connect to:' + portName +
                  ' at ' + str(baudRate) + ' Baud...')

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
        for i in range(self.numPlots):
            self.data[i].append(value[i])
            lines[i].set_data(range(self.plotMaxLength), self.data[i])

    def close(self):
        self.isRun = False
        self.thread.join()
        self.serialConnection.close()
        print('Disconnected...')


def main():
    portName = '/dev/ttyUSB0'
    baudRate = 115200
    maxPlotLength = 100
    dataNumBytes = 24
    numPlots = 3
    s = SerialPlotter(portName, baudRate, maxPlotLength,
                      dataNumBytes, numPlots)
    s.readSerialStart()

    pltInterval = 10
    xmin = 0
    xmax = maxPlotLength
    ymin = -1500
    ymax = 1500
    fig = plt.figure(figsize=(10, 8))
    ax = plt.axes(xlim=(xmin, xmax), ylim=(
        float(ymin-(ymax-ymin)/10), float(ymax+(ymax-ymin)/10)))
    ax.set_title('Accelerometer')
    ax.set_xlabel("time")
    ax.set_ylabel("Accelerometer Output")

    lineLabel = ['X', 'Y', 'Z']
    style = ['r-', 'c-', 'b-']  # linestyles for the different plots
    lines = []
    lineValueText = []
    timeText = ax.text(0.70, 0.95, '', transform=ax.transAxes)
    for i in range(numPlots):
        lines.append(ax.plot([], [], style[i], label=lineLabel[i])[0])
        lineValueText.append(
            ax.text(0.70, 0.90-i*0.05, '', transform=ax.transAxes))
    anim = animation.FuncAnimation(fig, s.getSerialData, fargs=(
        lines, lineLabel), interval=pltInterval)

    try:
        plt.legend(loc="upper left")
        plt.show()
    except:
        s.close()


if __name__ == '__main__':
    main()
