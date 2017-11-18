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
String input = "";
bool  resetPassword = false;
bool  passwordCheck = false;
void setup() {
  Serial.begin(9600);
  // set  the transistor pin as output:
  pinMode(transistorPin, OUTPUT);
}

void loop()
{
  char key = keypad.getKey();

  if (!resetPassword && containsDigit(key))
  {
    input = input + key;
    Serial.print("\n" + input);
    if (vaildPassword(input))
    {
      Serial.print("\nValid Password. Safe is now unlocked.");
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

  if (resetPassword && !passwordCheck && containsDigit(key))
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

  else if (resetPassword && passwordCheck && containsDigit(key))
  {
    input = input + key;
    Serial.print("\n" + input);
    if (vaildInputSize(input))
    {
      password = input;
      Serial.print("\nNew Password: " + input);
      resetPassword = false;
      input = "";
    }
  }
}

bool containsDigit(char keyChar)
{
  if (keyChar == '0' || keyChar == '1' || keyChar == '2' || keyChar == '3' || keyChar == '4' || keyChar == '5' || keyChar == '6' || keyChar == '7' || keyChar == '8' || keyChar == '9')
  {
    return true;
  }
  else
  {
    return false;
  }
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




