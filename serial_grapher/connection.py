import serial
import serial.tools.list_ports

# Port identification functions
def identify_ports():
    ports = serial.tools.list_ports.comports()
    return ports

def find_arduino_port(connected_devices):
    port = 'null'
    num_of_connected_devices = len(connected_devices)

    for i in range (0, num_of_connected_devices):
        port = found_ports[i]
        str_port = str(port)

        if 'Arduino' in str_port:
            split = str_port.split(' ')
            COM_port = (split[0])

    return COM_port

found_ports = identify_ports()
connected_COM_port = find_arduino_port(found_ports)

ser = serial.Serial(connected_COM_port, baudrate=115200, timeout=1)
