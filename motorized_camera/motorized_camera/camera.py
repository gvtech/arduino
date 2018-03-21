import serial
from serial.tools import list_ports
from time import sleep

class CameraControl(object):
    def __init__(self,pitch,yaw,increment):
        self._pitch=pitch
        self._yaw=yaw
        self._positions=dict()
        self._increment=increment
        self.usbport=None
        self._usbserial=None
        self._speed=5

    def get_camera_port(self):
        if self.usbport is None:
            try:
                self.usbport=list_ports.grep("wch").next()[0]
            except Exception,e:
                print("Camera not found")
                raise e
        return self.usbport

    def get_camera_connection(self):
        if self._usbserial is None:
            try:
                self._usbserial=serial.Serial(self.get_camera_port(), 9600)
                self._usbserial.write("S%d"%self._speed)
            except Exception,e:
                print("Camera not connected")
                self.usbport=None
                raise e
        return self._usbserial

    def sendcmd(self,cmd,param):
        cmdline="%s%d"%(cmd,param)
        print("sending: "+cmdline)
        self.get_camera_connection().write(cmdline)
        self.get_camera_connection().flush()
        print("done")

    def move(self,movedir):
        if movedir=="top":
            self._pitch=min(self._pitch-self._increment,150)
            self.sendcmd("P",self._pitch)
        elif movedir=="down":
            self._pitch=max(self._pitch+self._increment,100)
            self.sendcmd("P",self._pitch)
        elif movedir=="right":
            self._yaw=min(self._yaw+self._increment,180)
            self.sendcmd("Y",self._yaw)
        elif movedir=="left":
            self._yaw=max(self._yaw-self._increment,30)
            self.sendcmd("Y",self._yaw)
        elif movedir.startswith("go"):
            pos=movedir[-1]
            if pos in self._positions:
                print("restoring:"+pos)
                self._yaw,self._pitch=self._positions[pos]
                self.sendcmd("Y",self._yaw)
                sleep(1)
                self.sendcmd("P",self._pitch)
        elif movedir.startswith("set"):
            pos=movedir[-1]
            print("storing:"+pos)
            self._positions[pos]=self._yaw,self._pitch
