
// Sketch for debugging zigbee transmission, 
// sends back the data getting received.
#include <SoftwareSerial.h>
SoftwareSerial xbee(2,3);

void setup(){
    Serial.begin(115200);
}

void loop(){
    if(Serial.available()>0){
        Serial.write(Serial.read());
    }
}