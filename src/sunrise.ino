
//set up the pins for communication with the shift register
int latchPin = D5;
int clockPin = D4;
int dataPin = D3;

void setup()
{
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);

  Particle.function("utc-offset", setUTCOffset);
}

int setUTCOffset(String offset)
{
  int offsetInt = offset.toInt();
  Time.zone(offsetInt);
  return 1;
}

void loop()
{
  displayTime();
  delay(1000);
}

void displayTime()
{
  int hours = Time.hour();
  int hourUnits = hours % 10;
  int hourTens = (hours / 10) % 10;
  int mins = Time.minute();
  int minsUnits = mins % 10;
  int minsTens = (mins / 10) % 10;

  displayDigits(hourTens, hourUnits, minsTens, minsUnits);
}

// Anything in the 10,000s or higher will be ignored
void displayNumber(uint num)
{
  uint8_t a = (num % 10000) / 1000;
  uint8_t b = (num % 1000) / 100;
  uint8_t c = (num % 100) / 10;
  uint8_t d = (num % 10);

  displayDigits(a, b, c, d);
}

// The digits on sunrise read AB:CD
void displayDigits(uint8_t a, uint8_t b, uint8_t c, uint8_t d)
{
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, c + (d << 4));
  shiftOut(dataPin, clockPin, MSBFIRST, a + (b << 4));
  digitalWrite(latchPin, HIGH);
}
