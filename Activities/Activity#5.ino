/* This code recreates the previous classe's functionality, 
but this time the use of the delay function is eliminated 
and a debounce code was added to the button's readings.
Those changes make the button function precisely. */

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
  unsigned long lastMillis;
  bool state1;
  bool state2;
  unsigned long interval;

public:
  LEDs(byte p1, byte p2) : pin1(p1), pin2(p2), lastMillis(0), state1(false), state2(false), interval(500){
    pinMode(pin1, OUTPUT);
    pinMode(pin2, OUTPUT);
  }

  ~LEDs(){}

  void on(){
    digitalWrite(pin1, HIGH);
    digitalWrite(pin2, HIGH);
    state1 = true;
    state2 = true;
  }

  void off(){
    digitalWrite(pin1, LOW);
    digitalWrite(pin2, LOW);
    state1 = false;
    state2 = false;
  }

  void blinkTogether(){
    unsigned long currentMillis = millis();
    if (currentMillis - lastMillis >= interval) {
      lastMillis = currentMillis;
      state1 = !state1;
      state2 = !state2;
      digitalWrite(pin1, state1 ? HIGH : LOW);
      digitalWrite(pin2, state2 ? HIGH : LOW);
    }
  }

  void blinkAlternately(){
    unsigned long currentMillis = millis();
    if (currentMillis - lastMillis >= interval) {
      lastMillis = currentMillis;
      state1 = !state1;
      state2 = !state2;
      digitalWrite(pin1, state1 ? HIGH : LOW);
      digitalWrite(pin2, state2 ? LOW : HIGH);
    }
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
unsigned long lastButtonPress = 0;
const unsigned long debounceDelay = 50;

void setup(){
  Serial.begin(9600);
  ledRow.on();
}

void loop() {
  unsigned long currentMillis = millis();

  if(button.isPressed()){
    if(currentMillis - lastButtonPress > debounceDelay){
      currentState = (currentState + 1) % 3;
      lastButtonPress = currentMillis;
    }
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