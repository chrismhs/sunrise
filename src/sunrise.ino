/*  a sketch to drive two nixie tubes using two SN74141 BCD chips and a single SN74HC595N shift register
    developed from the tutorials on Adafruit.com and arduino.cc
    the sketch will cause two nixie tubes to count from 0 to 99 but you can change it to create any two-digit number and have the nixie tube display it
    Jeff Glans 2013
    Released into the public domain
    
*/
//set up the pins for communication with the shift register
int latchPin = D4;
int clockPin = D5;
int dataPin = D3;
int x; //create a counting variable

// create an array that translates decimal numbers into an appropriate byte for sending to the shift register
int charTable[] = {0, 128, 64, 192, 32, 160, 96, 224, 16, 144, 8, 136, 72, 200, 40, 168, 104, 232, 24, 152, 4, 132, 68, 196, 36, 164, 100, 228, 20, 148, 12, 140, 76, 204, 44, 172, 108, 236, 28, 156, 2, 130, 66, 194, 34, 162, 98, 226, 18, 146, 10, 138, 74, 202, 42, 170, 106, 234, 26, 154, 6, 134, 70, 198, 38, 166, 102, 230, 22, 150, 14, 142, 78, 206, 46, 174, 110, 238, 30, 158, 1, 129, 65, 193, 33, 161, 97, 225, 17, 145, 9, 137, 73, 201, 41, 169, 105, 233, 25, 153};
byte nixies = 255; //initiate the byte to be sent to the shift register and set it to blank the nixies

void setup()
{
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  Serial.begin(9600);
}

void loop()
{
  nixies = 255;          // create a blank byte
  updateShiftRegister(); // send the blank byte to the shift register
  delay(500);

  for (x = 0; x < 100; x++)
  {                        // count from 0 to 99
    nixies = charTable[x]; // translate into a byte to send to the shift register

    updateShiftRegister(); //send to the shift register
    delay(500);

    Serial.print("x = ");
    Serial.println(x);
    Serial.print("nixies = ");
    Serial.println(nixies);
  }
}
//the process of sending a byte to the shift register
void updateShiftRegister()
{
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, nixies);
  digitalWrite(latchPin, HIGH);
}
