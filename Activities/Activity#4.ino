/* This program creates state changes using a push button 
Requirements: Insert serial prints to see in which the states the LEDs are in
*/

class Button {
private:
  byte pin;

public:
  Button(byte p) : pin(p){
    pinMode(pin, INPUT_PULLUP);
  }

  ~Button(){}

  void on(){
    digitalWrite(pin, HIGH);
  }

  void off(){
    digitalWrite(pin, LOW);
  }
  
  boolean isPressed(){
    return !digitalRead(pin);
  }    
};

class LEDs {
private:
    byte pin1;
    byte pin2;

public:
  LEDs(byte p1, byte p2) : pin1(p1), pin2(p2){
    pinMode(pin1, OUTPUT);
    pinMode(pin2, OUTPUT);
  }

  ~LEDs(){}

  void on(){
    digitalWrite(pin1, HIGH);
    digitalWrite(pin2, HIGH);
    delay(500);
  }

  void off(){
    digitalWrite(pin1, LOW);
    digitalWrite(pin2, LOW);
  }

  void blinkTogether(){
    digitalWrite(pin1, HIGH);
    digitalWrite(pin2, HIGH);
    delay(500);
    digitalWrite(pin1, LOW);
    digitalWrite(pin2, LOW);
    delay(500);
  }

  void blinkAlternately(){
    digitalWrite(pin1, HIGH);
    digitalWrite(pin2, LOW);
    delay(500);
    digitalWrite(pin1, LOW);
    digitalWrite(pin2, HIGH);
    delay(500);
  }
};

class StateHandler {
private:
    LEDs &ledRow;

public:
    StateHandler(LEDs& ledRow) : ledRow(ledRow){}

    void state01(){
        ledRow.on();
        Serial.println("State 1: LEDs ON");
    }

    void state02(){
        ledRow.blinkTogether();
        Serial.println("State 2: LEDs Blinking Together");
    }

    void state03(){
        ledRow.blinkAlternately();
        Serial.println("State 3: LEDs Blinking Alternately");
    }
};

LEDs ledRow(6, 7);
Button button(13);
StateHandler stateHandler(ledRow);

int currentState = 0;

void setup(){
  Serial.begin(9600);
  ledRow.on();
}

void loop() {
  if(button.isPressed()){
    currentState = (currentState + 1) % 3;
  }

  switch(currentState){
    case 0:
      stateHandler.state01();
      break;
    case 1:
      stateHandler.state02();
      break;
    case 2:
      stateHandler.state03();
      break;
  }
}