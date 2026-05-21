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
        bs = ser.read(ser.in_waiting)  # Receive hex characters
        bs = bs.replace(b'\r', b'').replace(b'\n', b'')
        print("\nData received:", bs)  # Read and print the received data
        signature = bs[0]  # The first byte is the signature
        value_bytes = bs[1:]  # The remaining bytes are the value
        print("\nSignature byte:", signature)  # Print the signature byte
        print("\nValue bytes:", value_bytes)  # Print the value bytes

        # Calculate the signature by XORing all the bytes
        calculated_signature = 0
        decrypted_data = bytes([b ^ 0xAA for b in bs])  # Decrypt using XOR with 0xAA
        for b in bs:
            calculated_signature ^= b
        print("\nCalculated signature:", calculated_signature)  # Print the calculated signature
        print("\nDecrypted data:", decrypted_data)  # Print the decrypted data

        if calculated_signature == signature:
            print("Signature is valid.")
        else:
            print("Signature is invalid.")
    time.sleep(0.05)
