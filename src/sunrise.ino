#define CLOCK_MODE 1
#define ALARM_SET_MODE 2

uint currentTime = 0;

//Set up rotary encoder & button
void doEncoderA();
void doEncoderB();
int encoderA = A1;
int encoderB = A0;
int encoderBtn = D5;
volatile bool A_set = false;
volatile bool B_set = false;
volatile int encoderPos = 0;
int prevPos = 0;
int value = 0;

//Set up speaker
int volume = 30;

//set up the pins for communication with the shift register
int latchPin = D2;
int clockPin = D1;
int dataPin = D0;

int mode = CLOCK_MODE;

//Set up alarm
uint alarmTime = 0;
bool alarmSet = false;

void setup()
{

  Serial.begin(9600);
  delay(1000);
  Serial.println(encoderPos);

  Particle.function("utc-offset", setUTCOffset);

  //Shift setup
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);

  //Mode switch
  pinMode(encoderBtn, INPUT);
  attachInterrupt(encoderBtn, modeSwitch, RISING);

  //Rotary encoders
  pinMode(encoderA, INPUT_PULLUP);
  pinMode(encoderB, INPUT_PULLUP);
  attachInterrupt(encoderA, doEncoderA, CHANGE);
  attachInterrupt(encoderB, doEncoderB, CHANGE);
}

int setUTCOffset(String offset)
{
  int offsetInt = offset.toInt();
  Time.zone(offsetInt);
  return 1;
}

void loop()
{
  uint hourThousands = Time.hour() * 100;
  currentTime = hourThousands + Time.minute();

  switch (mode)
  {
  case CLOCK_MODE:
    displayTime();
    break;
  case ALARM_SET_MODE:
    setAlarm(alarmTime);
    break;
  }

  if (prevPos != encoderPos)
  {
    prevPos = encoderPos;
    Serial.println(encoderPos);
  }

  if (alarmTime == currentTime && alarmSet)
  {
    soundAlarm();
  }

  delay(50);
}

// Mode switch

void modeSwitch()
{
  if (mode == CLOCK_MODE)
  {
    if (!alarmSet)
    {
      alarmTime = currentTime;
    }
    mode = ALARM_SET_MODE;
  }
  else
  {
    alarmSet = TRUE;
    mode = CLOCK_MODE;
  }
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

// Setting up alarm

void setAlarm(uint num)
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

// Encoders --------------------

void doEncoderA()
{
  if (digitalRead(encoderA) != A_set)
  { // debounce once more
    A_set = !A_set;
    // adjust counter + if A leads B
    if (A_set && !B_set)
    {
      encoderPos += 1;
      alarmTime++;
    }
  }
}

// Interrupt on B changing state, same as A above
void doEncoderB()
{
  if (digitalRead(encoderB) != B_set)
  {
    B_set = !B_set;
    //  adjust counter - 1 if B leads A
    if (B_set && !A_set)
    {

      encoderPos -= 1;
      alarmTime--;
    }
  }
}

// Alarm
void soundAlarm()
{
}