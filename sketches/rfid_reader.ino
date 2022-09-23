#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>

#define SS_PIN 10
#define RST_PIN 7

LiquidCrystal lcd(2, 8, 4, 5, 6, 7);
MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
MFRC522::StatusCode status;
byte bufferLen = 18;
byte readBlockData[18];
String Pos = "";

void setup() {
    pinMode(3, OUTPUT);
    Serial.begin(9600);
    SPI.begin();
    mfrc522.PCD_Init();
    lcd.begin(16, 2);
    Serial.println("Scan a MIFARE 1K tag to determine is it ON or OFF!");
    lcd.clear();
    Serial.println("No tag detected.");
    lcd.setCursor(0,0);
    lcd.print("No tag detected.");
}

void loop() {
    String Pos = "";

    for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
    }
    if ( ! mfrc522.PICC_IsNewCardPresent()) {
        return;
    }
    if ( ! mfrc522.PICC_ReadCardSerial()) {
        return;
    }

    Serial.print("\n");
    Serial.println("**Card Detected**");
    /* Print UID of the card */
    Serial.print(F("Card UID:"));
    for (byte i = 0; i < mfrc522.uid.size; i++) {
        Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(mfrc522.uid.uidByte[i], HEX);
    }

    Serial.print("\n");
    /* Print type of card (for example, MIFARE 1K) */
    Serial.print(F("PICC type: "));
    MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
    Serial.println(mfrc522.PICC_GetTypeName(piccType));

    /* Read data from the same block */
    Serial.print("\n");
    Serial.println("Reading from Data Block 2/Determining position...");
    ReadDataFromBlock(2, readBlockData);
    /* Print the data read from block */
    Serial.print("\n");
    Serial.print("Data in Block:2/Position --> ");
    for (int j=0 ; j<16 ; j++) {
        Pos.concat((char)(readBlockData[j]));
        Serial.write(readBlockData[j]);
    }

    if (Pos == "OFF") {
        Serial.print("\n");
        Serial.println("This tag is allowed to be removed.");
        lcd.clear();
        lcd.print("This tag IS ");
        lcd.setCursor(0, 1);
        lcd.print("ALLOWED to REMOVE!");
        printNormalModeMessage();      
    }

    if (Pos == "ON") {
        Serial.print("\n");
        Serial.println("This tag is not allowed to be removed.");
        lcd.clear();
        lcd.print("This tag ISN'T");
        lcd.setCursor(0, 1);
        lcd.print("ALLOWED to REMOVE!");
        digitalWrite(3, HIGH);
        delay(200);
        digitalWrite(3,LOW);
        delay(200);
        digitalWrite(3, HIGH);
        delay(200);
        digitalWrite(3,LOW);
        delay(200);
        digitalWrite(3, HIGH);
        delay(200);
        digitalWrite(3,LOW);
        delay(200);
        digitalWrite(3, HIGH);
        delay(200);
        digitalWrite(3,LOW);
        printNormalModeMessage();
    }

    Serial.print("\n");
    mfrc522.PICC_HaltA(); // Halt PICC
    mfrc522.PCD_StopCrypto1();  // Stop encryption on PCD
    return;
}

void ReadDataFromBlock(int blockNum, byte readBlockData[]) {
    /* Authenticating the desired data block for Read access using Key A */
    byte status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNum, &key, &(mfrc522.uid));

    if (status != MFRC522::STATUS_OK) {
        Serial.print("Authentication failed for read: ");
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }
    else {
        Serial.println("Authentication success.");
    }

    /* Reading data from the Block */
    status = mfrc522.MIFARE_Read(blockNum, readBlockData, &bufferLen);
    if (status != MFRC522::STATUS_OK) {
        Serial.print("Reading failed: ");
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
        }
    else {
        Serial.println("Block was read successfully.");  
    }
    
    mfrc522.PICC_HaltA(); // Halt PICC
    mfrc522.PCD_StopCrypto1();  // Stop encryption on PCD
}

void printNormalModeMessage() {
    delay(1500);
    lcd.clear();
    Serial.print("\n");
    Serial.println("No tag detected.");
    lcd.setCursor(0,0);
    lcd.print("No tag detected.");
}
