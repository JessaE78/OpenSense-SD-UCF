import serial
import serial.tools.list_ports

# Port identification functions
def identify_ports():
    ports = serial.tools.list_ports.comports()
    return ports

def find_arduino_port(connected_devices):
    port = 'null'
    num_of_connected_devices = len(connected_devices)

    for i in range(0, num_of_connected_devices):
        port = found_ports[i]
        str_port = str(port)

        if 'Arduino' in str_port:
            split = str_port.split(' ')
            COM_port = (split[0])

    return COM_port

found_ports = identify_ports()
connected_COM_port = find_arduino_port(found_ports)

ser = serial.Serial(connected_COM_port, baudrate=115200, timeout=1)
num_of_points = 5
data_list = [(0.0, 0.0)] * num_of_points  # Tuple to store sensor data (float) and timestamp (string)
data_file = open('data.txt', 'w')
data_average = 0.0

# Parse values from Arduino serial
def get_values():
    arduinoData = ser.readline().decode('ascii').split('-')
    # print(arduinoData)
    return arduinoData

# Populate data list of serial values
def populate_list():
    for i in range(0, num_of_points):
        values = get_values()
        if len(values) >= 2:
            timestamp = values[0].strip()
            sensor_data = values[1].strip()
            data_list[i] = (timestamp, sensor_data)
        else:
            data_list[i] = (0.0, 0.0)
    return

# Write to file
def print_text():
    for i in range(0, num_of_points):
        timestamp, sensor_data = data_list[i]
        data_file.write(f'{timestamp}, {sensor_data}\n')  # Format the sensor data as float with two decimal places
    data_file.close()
    return

while 1:
    populate_list()
    print_text()
    break
