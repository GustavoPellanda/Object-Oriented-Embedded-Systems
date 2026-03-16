/* Controls the LEDs as separate devices, eliminating the use of the delay function.*/

const byte pin6 = 6; // LED A
const byte pin7 = 7; // LED B

unsigned long timeCounterA;
unsigned long timeCounterB;

const unsigned long intervalA = 3000;
const unsigned long intervalB = 1000;

void setup() {
    timeCounterA = millis();
    timeCounterB = millis();

    pinMode(pin6, OUTPUT);
    pinMode(pin7, OUTPUT);

    digitalWrite(pin6, HIGH);
    digitalWrite(pin7, LOW);
}

void loop(){
    unsigned long instantTime = millis();

    if((instantTime - timeCounterA) >= intervalA){
        digitalWrite(pin6, !digitalRead(pin6));
        timeCounterA = instantTime; // This update is necessary to prevent millis overflow
    }

    if((instantTime - timeCounterB) >= intervalB){
        digitalWrite(pin7, !digitalRead(pin7));
        timeCounterB = instantTime;
    }
}

/*
-> instantTime is the current time in milliseconds since the program started.
-> timeCounter A and B are used to keep track of the last time the LEDs were toggled.
-> Every if statement checks if the time elapsed since the last toggle is greater than the expected interval to change the state of an LED.
-> When the if statement is true, the timeCounter will be updated with the last measurement of instantTime
*/