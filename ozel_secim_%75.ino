#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); 

const int buttonPinD2 = 2;  
const int replaceButtonPinD10 = 12;  

int buttonStateD2 = 0;  
int replaceButtonStateD10 = 0;  
int buttonPressCount = 0;  


char characters[] = "|_|_|_|_|_|_|_|_|";  
char charactersAlt[] = "|_|_|_|_|_|_|_|_";  
int currentPosition = 0;  
int currentPositionAlt = 0;  
bool isTopRowFinished = false;  
bool isOnBottomRow = false;  


int characterNumbers[] = {1, 2, 3, 4, 5, 6, 7, 8};


bool selectedTopRow[8] = {false, false, false, false, false, false, false, false};
bool selectedBottomRow[8] = {false, false, false, false, false, false, false, false};

void setup() {
  pinMode(buttonPinD2, INPUT);  
  pinMode(replaceButtonPinD10, INPUT);  

  lcd.init();  
  lcd.backlight();    
  lcd.clear();  
  lcd.setCursor(0, 0);  
  lcd.print(characters);  
  lcd.setCursor(0, 1);  
  lcd.print(charactersAlt); 
}

void loop() {

  buttonStateD2 = digitalRead(buttonPinD2);


  if (buttonStateD2 == HIGH) {
    if (!isTopRowFinished) {  

      lcd.setCursor(currentPosition, 0);
      lcd.print('*');
      delay(200);  

  
      do {
        currentPosition++;
        if (currentPosition >= sizeof(characters) - 1) {
          currentPosition = 0;  
          isTopRowFinished = true;  
        }
      } while (characters[currentPosition] != '_');  

    
      lcd.setCursor(currentPosition, 0);
      lcd.print('_');
    } else {  
      isOnBottomRow = true;
      lcd.setCursor(currentPositionAlt, 1);
      lcd.print('*');
      delay(200);

     
      do {
        currentPositionAlt++;
        if (currentPositionAlt >= sizeof(charactersAlt) - 1) {
          currentPositionAlt = 0;  
                  }
      } while (charactersAlt[currentPositionAlt] != '_');  

      lcd.setCursor(currentPositionAlt, 1);
      lcd.print('_');
    }
    delay(100);  
  }


  replaceButtonStateD10 = digitalRead(replaceButtonPinD10);

  if (replaceButtonStateD10 == HIGH) {
    if (!isTopRowFinished && characters[currentPosition] == '_') {

      int numberToPlace = characterNumbers[currentPosition / 2 % 8];
      characters[currentPosition] = numberToPlace + '0'; 
      lcd.setCursor(currentPosition, 0);
      lcd.print(characters[currentPosition]);
      selectedTopRow[currentPosition / 2 % 8] = true;  
    } else if (isTopRowFinished && charactersAlt[currentPositionAlt] == '_') {
      int numberToPlace = characterNumbers[currentPositionAlt / 2 % 8];
      charactersAlt[currentPositionAlt] = numberToPlace + '0';  
      lcd.setCursor(currentPositionAlt, 1);
      lcd.print(charactersAlt[currentPositionAlt]);
      selectedBottomRow[currentPositionAlt / 2 % 8] = true;
    }
    delay(200);  
  }


  lcd.setCursor(0, 0);
  lcd.print(characters);
  lcd.setCursor(0, 1);
  lcd.print(charactersAlt);


  if (charactersAlt[15] != '_') {
    lcd.clear();
    lcd.setCursor(0, 0);
    for (int i = 0; i < 8; i++) {
      lcd.print(selectedTopRow[i] ? '1' : '0');
    }
    lcd.setCursor(0, 1);
    for (int i = 0; i < 8; i++) {
      lcd.print(selectedBottomRow[i] ? '1' : '0');
    }
    delay(3000);
    lcd.clear();
  }

  delay(100);
}
