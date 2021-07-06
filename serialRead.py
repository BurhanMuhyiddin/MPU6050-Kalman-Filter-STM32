import serial
import struct
import sys

try:
    ser = serial.Serial('/dev/ttyUSB0', 115200, timeout=4)

    ser.reset_input_buffer()
    print("Successful connection...\n")

    while (1):
        rawData = bytearray(24)
        ser.readinto(rawData)
        value = struct.unpack('ffffff', rawData)
        #print(value[0], '   ', value[1], '   ', value[2], '   ')
        print(value)
except:
    print(sys.exc_info()[1])
