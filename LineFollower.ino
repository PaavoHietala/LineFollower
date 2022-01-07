#define trigPin 7           // Trigger pin for distance sensor
#define echoPin 6           // Feedback pin from distance sensor
#define IRledPinLeft 2      // Left IR LED pin
#define IRledPinRight 3     // Right IR LED pin
#define IRpdLeft A1         // Left IR photodiode signal
#define IRpdRight A0        // Right IR photodiode signal
#define BlackThreshold -20  // Threshold for photodiode, on line if under this
#define debug 1             // Enable debugging output in serial monitor
#define motorD1 8           // Motor pin D1 (drive backwards)
#define motorD2 9           // Motor pin D2 (drive forwards)
#define ServoPin 4          // Servo pin for steering
#include <Servo.h>

Servo steering;             // Steering servo

void setup(){
    Serial.begin(9600);
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    pinMode(IRledPinLeft, OUTPUT);
    pinMode(IRledPinRight, OUTPUT);
    pinMode(motorD1, OUTPUT);
    pinMode(motorD2, OUTPUT);
    steering.attach(ServoPin);
    steering.write(0);
    Serial.println("Setup completed");
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
        motor_ctrl("stop");
    }
    else if (left_IR == 'B' && right_IR == 'W'){
        Serial.println("Turning left");
        steering.write(30);
        motor_ctrl("fwd");
    }
    else if (left_IR == 'W' && right_IR == 'B'){
        Serial.println("Turning right");
        steering.write(150);
        motor_ctrl("fwd");
    }
    else{
        Serial.println("Full steam ahead");
        steering.write(90);
        motor_ctrl("fwd");
    }
}

void motor_ctrl(char* task){
    if (task == "fwd"){
        Serial.println("Driving motor forward");
        analogWrite(motorD1, LOW);
        analogWrite(motorD2, 255);
    }
    else if (task == "stop"){
        Serial.println("Stopping motor");
        analogWrite(motorD1, LOW);
        analogWrite(motorD2, LOW);
    }
    else if (task == "rev"){
        Serial.println("Driving motor backwards");
        analogWrite(motorD1, 255);
        analogWrite(motorD2, LOW);
    }
}
