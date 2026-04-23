import serial
import time
import random

ser = serial.Serial(
    port='COM9',
    baudrate=115200,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS
)
while True:
    if(ser.in_waiting > 0):
        bs = ser.read(ser.in_waiting)  # Clear the buffer
        print("Data received:", bs)  # Read and print the received data
        decrypted_data = bytes([b ^ 0xCD for b in bs])  # Decrypt using XOR with 0xCD
        print("Decrypted data:", decrypted_data)  # Print the decrypted data
    time.sleep(0.05)
