import serial
import time
import random
import hashlib
from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
from cryptography.hazmat.backends import default_backend


# d_int = 9331878932546167513
# e_int = 65537
# n_int = 18446743979220271189

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
                print("\nKey HASH matches! Here is the payload bytes:") 
                print("Received Data (hex):", ciphertext.hex())
                cipher = Cipher(algorithms.AES(KEY), modes.ECB(), backend=default_backend())
                decryptor = cipher.decryptor()
                try:
                    decrypted_payload = decryptor.update(ciphertext) + decryptor.finalize()
                    print("AES Decryption Successful!")
                    print("Decrypted Data (hex):", decrypted_payload.hex())

                    global RSA_e, RSA_n
                    #store in inverse order
                    RSA_e = decrypted_payload[0:8][::-1]
                    RSA_n = decrypted_payload[8:16][::-1]
                    
                except Exception as e:
                    print("Decryption failed:", e)                    
            else:
                print("\nKey HASH does not match")
                print("the received hash: ", received_hash.hex())
                print("the computed hash: ", computed_hash.to_bytes(4, 'little').hex())
            
            return

        time.sleep(0.8)

def getFunctionPayload():
    while True:
        if(ser.in_waiting > 0):
            bs = ser.read(ser.in_waiting)
            print("\nPayload data received:", bs.hex())  # Read and print the received data
            
            #we get the signature from the first 32 bytes
            received_signature = bs[0:32]

            #we compute the local HASH - SHA256 of the next 208 bytes
            function_bytes = bs[32:240]
            digest = hashlib.sha256()
            digest.update(function_bytes)
            #computed_hash has the HASH value that will be used to compare to the decrypted one
            computed_hash = digest.digest()

            # RSA decryption of computed_hash with RSA_e and RSA_n
            e_int = int.from_bytes(RSA_e, 'big')
            n_int = int.from_bytes(RSA_n, 'big')
            
            #when we decrypt the signature using the public key, we get the HASH
            decrypted_HASH = bytearray()
            for i in range(0, 32, 8):
                chunk = received_signature[i:i+8]
                chunk_int = int.from_bytes(chunk, 'little')
                decrypted_chunk = pow(chunk_int, e_int, n_int).to_bytes(8, 'little')
                decrypted_HASH.extend(decrypted_chunk)
            
            #if the decrypted HASH is the same as the computed HASH, we know that: 
            #1: the payload was transmitted without error
            #2: the party that sent us the payload has the correct private key
            if decrypted_HASH == computed_hash:
                print("\nPayload HASH matches! Here is the payload bytes:") 
                print("Received Data (hex):", bs[32:240].hex())
            else:
                print("\nPayload HASH does not match")
                print("the  computed HASH: ", computed_hash.hex())
                print("the decrypted HASH: ", decrypted_HASH.hex())
            return
        time.sleep(0.8)

if __name__ == "__main__":
    getPublicKey()
    getFunctionPayload()
