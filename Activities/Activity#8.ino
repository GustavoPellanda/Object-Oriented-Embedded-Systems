/* This program receives inputs from a keypad and prints them on the LCD Display.
Requirements: The numbers inputted must be stored in a buffer.
The buffer must be a rotational array.
*/

#include <Keypad.h>
#include <LiquidCrystal.h>

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

    void printDigit(char digit) {
        lcd.clear();
        lcd.print("Input: ");
        lcd.print(digit);
    }
};

class KeypadHandler {
private:
    static const byte ROWS = 4;
    static const byte COLS = 4;
    char keys[ROWS][COLS] = {
        {'1', '2', '3', 'A'},
        {'4', '5', '6', 'B'},
        {'7', '8', '9', 'C'},
        {'*', '0', '#', 'D'}
    };
    byte rowPins[ROWS];
    byte colPins[COLS];
    Keypad keypad;

public:
    KeypadHandler(byte row1, byte row2, byte row3, byte row4,
                  byte col1, byte col2, byte col3, byte col4)
        : rowPins{row1, row2, row3, row4}, colPins{col1, col2, col3, col4},
          keypad(Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS)) {}

    char getLastPressedKey() {
        return keypad.getKey();
    }
};

class Buffer {
private:
    char inputBuffer[16];
    int currentIndex = 0;

public:
    Buffer() {
        for (int i = 0; i < 16; i++) {
            inputBuffer[i] = ' ';
        }
    }

    void insertIntoBuffer(char toBuffer) {
        inputBuffer[currentIndex] = toBuffer;
        currentIndex = (currentIndex + 1) % 16;
    }

    char getLastKey() {
        int lastIndex = (currentIndex - 1 + 16) % 16;
        return inputBuffer[lastIndex];
    }

    void printBuffer() {
        Serial.print("Buffer Updated: [");
        for (int i = 0; i < 16; i++) {
            Serial.print(inputBuffer[i]);
        }
        Serial.println("]");
    }
};

Display display(13, 12, 11, 10, 9, 8);
KeypadHandler keypad(2, 3, 4, 5, 6, 7, A0, A1);
Buffer buffer;

void setup() {
    Serial.begin(9600);
}

void loop() {
    char key = keypad.getLastPressedKey();
    if (key) {
        buffer.insertIntoBuffer(key);
        display.printDigit(buffer.getLastKey());
    }
    buffer.printBuffer();
}
