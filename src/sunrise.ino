
//set up the pins for communication with the shift register
int latchPin = D4;
int clockPin = D5;
int dataPin = D3;

int hourTens = 0;
int hourUnits = 0;
int minsTens = 0;
int minsUnits = 0;

void setup()
{
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
}

void loop()
{
  displayTime(); // send the time to the shift register
  delay(1000);
}

void displayTime()
{
  uint16_t nixieTime = 0;

  int mins = Time.minute();
  int minsUnits = mins % 10;
  int minsTens = mins - minsUnits;

  int hours = Time.hour();
  int hourUnits = hours % 10;
  int hourTens = hours - hourUnits;

  nixieTime += minsUnits;      // No shifting needed for the minsUnits = 1001
  nixieTime += minsTens << 4;  // Shift minsTens 4 bits, and add it on: = 00001001
  nixieTime += hourUnits << 8; // Shift 2 places, or 8 bits = 100100001001
  nixieTime += hourTens << 12; // shift 3 places, or 12 bites = 0000100100001001

  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, nixieTime); //shiftOut(dataPin, clockPin, bitOrder, value)
  digitalWrite(latchPin, HIGH);
}
