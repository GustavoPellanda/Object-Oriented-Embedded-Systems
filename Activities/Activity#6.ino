/* This code displays an int on a LCD Display,
being that the int can be increased or decreased by push buttons */

#include <LiquidCrystal.h>

class Display {
private:
    LiquidCrystal lcd;
    const int columns = 16, rows = 2;

public:
    Display(byte p1, byte p2, byte p3, byte p4, byte p5, byte p6):
        lcd(p1, p2, p3, p4, p5, p6) {
        lcd.begin(columns, rows);
        lcd.setCursor(0, 0);
    }

    void printNumber(int number){
        lcd.setCursor(0, 1);
        lcd.print("                ");
        lcd.setCursor(0, 1);
        lcd.print(number);
    }
};

class ButtonCounter {
private:
    int counter;
    byte pinB1, pinB2;
    bool lastStateB1, lastStateB2, currentStateB1, currentStateB2;

public:
    ButtonCounter(byte pb1, byte pb2): pinB1(pb1), pinB2(pb2), counter(0){
        pinMode(pinB1, INPUT_PULLUP);
        pinMode(pinB2, INPUT_PULLUP);
        
        lastStateB1 = digitalRead(pinB1);
        lastStateB2 = digitalRead(pinB2);
        currentStateB1 = lastStateB1;
        currentStateB2 = lastStateB2;
    }

    void update(){
        currentStateB1 = digitalRead(pinB1);
        currentStateB2 = digitalRead(pinB2);

        if(lastStateB1 == HIGH && currentStateB1 == LOW) counter++;
        if(lastStateB2 == HIGH && currentStateB2 == LOW) counter--;

        lastStateB1 = currentStateB1;
        lastStateB2 = currentStateB2;
    }

    int getCounter(){
        return counter;
    }
};

ButtonCounter buttonCounter(6, 7);
Display display(13, 12, 11, 10, 9, 8);

void setup(){

}

void loop(){
    buttonCounter.update();
    display.printNumber(buttonCounter.getCounter());
    delay(100);
}