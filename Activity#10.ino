/*
  This code now implements an actuator routine to the previous activity.

  I chose to add a control to a DC motor, that spins faster as the temperature increases, and slower as the temperature decreases.
  The motor is controlled by a PWM signal, and the speed is determined by the temperature reading from the DHT sensor. 

  Author: Gustavo Pellanda / 2026
*/

#include <dht.h>
#include <LiquidCrystal.h>

/*
The DHTRead class returns the temperature and humidity separately.
It receives a pin to be read and a reference to a dht object.
The shouldReadSensor() function ensures calling the getter methods will not trigger 
the readSensor() function faster than the interval defined by readInterval.
*/
class DHTRead {
private:
  const byte pin;
  dht &sensor;
  int temperature;
  int humidity;
  unsigned long lastReadTime;
  const unsigned long readInterval = 2000; // Reads every two seconds

  void readSensor() {
    sensor.read11(pin);
    temperature = sensor.temperature;
    humidity = sensor.humidity;
    lastReadTime = millis();
  }

  bool shouldReadSensor() {
    return (millis() - lastReadTime) >= readInterval;
  }

public:
  DHTRead(byte p, dht& dhtRef) 
  : pin(p), sensor(dhtRef), temperature(0), humidity(0), lastReadTime(0) {}
  
  int getTemperature() {
    if (shouldReadSensor()) {
      readSensor();
    }
    return temperature;
  }

  int getHumidity() {
    if (shouldReadSensor()) {
      readSensor();
    }
    return humidity;
  }

  void update() {
    if (shouldReadSensor()) {
      readSensor();
    }
  }
};

/*
Handles the LCD display operations:
*/
class LCDDisplay {
private:
  LiquidCrystal lcd;
  const int columns = 16;
  const int rows = 2;
  
public:
  LCDDisplay(byte rs, byte en, byte d4, byte d5, byte d6, byte d7) 
    : lcd(rs, en, d4, d5, d6, d7) {
    lcd.begin(columns, rows);
  }

  void displayAll(int temp, int humidity, const char* state) {
    lcd.clear();
    
    // First line: Temperature and State
    lcd.setCursor(0, 0);
    lcd.print("Tmp:");
    lcd.print(temp);
    lcd.print((char)223);
    lcd.print("C ");
    lcd.print(state);
    
    // Second line: Humidity
    lcd.setCursor(0, 1);
    lcd.print("Humidity: ");
    lcd.print(humidity);
    lcd.print("%");
  }
};

/*
Handles the serial prints:
*/
class SerialMonitorHandler {
public:
  SerialMonitorHandler() {}

  void printSensorValues(int temp, int humidity) {
    Serial.print("Temperature: ");
    Serial.print(temp);
    Serial.println(" °C");
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");
    Serial.println("-------------------");
  }

  void printState(byte state) {
    Serial.print("State: ");
    if(state == 1) Serial.println("green");
    if(state == 2) Serial.println("yellow");
    if(state == 3) Serial.println("red");
  }
};

/*
Handles which light should be on in the stacklight:
*/
class StackLight {
private:
  byte Rpin, Ypin, Gpin;
  unsigned long previousBlinkTime = 0;
  const unsigned long blinkInterval = 500; // 500ms blink interval

public:
  StackLight(byte r, byte y, byte g) : Rpin(r), Ypin(y), Gpin(g) {
    pinMode(Rpin, OUTPUT);
    pinMode(Ypin, OUTPUT);
    pinMode(Gpin, OUTPUT);

    digitalWrite(Rpin, LOW);
    digitalWrite(Ypin, LOW);
    digitalWrite(Gpin, LOW);
  }

  void setRedLightOn() {
    digitalWrite(Ypin, LOW);
    digitalWrite(Gpin, LOW);
    
    // Handle blinking
    unsigned long currentTime = millis();
    if (currentTime - previousBlinkTime >= blinkInterval) {
      previousBlinkTime = currentTime;
      digitalWrite(Rpin, !digitalRead(Rpin)); // Toggle red LED
    }
  }

  void setYellowLightOn() {
    digitalWrite(Rpin, LOW);
    digitalWrite(Ypin, HIGH);
    digitalWrite(Gpin, LOW);
  }

  void setGreenLightOn() {
    digitalWrite(Rpin, LOW);
    digitalWrite(Ypin, LOW);
    digitalWrite(Gpin, HIGH);
  }
};

/*
Increases or decreases the speed of a DC motor:
*/
class DCMotorController {
private:
  byte pwmPin;

public:
  DCMotorController(byte pwm) : pwmPin(pwm) {   
    pinMode(pwmPin, OUTPUT);
  }

  void setSpeedByState(byte state) {
    if(state == 1) analogWrite(pwmPin, 80);
    if(state == 2) analogWrite(pwmPin, 160);
    if(state == 3) analogWrite(pwmPin, 255);
  }
};

/*
Handles the states of the system's operations 
based on the temperature and humidity:
*/
class StateHandler {
private:
  SerialMonitorHandler &serialMonitor;
  StackLight &stackLight;
  LCDDisplay &lcdDisplay;
  DCMotorController &dcMotor;

public:
  StateHandler(SerialMonitorHandler &serialRef, StackLight &stackLightRef, LCDDisplay &lcdRef, DCMotorController &dcMotorRef) 
    : serialMonitor(serialRef), stackLight(stackLightRef), lcdDisplay(lcdRef), dcMotor(dcMotorRef) {}

  void analyseState(int temp, int humidity) {
    if(temp >= 30) {
      redAlert(temp, humidity);
      return;
    }
    if(temp >= 25 && humidity < 20) {
      yellowOperation(temp, humidity);
      return;
    }
    if(temp < 25 || humidity >= 20) { 
      greenOperation(temp, humidity);
      return;
    }
  }

  void greenOperation(int temp, int humidity) {
    dcMotor.setSpeedByState(1);
    stackLight.setGreenLightOn();
    serialMonitor.printState(1);
    lcdDisplay.displayAll(temp, humidity, "Normal");
  }

  void yellowOperation(int temp, int humidity) {
    dcMotor.setSpeedByState(2);
    stackLight.setYellowLightOn();
    serialMonitor.printState(2);
    lcdDisplay.displayAll(temp, humidity, "Warning");
  }

  void redAlert(int temp, int humidity) {
    dcMotor.setSpeedByState(3);
    stackLight.setRedLightOn();
    serialMonitor.printState(3);
    lcdDisplay.displayAll(temp, humidity, "ALERT!");
  }
};

/*
dht DHT creates an object of the DHT class from the dht.h library.
The read11() function from the library populates its attributes with float values
of temperature in °C and humidity in relative percentage.
*/
dht DHT;
DHTRead dhtRead(8, DHT);
SerialMonitorHandler serialMonitor;
StackLight stackLight(12, 11, 10);
LCDDisplay lcdDisplay(7, 6, 5, 4, 3, 2);
StateHandler stateHandler(serialMonitor, stackLight, lcdDisplay);

void setup() {
  Serial.begin(9600);
}

void loop() {
  unsigned long currentMillis = millis();
  static unsigned long lastSensorRead = 0;
  if(currentMillis - lastSensorRead >= 2000) {
    lastSensorRead = currentMillis;
    dhtRead.update();
    int temperature = dhtRead.getTemperature();
    int humidity = dhtRead.getHumidity();
    stateHandler.analyseState(temperature, humidity);
    serialMonitor.printSensorValues(temperature, humidity);
  }
}