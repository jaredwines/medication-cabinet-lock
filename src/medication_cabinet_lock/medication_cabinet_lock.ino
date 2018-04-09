#include <Keypad.h>
#include <IRremote.h>
#include <SPI.h>
#include <RFID.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//Setting to setup the LCD Screen
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

const int LOCK_PIN = A0;    // connected to the base of the transistor

/* Define the DIO used for the SDA (SS) and RST (reset) pins. */
#define SDA_DIO 10
#define RESET_DIO A1
/* Create an instance of the RFID library */
RFID RC522(SDA_DIO, RESET_DIO);

char bluetoothData = 0; //The bluetoothData store the bluetooth data

//Settings to setup the IR Receiver
int RECV_PIN = A2;
IRrecv irrecv(RECV_PIN);
decode_results results;

//Settings to setup the Keypad
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
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.clear();
}

void loop()
{
  // receieves the input from RFID read and if it is corrent than unlock safe.
  receiveRFIDInput();

  // receieves the input from key and stores into key.
  receiveKeyPadInput();

  // receieves the input from IR and stores into key.
  receiveIRInput();

  // receieves the input from bluetooth key and stores into key.
  receiveBluetoothInput();

  //The mode where it takes the user input inorder to determine to unlock the safe
  if (!resetPassword && isDigit(key))
  {
    input = input + key;
    Serial.print(input);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(input);
    //If the user input is valid than unlock the safe
    if (vaildPassword(input))
    {
      delay(500);
      Serial.print("\nValid Password. Safe is now unlocked.");
      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print("safe unlocked.");
      lcd.setCursor(0, 0);
      lcd.print("Valid Password,");
      unlock();
      input = "";
      lcd.clear();
    }
    //if the user input is wrong than clear input
    else if (vaildInputSize(input))
    {
      delay(500);
      Serial.print("\nInvalid Password!");
      lcd.clear();
      lcd.print("Invalid Password");
      delay(1500);
      lcd.clear();
      input = "";
    }
  }

  //Enter reset mode with key is '*'
  else if (!resetPassword && key == '*')
  {
    resetPassword = true;
    input = "";
    Serial.print("\nPASSWORD RESET");
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("MODE");
    lcd.setCursor(0, 0);
    lcd.print("PASSWORD RESET");
    delay(1500);
    Serial.print("\nCurrent password.");
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("password.");
    lcd.setCursor(0, 0);
    lcd.print("Enter current");
  }

  //Checks the user input when in reset mode
  if (resetPassword && !passwordCheck && isDigit(key))
  {
    input = input + key;
    Serial.print("\n" + input);
    lcd.clear();
    lcd.print(input);
    //if the password is corrent while in reset mode than
    //allow the user to enter a new password
    if (vaildPassword(input))
    {
      delay(500);
      Serial.print("\nValid Password. Please enter new password.");
      lcd.clear();
      lcd.print("Valid Password");
      delay(1500);
      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print("password.");
      lcd.setCursor(0, 0);
      lcd.print("Enter new");
      passwordCheck = true;
      input = "";
    }
    //If the password is incorrent than exit the reset mode.
    else if (vaildInputSize(input))
    {
      delay(500);
      Serial.print("\nInvalid Password! Password reset failed!");
      lcd.clear();
      lcd.print("Invalid Password");
      delay(1500);
      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print("failed!");
      lcd.setCursor(0, 0);
      lcd.print("Password reset");
      delay(2000);
      lcd.clear();
      resetPassword = false;
      input = "";
    }
  }
  //Check user input and than makes the input the new password
  else if (resetPassword && passwordCheck && isDigit(key))
  {
    input = input + key;
    Serial.print(input);
    lcd.clear();
    lcd.print(input);
    if (vaildInputSize(input))
    {
      delay(500);
      password = input;
      Serial.print("\nNew Password: " + input);
      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print(input);
      lcd.setCursor(0, 0);
      lcd.print("New Password:");
      delay(1500);
      lcd.clear();
      resetPassword = false;
      passwordCheck = false;
      input = "";
    }
  }
  key = ' ';  //clear key for next input
}

//Check the user input if it matches the password.
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

//Check if the input is the correct length of 4 characters
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

//Check the ir code and sets it to the respective number
void receiveIRInput ()
{
  if (irrecv.decode(&results)) {
    if (results.value == 0x373119) {
      key = '0';
    }
    else if (results.value == 0x36113D) {
      key = '1';
    }
    else if (results.value == 0x37111D) {
      key = '2';
    }
    else if (results.value == 0x36912D) {
      key = '3';
    }
    else if (results.value == 0x37910D) {
      key = '4';
    }
    else if (results.value == 0x365135) {
      key = '5';
    }
    else if (results.value == 0x375115) {
      key = '6';
    }
    else if (results.value == 0x36D125) {
      key = '7';
    }
    else if (results.value == 0x37D105) {
      key = '8';
    }
    else if (results.value == 0x363139) {
      key = '9';
    }
    else if (results.value == 0x366133) {
      key = '*';
    }
    delay(100);
    irrecv.resume(); // Receive the next value
  }
}

//get the input for the
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

//Checks the serial of the RFID card and if it matchs the password than unlock the safe
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
    //If the serial number matches the password then unlock
    if (serialNumRFID == "8010115163138")
    {
      Serial.print("\nValid Card. Safe is now unlocked.");
      lcd.setCursor(0, 1);
      lcd.print("safe unlocked.");
      lcd.setCursor(0, 0);
      lcd.print("Valid Card,");
      unlock();
      lcd.clear();
    }
    serialNumRFID = ""; //Clear serialNumRFID for the next RFID card.
  }
}

//unlocks the safe
void unlock()
{
  digitalWrite(LOCK_PIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(4300);                       // wait for a second
  digitalWrite(LOCK_PIN, LOW);    // turn the LED off by making the voltage LOW
}
