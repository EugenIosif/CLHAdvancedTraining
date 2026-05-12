import serial
import time
import random
import hashlib
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

KEY = bytes([0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c])

def getPublicKey():
    while True:
        if(ser.in_waiting > 0):
            bs = ser.read(ser.in_waiting)
            print("Key data received:", bs.hex())  # Read and print the received data
            
            received_hash = bs[0:4]
            ciphertext = bs[4:20]
            computed_hash = compute_hash(ciphertext, 16)

            if computed_hash == int.from_bytes(received_hash, 'little'):
                #if the hashes match, do the rest
                print("Key HASH matches! Here is the payload bytes:") 
                print("Received Data (hex):", ciphertext.hex())       
                cipher = Cipher(algorithms.AES(KEY), modes.ECB(), backend=default_backend())
                decryptor = cipher.decryptor()            
                try:
                    decrypted_payload = decryptor.update(ciphertext) + decryptor.finalize()
                    print("AES Decryption Successful!")
                    print("Decrypted Data (hex):", decrypted_payload.hex())
                except Exception as e:
                    print("Decryption failed:", e)                    
            else:
                print("Key HASH does not match")
                print("the received hash: ", received_hash.hex())
                print("the computed hash: ", computed_hash.to_bytes(4, 'little').hex())
            
            return

        time.sleep(0.5)

def getFunctionPayload():
    while True:
        if(ser.in_waiting > 0):
            bs = ser.read(ser.in_waiting)
            print("Payload data received:", bs.hex())  # Read and print the received data
            
            received_hash = bs[0:32]
            ciphertext = bs[32:96]
            digest = hashlib.sha256()
            digest.update(ciphertext)

            if digest.digest() == received_hash:
                #if the hashes match, do the rest
                print("Payload HASH matches! Here is the payload bytes:") 
                print("Received Data (hex):", bs[32:240].hex())
            else:
                print("Payload HASH does not match")
                print("the received hash: ", received_hash.hex())
                print("the computed hash: ", digest.hexdigest())
        time.sleep(0.5)
    return

if __name__ == "__main__":
    getPublicKey()
    getFunctionPayload()
