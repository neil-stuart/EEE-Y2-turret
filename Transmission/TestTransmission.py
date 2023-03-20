import time
import serial
import random

# This script simulates the data coming from an xbox controller using random numbers
# Was used for debugging the transceivers without needing a controller.

# Port where dongle is connected, 115200 baud rate.
ser = serial.Serial("COM11",baudrate=115200,timeout=0.1)

# Returns a random array 
# in the form [LeftJoystickX,RightJoystickX,RightjoystickY,LeftBumper,RightBumper]
def getRandomData():
    return ([round(random.random(),2),round(random.random(),2),round(random.random(),2),random.choice([0,1]),random.choice([0,1])])

# Send random data to the ner shooter every 0.5 seconds
while(True):

    data = getRandomData()
    string = "[%d,%d,%.2f,%.2f,%d,%d]"%(data[0],data[1],data[2],data[3],data[4],data[5])
    data_to_send = string.encode("utf-8")
    print("Transmitting: \n" +"\t\t" +string)

    ser.write(data_to_send)
    
    # Delay to wait for receive
    time.sleep(0.05)
    
    try:
        data = ser.read_all().decode("utf-8")
    except UnicodeDecodeError as e: 
        data = b'Error'
            
    print("Response:\n\t\t" + str(data))

    time.sleep(0.5)
