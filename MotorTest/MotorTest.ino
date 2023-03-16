void setup(){
    pinMode(A0, OUTPUT);
    pinMode(A1, OUTPUT);

    pinMode(A2, OUTPUT);
    pinMode(A3, OUTPUT);
    
    pinMode(A4, OUTPUT);
    pinMode(A5, OUTPUT);

    pinMode(3, OUTPUT);
    pinMode(4, OUTPUT);

}

void loop(){

    // Front = side with h bridges.

    // Front right (Leftmost pins on h bridge)
    digitalWrite(A0, HIGH);
    digitalWrite(A1, LOW);
    delay(500);

    // Back right (Rightmost pins on h bridge)
    digitalWrite(A2, HIGH);
    digitalWrite(A3, LOW);
    delay(500);

    // Front Left (Second set of pins on h bridges)
    digitalWrite(A4, HIGH);
    digitalWrite(A5, LOW);
    delay(500);

    // Back Left (Third set of pins of h bridge)
    digitalWrite(3, HIGH);
    digitalWrite(4, LOW);
    delay(500);
    
    digitalWrite(A0, LOW);
    digitalWrite(A1, LOW);
    digitalWrite(A2, LOW);
    digitalWrite(A3, LOW);
    digitalWrite(A4, LOW);
    digitalWrite(A5, LOW);
    digitalWrite(3, LOW);
    digitalWrite(4, LOW);

    delay(2500);

    digitalWrite(A0, LOW);
    digitalWrite(A1, HIGH);
    delay(500);
    digitalWrite(A2, LOW);
    digitalWrite(A3, HIGH);
    delay(500);
    digitalWrite(A4, LOW);
    digitalWrite(A5, HIGH);
    delay(500);
    digitalWrite(3, LOW);
    digitalWrite(4, HIGH);
    delay(500);

    digitalWrite(A0, LOW);
    digitalWrite(A1, LOW);
    digitalWrite(A2, LOW);
    digitalWrite(A3, LOW);
    digitalWrite(A4, LOW);
    digitalWrite(A5, LOW);
    digitalWrite(3, LOW);
    digitalWrite(4, LOW);

    delay(2500);
}

