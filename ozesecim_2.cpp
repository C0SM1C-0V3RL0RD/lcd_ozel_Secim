#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int buttonPinD2 = 2;
const int replaceButtonPinD10 = 12;
const int S[4] = {9, 8, 7, 6};  // S0, S1, S2, S3 pinleri


const int signal = 11;

int buttonStateD2 = 0;
int replaceButtonStateD10 = 0;

char characters[] = "|_|_|_|_|_|_|_|_|";
char charactersAlt[] = "|_|_|_|_|_|_|_|_";
int currentPosition = 0;
int currentPositionAlt = 0;
bool isTopRowFinished = false;
bool isOnBottomRow = false;

int characterNumbers[] = {1, 2, 3, 4, 5, 6, 7, 8};

bool selectedTopRow[8] = {false, false, false, false, false, false, false, false};
bool selectedBottomRow[8] = {false, false, false, false, false, false, false, false};

// **SEÇİMLERİ SAKLAYACAK DEĞİŞKENLER**
char topRowResult[9] = "00000000";    
char bottomRowResult[9] = "00000000";
char pin_states[17] = "0000000000000000"; // 16 bitlik birleşik seçim değişkeni

void setup() {
    pinMode(buttonPinD2, INPUT);
    pinMode(replaceButtonPinD10, INPUT);
    pinMode(signal, OUTPUT);

    digitalWrite(signal, 1);

    lcd.init();
    lcd.backlight();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(characters);
    lcd.setCursor(0, 1);
    lcd.print(charactersAlt);

    Serial.begin(9600); // **Seri iletişimi başlat**


    // Mutex pinlerini çıkış olarak ayarla
    for (int i = 0; i < 4; i++) {
        pinMode(S[i], OUTPUT);
    }
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

    // **SEÇİMLER KAYDEDİLİYOR VE BİRLEŞTİRİLİYOR**
    if (charactersAlt[15] != '_') {
        lcd.clear();
        lcd.setCursor(0, 0);
        for (int i = 0; i < 8; i++) {
            topRowResult[i] = selectedTopRow[i] ? '1' : '0';
            lcd.print(topRowResult[i]);
        }
        topRowResult[8] = '\0'; // String bitiş karakteri

        lcd.setCursor(0, 1);
        for (int i = 0; i < 8; i++) {
            bottomRowResult[i] = selectedBottomRow[i] ? '1' : '0';
            lcd.print(bottomRowResult[i]);
        }
        bottomRowResult[8] = '\0'; // String bitiş karakteri

        // **Birleştirerek 'pin_states' değişkenine kaydet**
        strcpy(pin_states, topRowResult);
        strcat(pin_states, bottomRowResult); // İki stringi birleştir

        delay(3000);
        lcd.clear();

        // **Saklanan verileri seri porttan gönder**
        Serial.print("Top Row: ");
        Serial.println(topRowResult);
        Serial.print("Bottom Row: ");
        Serial.println(bottomRowResult);
        Serial.print("Pinler (16-bit birleşik): ");
        Serial.println(pin_states);

        // **Pin seçimini mutex sistemine gönder**
        mutex_control();
    }

    delay(100);
}

// **Mutex Kontrol Fonksiyonu**
void mutex_control() {
    int active_sensor = 0;
    for (int i = 0; i < 16; i++) {
        if (pin_states[i] == '1') {
            active_sensor++;
        }
    }

    Serial.print("Aktif Sensor Sayisi: ");
    Serial.println(active_sensor);

    for (int i = 0; i < 16; i++) {
        if (pin_states[i] == '1') {
            int s0 = i & 0x01;
            int s1 = (i >> 1) & 0x01;
            int s2 = (i >> 2) & 0x01;
            int s3 = (i >> 3) & 0x01;

            digitalWrite(S[0], s0);
            digitalWrite(S[1], s1);
            digitalWrite(S[2], s2);
            digitalWrite(S[3], s3);

            Serial.print("Sensör ");
            Serial.print(i);
            Serial.print(" aktif! S0-S3: ");
            Serial.print(s3);
            Serial.print(s2);
            Serial.print(s1);
            Serial.println(s0);
        }
    }
} 
