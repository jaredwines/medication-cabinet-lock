#include <Keypad.h>

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
const int transistorPin = 9;    // connected to the base of the transistor

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
String tempPassword = "";
String input = "";
bool  resetPass = false;
void setup() {
  Serial.begin(9600);
  // set  the transistor pin as output:
  pinMode(transistorPin, OUTPUT);
}

void loop() {
  char key = keypad.getKey();

  if (resetPass == true && (key == '0' || key == '1' || key == '2' || key == '3' || key == '4' || key == '5' || key == '6' || key == '7' || key == '8' || key == '9'))
  {
    input = input + key;
    Serial.print("\n" + input);
    checkPassword(input);
    checkInputSize(input);

  }

  if (resetPass == false && (key == '0' || key == '1' || key == '2' || key == '3' || key == '4' || key == '5' || key == '6' || key == '7' || key == '8' || key == '9'))
  {
    input = input + key;
    Serial.print("\n" + input);
    checkPassword(input);
    checkInputSize(input);
  }
  else if (resetPass == false && key == '*')
  {
    resetPassword();
  }
}

void checkPassword(String inputStr)
{
  if (inputStr == password && resetPass == false)
  {
    Serial.print("\nUnlocked");
    // digitalWrite(transistorPin, HIGH);
    // delay(5000);
    // digitalWrite(transistorPin, LOW);
    // delay(1000);
  }
  else if (inputStr == password && resetPass == true)
  {
    
  }
}

void checkInputSize(String inputStr)
{
  if (inputStr.length() > 3 && resetPass == false)
  {
    input = "";
    Serial.print("\nReset Input");
  }
  if (inputStr.length() > 3 && resetPass == true)
  {
    input = "";
    Serial.print("\nReset Input");
  }
}

void resetPassword()
{
  resetPass = true;
  input = "";
  Serial.print("\nPASSWORD RESET");
  Serial.print("\nEnter current password.");
}



