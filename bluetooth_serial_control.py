import serial
import time

# CHANGE THIS to your Bluetooth COM port
COM_PORT = "COM7"

esp = serial.Serial(COM_PORT, 9600)

time.sleep(2)

# TURN ON
esp.write(b"ON\n")

print("Relay ON via Bluetooth")

esp.close()
