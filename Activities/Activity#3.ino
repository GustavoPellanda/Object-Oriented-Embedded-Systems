/* This program to controls four LEDs in two separate groups using C++ classes. 
Requirements: The loop function should be composed only by class methods. 
For now, each state of the LEDs will occur for a fixed set of time. */

class LEDs {
private:
    byte pin1;
    byte pin2;

public:
    LEDs(byte p1, byte p2) : pin1(p1), pin2(p2) {
        pinMode(pin1, OUTPUT);
        pinMode(pin2, OUTPUT);
    }

    ~LEDs() {}

    void on() {
        digitalWrite(pin1, HIGH);
        digitalWrite(pin2, HIGH);
    }

    void off() {
        digitalWrite(pin1, LOW);
        digitalWrite(pin2, LOW);
    }

    void blinkTogether() {
        for (int i = 0; i < 4; i++) {
            on();
            delay(1000);
            off();
            delay(1000);
        }
    }

    void blinkSeparately() {
        for (int i = 0; i < 4; i++) {
            digitalWrite(pin1, HIGH);
            digitalWrite(pin2, LOW);
            delay(1000);
            digitalWrite(pin1, LOW);
            digitalWrite(pin2, HIGH);
            delay(1000);
        }
    }
};

LEDs redRow(6, 7);
LEDs greenRow(12, 13);

void setup() {
}

void loop() {
    redRow.on();
    greenRow.on();
    delay(2000);
    redRow.blinkTogether();
    redRow.on();
    greenRow.blinkSeparately();
    greenRow.blinkTogether();
    greenRow.on();
    redRow.blinkSeparately();
    greenRow.off();
    delay(1000);
}
