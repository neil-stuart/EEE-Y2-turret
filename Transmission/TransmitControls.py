from XboxController import XboxController
from serial import Serial
from threading import Thread
import re

# Main script for sending the active controller data to the arduino.
zigbee_dongle = Serial("COM11",baudrate=115200)
transmitting = False;
debug = True;

# Only called in debug mode.
def print_rx_data():
    while True:
        if (zigbee_dongle.in_waiting>0):
            data_in = []
            if(not transmitting):
                try:
                    data_in = zigbee_dongle.read_all().decode("utf-8")
                except UnicodeDecodeError as e:
                    data_in = "Problem decoding data."      
                print("data_rxed: " + str(data_in))
        
if __name__ == '__main__':
    controller = XboxController()

    # Start the thread that prints any data received if debugging, 
    # Using thread so it doesn't interfere with the sending of data.
    if(debug):
        rx_thread = Thread(target=print_rx_data)
        rx_thread.start()
        
    
    # Serial port for dongle, baudrate 115200.
    data = controller.read()
    
    # Send data loop.
    while(True):

        # If controller readout not equal to last data sent.
        if(controller.read()!=data):

            # The below flag blocks the receiving thread, 
            # if trying to receive at the same time as transmitting 
            # it throws an error, this prevents that from happening
            transmitting = True;

            # Read controller into current input.
            data = controller.read();

            # Convert to string of known length.
            string = "[%d,%d,%.2f,%.2f,%d,%d]"%(data[0],data[1],data[2],data[3],data[4],data[5])
            
            # Convert to bytes.
            data_out = string.encode("utf-8")

            # Send data as bytes.
            zigbee_dongle.write(data_out)

            # Allow receive.
            transmitting = False;
            
            print("data_txed: " + string[1:-1])