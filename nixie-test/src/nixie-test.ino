// Nixie display test for sunrise.
// Uses the same shift-register pins as src/sunrise.ino and none of the clock logic.
//
// Modes (switch with the "mode" cloud function or by sending 0-4 over USB serial):
//   0  ALL     - every tube shows the same digit, counting 0-9 once a second
//   1  EACH    - one tube at a time counts 0-9, the others are blanked
//   2  BITWALK - a single 1 walks through all 16 shift-register outputs (multimeter test)
//   3  STATIC  - shows the digits set with the "digits" cloud function, e.g. "1234"
//   4  PINS    - toggles data/clock/latch together at 1 Hz (multimeter test on D0-D2)

// Run setup()/loop() straight away, even when Wi-Fi or the cloud is unavailable
SYSTEM_THREAD(ENABLED);

int latchPin = D2;
int clockPin = D1;
int dataPin = D0;

#define BLANK 15 // BCD values above 9 turn all cathodes off on a 74141/K155ID1

int mode = 0;
int step = 0;
uint8_t staticDigits[4] = {1, 2, 3, 4};
unsigned long lastStep = 0;

void setup()
{
  Serial.begin(115200);
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  displayDigits(BLANK, BLANK, BLANK, BLANK); // clear whatever the shift registers powered up with

  Particle.function("mode", setMode);
  Particle.function("digits", setDigits);
  Particle.variable("mode", mode);
  Particle.variable("step", step);

  RGB.control(true);
  Serial.println("nixie-test started");
}

int setMode(String arg)
{
  int m = arg.toInt();
  if (m < 0 || m > 4)
  {
    return -1;
  }
  mode = m;
  step = 0;
  lastStep = 0;
  Serial.printlnf("mode -> %d", mode);
  return mode;
}

int setDigits(String arg)
{
  if (arg.length() != 4)
  {
    return -1;
  }
  for (int i = 0; i < 4; i++)
  {
    char ch = arg.charAt(i);
    staticDigits[i] = (ch >= '0' && ch <= '9') ? ch - '0' : BLANK;
  }
  mode = 3;
  Serial.printlnf("digits -> %s", arg.c_str());
  return 1;
}

// Same byte layout as sunrise.ino: the display reads AB:CD
void shiftBytes(uint8_t first, uint8_t second)
{
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, first);
  shiftOut(dataPin, clockPin, MSBFIRST, second);
  digitalWrite(latchPin, HIGH);
}

void displayDigits(uint8_t a, uint8_t b, uint8_t c, uint8_t d)
{
  shiftBytes(c + (d << 4), a + (b << 4));
}

void loop()
{
  if (Serial.available())
  {
    char ch = Serial.read();
    if (ch >= '0' && ch <= '4')
    {
      setMode(String(ch));
    }
  }

  if (lastStep != 0 && millis() - lastStep < 1000)
  {
    return;
  }
  lastStep = millis();
  RGB.color(step % 2 ? 0 : 255, 0, step % 2 ? 255 : 0); // heartbeat: red/blue alternate

  switch (mode)
  {
  case 0:
  {
    uint8_t n = step % 10;
    displayDigits(n, n, n, n);
    Serial.printlnf("ALL: showing %d%d:%d%d", n, n, n, n);
    break;
  }
  case 1:
  {
    int tube = (step / 10) % 4;
    uint8_t n = step % 10;
    uint8_t d[4] = {BLANK, BLANK, BLANK, BLANK};
    d[tube] = n;
    displayDigits(d[0], d[1], d[2], d[3]);
    Serial.printlnf("EACH: tube %c (position %d of AB:CD) showing %d", 'A' + tube, tube + 1, n);
    break;
  }
  case 2:
  {
    int bit = step % 16;
    uint16_t v = 1 << bit;
    shiftBytes(v >> 8, v & 0xFF);
    Serial.printlnf("BITWALK: only output %d high (chip %d, Q%d)", bit, bit < 8 ? 2 : 1, bit % 8);
    break;
  }
  case 3:
    displayDigits(staticDigits[0], staticDigits[1], staticDigits[2], staticDigits[3]);
    Serial.printlnf("STATIC: %d%d:%d%d", staticDigits[0], staticDigits[1], staticDigits[2], staticDigits[3]);
    break;
  case 4:
  {
    int level = step % 2 ? HIGH : LOW;
    digitalWrite(dataPin, level);
    digitalWrite(clockPin, level);
    digitalWrite(latchPin, level);
    Serial.printlnf("PINS: D0, D1, D2 all %s", level ? "HIGH (3.3V)" : "LOW (0V)");
    break;
  }
  }
  step++;
}
