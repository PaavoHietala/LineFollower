#define trigPin 7           // Trigger pin for distance sensor
#define echoPin 6           // Feedback pin from distance sensor
#define IRledPinLeft 2      // Left IR LED pin
#define IRledPinRight 3     // Right IR LED pin
#define IRpdLeft A1         // Left IR photodiode signal
#define IRpdRight A0        // Right IR photodiode signal
#define BlackThreshold -20  // Threshold for photodiode, on line if under this
#define debug 0             // Enable debugging output in serial monitor

void setup(){
    Serial.begin (9600);
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    pinMode(IRledPinLeft, OUTPUT);
    pinMode(IRledPinRight, OUTPUT);
}

void loop(){
    float distance;
    char left_IR;
    char right_IR;
    
    distance = check_distance();
    left_IR = check_IR('L');
    right_IR = check_IR('R');

    Serial.print(left_IR);
    Serial.print('\t');
    Serial.print(right_IR);
    Serial.print('\t');
    Serial.println(distance);

    driver(left_IR, right_IR, distance);
    
    delay(500);
}

int check_distance(){
    float duration;
    float distance;
    
    digitalWrite(trigPin, LOW); 
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    
    duration = pulseIn(echoPin, HIGH);
    distance = ((float)duration / 2.0) / 27.0;
    
    return distance;
}

char check_IR(char side){
    int sigNoise, sig, noise, ledPin;
    uint8_t pdPin;

    if (side == 'R'){
        ledPin = IRledPinRight;
        pdPin = IRpdRight;
        if(debug==1){Serial.print("Right: ");}
    }
    else{
        ledPin = IRledPinLeft;
        pdPin = IRpdLeft;
        if(debug==1){Serial.print("Left: ");}
    }
    
    digitalWrite(ledPin, HIGH);
    delayMicroseconds(100);
    sigNoise = analogRead(pdPin); // Read noisy photodiode signal (led on)
    digitalWrite(ledPin, LOW);
    delayMicroseconds(100);
    noise = analogRead(pdPin); // Read only noise from photodiode (led off)
    sig = sigNoise - noise;
    
    if(debug==1){Serial.println(sig);}

    if (sig >= BlackThreshold){
        return 'W';
    }
    else{
        return 'B';
    }
}

void driver(char left_IR, char right_IR, float distance){
    if ((left_IR == 'B' && right_IR == 'B') || distance <= 5){
        Serial.println("stop");
    }
    else if (left_IR == 'B' && right_IR == 'W'){
        Serial.println("Turning left");
    }
    else if (left_IR == 'W' && right_IR == 'B'){
        Serial.println("Turning right");
    }
    else{
        Serial.println("Full steam ahead");
    }
}
