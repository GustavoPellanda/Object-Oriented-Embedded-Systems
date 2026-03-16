// Define the interval period (in milliseconds)
const unsigned long interval = 1000; // 1 second

void setup() {
  DDRD = 0B11000000; // Pins 6 and 7 are exits
}

void loop() {
  PORTD = 0b10000000; // Port 6 pulldown and port 7 pullup
  _delay_ms(interval);
  PORTD = 0b01000000;
  _delay_ms(interval);
}
