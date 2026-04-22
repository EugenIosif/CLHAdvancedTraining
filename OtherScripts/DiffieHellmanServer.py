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

EXCHANGEINITIALIZATION = 0x01
SECRETTRANSMISSION = 0x03
SECRETRECEIVED = 0x02
ENCRYPEDMESSAGETRANSMISSION = 0x04

e = 2
n = 19
mypublickey = 8
#15
sharedsecret = 0

state = "IDLE"

last_printed_state = None
while True:
    if state == "IDLE":
        if last_printed_state != state:
            print("\nState: IDLE")
            last_printed_state = state
        
        if(ser.in_waiting > 0):
            bs = ser.read(ser.in_waiting)  # Clear the buffer
            print("Data received:", bs)  # Read and print the received data

            command = bs[0]
            val1 = bs[1:5]  # Drop the value of the 4 bytes, we don't need it at this point

            if command == EXCHANGEINITIALIZATION:  # Example command to start negotiation
                state = "NEGOTIATING"
            else:
                state = "CLOSED"
    
    elif state == "NEGOTIATING":
        if last_printed_state != state:
            print("\nState: NEGOTIATING")
            last_printed_state = state

        transmitvalue = bytes([SECRETTRANSMISSION]) + pow(e, mypublickey, n).to_bytes(4, byteorder='big')  # Calculate the value to send
        print(f"Transmitting value: {int.from_bytes(transmitvalue[1:5], byteorder='big')}")  # Print the value being transmitted for debugging
        
        time.sleep(0.5)  # Wait for the microcontroller to be ready. 1 seconds
        
        ser.write(transmitvalue)  # send the calculated secret on UART
        
        state = "WAITINGFORSECRET"
    
    elif state == "WAITINGFORSECRET":
        if last_printed_state != state:
            print("\nState: WAITINGFORSECRET")
            
            last_printed_state = state        
        if(ser.in_waiting > 0):
            bs = ser.read(ser.in_waiting)  # Clear the buffer
            print("Data received:", bs)  # Read and print the received data
            
            command = bs[0]
            val1 = bs[1:5]
            
            if command == SECRETRECEIVED:
                sharedsecret = int.from_bytes(val1, byteorder='big')  # Convert the received bytes to an integer
                print(f"Shared secret received: {sharedsecret}")  # Print the shared secret for debugging

                sharedsecret = pow(sharedsecret, mypublickey, n)  # Calculate the shared secret
                print(f"Calculated shared secret: {sharedsecret}")  # Print the calculated shared secret for debugging
            
            state = "SENDENCRYPEDMESSAGE"
    
    elif state == "SENDENCRYPEDMESSAGE":
        if last_printed_state != state:
            print("\nState: SENDENCRYPEDMESSAGE")
            last_printed_state = state
        ledstate = random.randint(1, 7)  # Simulate LED state (1 to 7)
        print(f"LED State: {ledstate}")  # Print the LED state for debugging
        
        ledstate = ledstate ^ sharedsecret  # Simple XOR encryption for demonstration
        print(f"Encrypted LED State: {ledstate}")  # Print the encrypted LED state for debugging
        time.sleep(0.1)  # Wait for the microcontroller to process the key. 0.1 seconds
        
        ser.write(bytes([ENCRYPEDMESSAGETRANSMISSION]) + ledstate.to_bytes(4, byteorder='big'))  # Send the encrypted LED state
        
        state = "CLOSED"

    elif state == "CLOSED":
        if last_printed_state != state:
            print("\nState: CLOSED")
            last_printed_state = state
        
        state = "IDLE"

    time.sleep(0.05)
    