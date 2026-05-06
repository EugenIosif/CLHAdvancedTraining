import serial
import time
import random
from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
from cryptography.hazmat.backends import default_backend

ser = serial.Serial(
    port='COM9',
    baudrate=115200,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS
)

def compute_hash(data_bytes, number_of_bytes):
    hash_value = 0x811c9dc5
    for i in range(number_of_bytes):
        hash_value ^= data_bytes[i]
        hash_value = (hash_value * 0x01000193) & 0xFFFFFFFF
    return hash_value

# def decryptRSA(data_bytes, number_of_bytes)

KEY = b'2b7e151628aed2a6abf7158809cf4f3c' # Must match the STM32 key

while True:
    if(ser.in_waiting > 0):
        bs = ser.read(ser.in_waiting)
        print("Data received:", bs.hex())  # Read and print the received data
        
        iv = bs[0:16]
        ciphertext = bs[16:]
        
        cipher = Cipher(algorithms.AES(KEY), modes.CBC(iv), backend=default_backend())
        decryptor = cipher.decryptor()
        
        try:
            decrypted_payload = decryptor.update(ciphertext) + decryptor.finalize()
            print("AES Decryption Successful!")
            print("Decrypted Data (hex):", decrypted_payload.hex())
            print("Decrypted Data (utf-8):", decrypted_payload.decode('utf-8', errors='ignore'))
        except Exception as e:
            print("Decryption failed:", e)

    time.sleep(0.05)

# while True:
#     if(ser.in_waiting > 0):
#         bs = ser.read(ser.in_waiting)  # Clear the buffer
#         print("Data received:", bs)  # Read and print the received data
#         received_signature = bs[0:4]
#         received_signature = bytes([b ^ 0xAB for b in received_signature])  # Decrypt using XOR with 0xAB

#         computed_signature = compute_hash(bs[4:20], 16)
        
#         if computed_signature == int.from_bytes(received_signature, 'little'):
#             print("Signature matches! Here is the payload bytes:")
#             print(bs[4:20].hex())
#         else:
#             print("Signature does not match")

#     time.sleep(0.05)
