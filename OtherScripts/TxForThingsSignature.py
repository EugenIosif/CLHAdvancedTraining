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

def compute_signature(bytes_data, number_of_bytes):
    hash_value = 0x811c9dc5
    for i in range(number_of_bytes):
        hash_value ^= bytes_data[i]
        hash_value = (hash_value * 0x01000193) & 0xFFFFFFFF
    return hash_value

while True:
    if(ser.in_waiting > 0):
        bs = ser.read(ser.in_waiting)  # Clear the buffer
        print("Data received:", bs)  # Read and print the received data
        received_signature = bs[0:4]
        computed_signature = compute_signature(bs[4:20], 16)
        if computed_signature == int.from_bytes(received_signature, 'little'):
            print("Signature matches! Here is the payload bytes:")
            print(bs[4:20].hex())

        else:
            print("Signature does not match")

    time.sleep(0.05)
