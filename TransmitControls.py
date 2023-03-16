from XboxController import XboxController
from serial import Serial
from threading import Thread
import re

# Main script for sending the active controller data to the arduino.
zigbee_dongle = Serial("COM11",baudrate=115200)
transmitting = False;
debug = False;

# Only called in debug mode.
def print_rx_data():
    # Regular expression for parsing data in.
    data_pattern = re.compile(r'\[([^]]*)\]') 

    while True:
        if (zigbee_dongle.in_waiting >= 14):
            data_in = []
            if(not transmitting):
                try:
                    data_in = zigbee_dongle.read_all().decode("utf-8")
                    data_in = data_pattern.findall(data_in)
                except UnicodeDecodeError as e:
                    data_in = ["Problem decoding data."]      

            for data in data_in:
                print("data_received: " + (data if len(data_in)>0 else "No data"))
        

if __name__ == '__main__':
    joy = XboxController()

    # Start the thread that prints any data received if debugging, 
    # Using thread so it doesn't interfere with the sending of data.
    if(debug):
        rx_thread = Thread(target=print_rx_data)
        rx_thread.start()
        
    
    # Serial port for dongle, baudrate 115200.
    last_sent = joy.read()
    
    # Send data loop.
    while(True):

        # If controller readout not equal to last data sent.
        if(joy.read()!=last_sent):

            # The below flag blocks the receiving thread, 
            # if trying to receive at the same time as transmitting 
            # it throws an error, this prevents that from happening
            transmitting = True;

            # Read controller into current input.
            last_sent = joy.read();

            # Convert to string of known length.
            string = "[%.2f,%.2f,%.2f,%d,%d]"%(last_sent[0],last_sent[1],last_sent[2],last_sent[3],last_sent[4])
            
            # Convert to bytes.
            data_out = string.encode("utf-8")

            # Send data as bytes.
            zigbee_dongle.write(data_out)

            # Allow receive.
            transmitting = False;
            
            print("data_transmitted: " + string[1:-1])




