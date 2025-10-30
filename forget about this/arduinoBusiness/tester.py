#install pyserial through pip

import serial
import serial.tools.list_ports

ports = list(serial.tools.list_ports.comports())
for p in ports:
    print(p.device, p.description)