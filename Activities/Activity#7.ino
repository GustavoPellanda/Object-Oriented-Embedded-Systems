/* This program manages LED states via push buttons, 
whereas a LCD display presents a menu for state selection.
*/

#include <LiquidCrystal.h>

// Saves button settings and returns which buttons are being pressed:
class ButtonSelector {
private:
    byte pinB1;
    bool lastButtonState;

public:
    ButtonSelector(byte pb1) : pinB1(pb1), lastButtonState(HIGH) {
        pinMode(pinB1, INPUT_PULLUP);
    }

    bool isPressed() {
        bool currentState = digitalRead(pinB1);
        if (currentState == LOW && lastButtonState == HIGH) {
            delay(50);
            lastButtonState = currentState;
            return true;
        }
        lastButtonState = currentState;
        return false;
    }
};

// Saves the LCD display settings and prints a message on the display:
class Display {
private:
    LiquidCrystal lcd;
    const int columns = 16, rows = 2;

public:
    Display(byte p1, byte p2, byte p3, byte p4, byte p5, byte p6)
        : lcd(p1, p2, p3, p4, p5, p6) {
        lcd.begin(columns, rows);
        lcd.setCursor(0, 0);
    }

    void printMessage(const char* message) {
        lcd.clear();
        int length = strlen(message);
        if (length <= columns) {
            lcd.setCursor(0, 0);
            lcd.print(message);
        } else {
            char line1[columns + 1] = {0}; 
            char line2[columns + 1] = {0};
            strncpy(line1, message, columns);
            strncpy(line2, message + columns, columns);
            lcd.setCursor(0, 0);
            lcd.print(line1);
            lcd.setCursor(0, 1);
            lcd.print(line2);
        }
    }
};

// Creates the LEDs effects:
class LEDs {
private:
    byte pin1;
    byte pin2;

public:
    LEDs(byte p1, byte p2) : pin1(p1), pin2(p2) {
        pinMode(pin1, OUTPUT);
        pinMode(pin2, OUTPUT);
    }

    void on() {
        digitalWrite(pin1, HIGH);
        digitalWrite(pin2, HIGH);
        delay(500);
    }

    void off() {
        digitalWrite(pin1, LOW);
        digitalWrite(pin2, LOW);
    }

    void blinkTogether() {
        digitalWrite(pin1, HIGH);
        digitalWrite(pin2, HIGH);
        delay(500);
        digitalWrite(pin1, LOW);
        digitalWrite(pin2, LOW);
        delay(500);
    }

    void blinkAlternately() {
        digitalWrite(pin1, HIGH);
        digitalWrite(pin2, LOW);
        delay(500);
        digitalWrite(pin1, LOW);
        digitalWrite(pin2, HIGH);
        delay(500);
    }
};

// Manages the state selection of the LEDs:
class StateHandler {
private:
    LEDs &ledRow;
    Display &display;
    ButtonSelector &buttonSelector;

public:
    StateHandler(LEDs &ledRow, Display &display, ButtonSelector &buttonSelector)
        : ledRow(ledRow), display(display), buttonSelector(buttonSelector) {}

    void state01() {
        ledRow.on();
        Serial.println("State 1: LEDs ON");
        display.printMessage("LEDs ON");
    }

    void state02() {
        Serial.println("State 2: LEDs Blinking Together");
        display.printMessage("LEDs Blinking Together");
        while (true) {
            ledRow.blinkTogether();
            if (buttonSelector.isPressed()) {
                break;
            }
        }
    }

    void state03() {
        Serial.println("State 3: LEDs Blinking Alternately");
        display.printMessage("LEDs Blinking Alternately");
        while (true) {
            ledRow.blinkAlternately();
            if (buttonSelector.isPressed()) {
                break;
            }
        }
    }

    void setState(int state) {
        switch (state) {
            case 1:
                state01();
                break;
            case 2:
                state02();
                break;
            case 3:
                state03();
                break;
            default:
                Serial.println("Invalid state");
                break;
        }
    }
};

LEDs ledRow(2, 3);
ButtonSelector buttonSelector(5);
Display display(13, 12, 11, 10, 9, 8);
StateHandler stateHandler(ledRow, display, buttonSelector);
int selection = 1;

void setup() {
    Serial.begin(9600);
}

void loop() {
    if (buttonSelector.isPressed()) {
        selection++;
        if (selection > 3) selection = 1;
        stateHandler.setState(selection);
    }
    delay(100);
}
