import serial
import struct
import keyboard
from sys import stdout

class SerialPort(object):
    # Contains functions that enable communication between the docking station and the roarm watches

    def __init__(self, serialport, serialrate=115200):
        # Initialise serial payload
        self.count = 0
        self.plSz = 0
        self.payload = bytearray()

        self.serialport = serialport
        self.ser_port = serial.Serial(serialport, serialrate)
    



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
                x = struct.unpack("f", self.payload[:4])    
                y = struct.unpack("f",self.payload[4:8])    
                z = struct.unpack("f",self.payload[8:12])
                # print(self.ser_port.read())

            if keyboard.is_pressed("q"):
                print("closing")
                break
            if not self.ser_port.is_open:
                
                print("port closed")
                break
            
        print("program ended")
    
if __name__ == '__main__':
    # Input values
    x = float(input("Enter values of  x : "))
    y = float(input("Enter values of  y : "))
    z = float(input("Enter values of  z : "))



    myport = SerialPort("COM7", 115200)
    # while(True):
    myport.run_program()
    myport.serial_read()
    myport.serial_write(x)
    


    

