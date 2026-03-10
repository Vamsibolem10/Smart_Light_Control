import serial
import time

# CHANGE THIS to your ESP32 Bluetooth COM port
COM_PORT = "COM1"

# Open Bluetooth Serial connection
esp = serial.Serial(COM_PORT, 9600)

time.sleep(2)

# Send command
esp.write(b"ON\n")

print("Relay turned ON via Bluetooth")

esp.close()
