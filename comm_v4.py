"""This program is for roarm """

from itertools import starmap
import serial
import struct
import keyboard
import csv
from datetime import datetime
from sys import stdout
from Check import *
from slider2 import *

class SerialPort(object):
    # Contains functions that enable communication between the docking station and the roarm watches

    def __init__(self, serialport, serialrate=115200, csv_path="", csv_enable=False):
        # Initialise serial payload
        self.count = 0
        self.plSz = 0
        self.payload = bytearray()

        self.serialport = serialport
        self.ser_port = serial.Serial(serialport, serialrate)
        
        self.csv_enabled = csv_enable
        if csv_enable:
            self.csv_file = open(csv_path + "//roarm01.csv", "w")
            self.csv = csv.writer(self.csv_file)
            self.csv.writerow(["sys_time", "theta1", "theta2", "theta3"])
        self.triggered = True
        self.connected = False
        
        stdout.write("Initialized roarm program\n")



    def serial_write(self, float_value):
    # Convert float to bytes
        float_bytes = struct.pack('f', float_value)

        # Format:
        # | 255 | 255 | no. of bytes | float_bytes | checksum |
        while self.ser_port.is_open:
            header = [255, 255]

            # Calculate payload size including payload bytes and payload size byte
            payload_size = len(float_bytes)

            # Calculate checksum
            chksum = sum(header) + payload_size + sum(float_bytes)

            # Send data
            self.ser_port.write(bytes(header))
            self.ser_port.write(bytes([payload_size]))
            self.ser_port.write(float_bytes)
            self.ser_port.write(bytes([chksum % 256]))  # Send the checksum
            
            print(chksum, "--------")


            

    def serial_read(self):
        """returns bool for valid read, also returns the data read"""
       
        if (self.ser_port.read() == b'\xff') and (self.ser_port.read() == b'\xff'):
            self.connected = True 
            chksum = 255 + 255
            self.plSz = self.ser_port.read()[0]            
            chksum += self.plSz
            self.payload = self.ser_port.read(self.plSz - 1)
            print(chksum)
           
            chksum += sum(self.payload)
            chksum = bytes([chksum % 256])
            _chksum = self.ser_port.read()

            return _chksum == chksum
        return False
    
    def disconnect(self):
        stdout.write("disconnected\n")
    
    
    def run_program(self):
        while self.ser_port.is_open:
            if self.serial_read():
                u = struct.unpack("f", self.payload[:4])    # encoder values
                v = struct.unpack("f",self.payload[4:8])    # rtc values time delta
                w = struct.unpack("f",self.payload[8:12])
                print(self.ser_port.read())

              

                nw = None

                if not nw:
                    nw = datetime.now()     # datetime

                if self.csv_enabled:
                
                  self.csv.writerow([ u,v,w])
                    
                if keyboard.is_pressed("e"):
                    self.csv_file.close()
                    break
            if keyboard.is_pressed("q"):
                print("closing")
                break
            if not self.ser_port.is_open:
                
                print("port closed")
                break
            
        print("program ended")
    
if __name__ == '__main__':
    # Input angles in degrees
    u = float(input("Enter angle u in degrees: "))
    v = float(input("Enter angle v in degrees: "))
    w = float(input("Enter angle w in degrees: "))
    #u = float(input("Enter angle u in degrees: "))
    #v = float(input("Enter angle v in degrees: "))
    #w = float(input("Enter angle w in degrees: "))


    myport = SerialPort("COM7", 115200)
    # while(True):
    # myport.run_program()
    myport.serial_write(u)
    
        # myport.run_program()

# Option 2: Directly passing u, v, and w
#myport.serial_write(u, v, w)

    

