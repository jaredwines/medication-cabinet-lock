#include <Keypad.h>
#include <IRremote.h>
#include <SPI.h>
#include <RFID.h>

const int LOCK_PIN = A3;    // connected to the base of the transistor

/* Define the DIO used for the SDA (SS) and RST (reset) pins. */
#define SDA_DIO 10
#define RESET_DIO A4
/* Create an instance of the RFID library */
RFID RC522(SDA_DIO, RESET_DIO);

char bluetoothData = 0;

int RECV_PIN = A5;
IRrecv irrecv(RECV_PIN);
decode_results results;

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {5, 4, 3, 2}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {9, 8, 7, 6}; //connect to the column pinouts of the keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

String password = "0000";
String input = "";
bool  resetPassword = false;
bool  passwordCheck = false;
char key = ' ';
String serialNumRFID = "";

void setup() {
  Serial.begin(9600);
  pinMode(LOCK_PIN, OUTPUT);  // set  the transistor pin as output
  irrecv.enableIRIn(); // Start the receiver
  SPI.begin(); //Enable the SPI interface
  RC522.init(); //Initialise the RFID reader
}

void loop()
{
  //Key Pad Code
  receiveKeyPadInput();

  //RFID Reciever Code
  receiveRFIDInput();

  //IR Reciever Code
  receiveIRInput();

  //Bluetooth Reiever Code
  receiveBluetoothInput();

  if (!resetPassword && isDigit(key))
  {
    input = input + key;
    Serial.print("\n" + input);
    if (vaildPassword(input))
    {
      Serial.print("\nValid Password. Safe is now unlocked.");
      unlock();
      input = "";
    }
    else if (vaildInputSize(input))
    {
      Serial.print("\nInvalid Password!");
      input = "";
    }
  }

  else if (!resetPassword && key == '*')
  {
    resetPassword = true;
    input = "";
    Serial.print("\nPASSWORD RESET");
    Serial.print("\nEnter current password.");
  }

  if (resetPassword && !passwordCheck && isDigit(key))
  {
    input = input + key;
    Serial.print("\n" + input);
    if (vaildPassword(input))
    {
      Serial.print("\nValid Password. Please enter new password.");
      passwordCheck = true;
      input = "";
    }
    else if (vaildInputSize(input))
    {
      Serial.print("\nInvalid Password! Password reset failed!");
      resetPassword = false;
      input = "";
    }
  }

  else if (resetPassword && passwordCheck && isDigit(key))
  {
    input = input + key;
    Serial.print("\n" + input);
    if (vaildInputSize(input))
    {
      password = input;
      Serial.print("\nNew Password: " + input);
      resetPassword = false;
      passwordCheck = false;
      input = "";
    }
  }
  key = ' ';  //clear key for next input
}

bool vaildPassword(String inputStr)
{
  if (inputStr == password)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool vaildInputSize(String inputStr)
{
  if (inputStr.length() == 4)
  {
    return true;
  }
  else
  {
    return false;
  }
}
void receiveIRInput ()
{
  if (irrecv.decode(&results)) {
    if (results.value == 0x20DF08F7) {
      key = '0';
    }
    else if (results.value == 0x20DF8877) {
      key = '1';
    }
    else if (results.value == 0x20DF48B7) {
      key = '2';
    }
    else if (results.value == 0x20DFC837) {
      key = '3';
    }
    else if (results.value == 0x20DF28D7) {
      key = '4';
    }
    else if (results.value == 0x20DFA857) {
      key = '5';
    }
    else if (results.value == 0x20DF6897) {
      key = '6';
    }
    else if (results.value == 0x20DFE817) {
      key = '7';
    }
    else if (results.value == 0x20DF18E7) {
      key = '8';
    }
    else if (results.value == 0x20DF9867) {
      key = '9';
    }
    else if (results.value == 0x20DF22DD) {
      key = '*';
    }
    delay(100);
    irrecv.resume(); // Receive the next value
  }
}

void receiveKeyPadInput()
{
  key = keypad.getKey();
}

void receiveBluetoothInput()
{
  if (Serial.available() > 0)     // Send data only when you receive data:
  {
    bluetoothData = Serial.read();        //Read the incoming data & store into data
    if (bluetoothData == '0')
      key = '0';
    else if (bluetoothData == '1')
      key = '1';
    else if (bluetoothData == '2')
      key = '2';
    else if (bluetoothData == '3')
      key = '3';
    else if (bluetoothData == '4')
      key = '4';
    else if (bluetoothData == '5')
      key = '5';
    else if (bluetoothData == '6')
      key = '6';
    else if (bluetoothData == '7')
      key = '7';
    else if (bluetoothData == '8')
      key = '8';
    else if (bluetoothData == '9')
      key = '9';
    else if (bluetoothData == '+')
      key = '*';
  }
}

void receiveRFIDInput()
{
  /* Has a card been detected? */
  if (RC522.isCard())
  {
    /* If so then get its serial number */
    RC522.readCardSerial();
    for (int i = 0; i < 5; i++)
    {
      serialNumRFID += RC522.serNum[i], DEC;
    }
    if (serialNumRFID == "8010115163138")
    {
      Serial.print("\nValid Card. Safe is now unlocked.");
      unlock();
    }
    serialNumRFID = ""; //Clear serialNumRFID for the next RFID card.
  }
}

void unlock()
{
  digitalWrite(LOCK_PIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(3000);                       // wait for a second
  digitalWrite(LOCK_PIN, LOW);    // turn the LED off by making the voltage LOW
}

