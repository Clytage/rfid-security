#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>

#define SS_PIN 10
#define RST_PIN 9

MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
MFRC522::StatusCode status;
byte bufferLen = 18;
byte readBlockData[18];

LiquidCrystal lcd(2, 8, 4, 5, 6, 7);

boolean successRead = false;
String myTags[10] = {};
int tagsCount = 0;
String tagID = "";
byte readCard[4];
boolean correctTag = false;

void setup() {
    Serial.begin(9600);
    SPI.begin();
    mfrc522.PCD_Init();
    lcd.begin(16, 2);
    lcd.clear();
    Serial.println("Please scan the master tag!");
    lcd.setCursor(1,0);
    lcd.print("No master tag.");
    lcd.setCursor(4, 1);
    lcd.print("Scan now...");

    while (!successRead) {
        successRead = getID();
        if (successRead == true) {
            myTags[tagsCount] = strdup(tagID.c_str()); // Sets the master tag into position 0 in the array
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Master tag set.");
            tagsCount++;
            Serial.println("Master tag set.");
            Serial.print("Masted tag ID is: ");
            Serial.println(tagID);
        }
    }
    successRead = false;
    printNormalModeMessage();
}

void loop() {
    byte blockData_ON [16] = {"ON"};
    byte blockData_OFF [16] = {"OFF"};
    int j;
    String Pos ="";

    if ( ! mfrc522.PICC_IsNewCardPresent()) { // If a new PICC placed to RFID reader continue
        return;
    }
    if ( ! mfrc522.PICC_ReadCardSerial()) { // Since a PICC placed get Serial and continue
        return;
    }
    tagID = "";
    /* The MIFARE PICCs that we use have 4 byte UID */
    for (int i = 0; i < 4; i++) {
        readCard[i] = mfrc522.uid.uidByte[i];
        tagID.concat(String(mfrc522.uid.uidByte[i], HEX)); // Adds the 4 bytes in a single String variable
    }
    tagID.toUpperCase();
    mfrc522.PICC_HaltA(); // Stop reading
    Serial.print("\n");
    Serial.print("Detected tag ID is: ");
    Serial.println(tagID);

    /* Checks whether the scanned tag is the master tag */
    if (tagID == myTags[0]) {
        lcd.clear();
        Serial.println("Master tag detected.");
        Serial.println("Entered program mode, please add or remove tag!");
        lcd.print("Program mode:");
        lcd.setCursor(0, 1);
        lcd.print("Add or remove tag.");

        while (!mfrc522.PICC_IsNewCardPresent()) { }
        if (!mfrc522.PICC_ReadCardSerial()) {
            return;
        }
        tagID = "";
        /* The MIFARE PICCs that we use have 4 byte UID */
        for (int i = 0; i < 4; i++) {
            readCard[i] = mfrc522.uid.uidByte[i];
            tagID.concat(String(mfrc522.uid.uidByte[i], HEX)); // Adds the 4 bytes in a single String variable
        }
        tagID.toUpperCase();
        mfrc522.PICC_HaltA(); // Stop reading

        if (tagID == myTags[0]) {
            lcd.clear();
            lcd.print("Cannot add or remove.");
            lcd.setCursor(0, 1);
            lcd.print("  master tag.");
            Serial.print("\n");
            Serial.println("Cannot add or remove master tag.");
            delay(1500);
            printNormalModeMessage();
            return;
        }

        for (byte i = 0; i < 6; i++) {
            key.keyByte[i] = 0xFF;
        }

        boolean newCard = false;
        while (!newCard) {
            if (!mfrc522.PICC_IsNewCardPresent()) {continue;}
            // Serial.print("ye");
            if (!mfrc522.PICC_ReadCardSerial()) {continue;}
            // Serial.print("yee");
            newCard = true;
        }

        Serial.print("\n");
        Serial.println("**Card Detected**");

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

        if (Pos == "ON") {
            void Clear();
            lcd.clear();
            lcd.print("   tag was ON.");
            Serial.print("\n");
            WriteDataToBlock(2, blockData_OFF);
            Serial.println("Writing to Data Block/Chaning position to OFF...");
            lcd.setCursor(0,1);
            lcd.print("Tag turned OFF.");
            delay(1000);
        }

        else {
            void Clear();
            lcd.clear();
            lcd.print("  tag was OFF.");
            Serial.print("\n");
            Serial.println("Writing to Data Block/Chaning position to ON...");
            WriteDataToBlock(2, blockData_ON);
            lcd.setCursor(0,1);
            lcd.print("Tag turned ON.");
            delay(1000);
        }

        printNormalModeMessage();
        Serial.print("\n");
        mfrc522.PICC_HaltA(); // Halt PICC
        mfrc522.PCD_StopCrypto1();  // Stop encryption on PCD
        return;
    }

    if (tagID != myTags[0]) {
        lcd.clear();
        lcd.print(" **Card Detected**");
        lcd.setCursor(0,1);
        lcd.print("Not master tag.");
        Serial.println("Not master tag.");

        for (byte i = 0; i < 6; i++) {
            key.keyByte[i] = 0xFF;
        }
        
        while (!mfrc522.PICC_IsNewCardPresent()) {}

        while (!mfrc522.PICC_ReadCardSerial()) {
            return;
        }

        /* Read data from the same block */
        Serial.print("\n");
        Serial.println("Reading from Data Block 2/Determining position...");
        ReadDataFromBlock(2, readBlockData);
        /* Print the data read from block */
        Serial.print("Data in Block:2/Position --> ");
        for (int j=0 ; j<16 ; j++) {
            Pos.concat((char)(readBlockData[j]));
            Serial.write(readBlockData[j]);
        }

        if (Pos == "ON") {
            lcd.clear();
            lcd.print("Position is ON");
            lcd.setCursor(15,0);
            lcd.print("  ");
            delay(1000);
        }

        else {
            lcd.clear();
            lcd.print("Position is OFF");
            lcd.setCursor(15,0);
            lcd.print("  ");
            delay(1000);
        }

        printNormalModeMessage();
        Serial.print("\n");
        mfrc522.PICC_HaltA(); // Halt PICC
        mfrc522.PCD_StopCrypto1();  // Stop encryption on PCD
        return;
    }
}

int getID() {
    // Getting ready for Reading PICCs
    if ( ! mfrc522.PICC_IsNewCardPresent()) { // If a new PICC placed to RFID reader continue
        return 0;
    }
    if ( ! mfrc522.PICC_ReadCardSerial()) { // Since a PICC placed get Serial and continue
        return 0;
    }
    tagID = "";
    for ( int i = 0; i < 4; i++) { // The MIFARE PICCs that we use have 4 byte UID
        readCard[i] = mfrc522.uid.uidByte[i];
        tagID.concat(String(mfrc522.uid.uidByte[i], HEX)); // Adds the 4 bytes in a single String variable
    }

    tagID.toUpperCase();
    // mfrc522.PICC_HaltA(); // Stop reading
    return 1;
}

void printNormalModeMessage() {
    delay(1500);
    lcd.clear();
    lcd.print(" Programmer tag");
    lcd.setCursor(0, 1);
    lcd.print(" Please scan your tag!");
}

void WriteDataToBlock(int blockNum, byte blockData[]) {
    /* Authenticating the desired data block for write access using Key A */
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNum, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
        Serial.print("Authentication failed for Write: ");
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }
    else {
        Serial.println("Authentication success.");
    }
    /* Write data to the block */
    status = mfrc522.MIFARE_Write(blockNum, blockData, 16);
    if (status != MFRC522::STATUS_OK) {
        Serial.print("Writing to Block failed: ");
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }
    else {
        Serial.println("Successfully written data into Block.");
    }
}

void ReadDataFromBlock(int blockNum, byte readBlockData[]) {
    /* Authenticating the desired data block for Read access using Key A */
    byte status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNum, &key, &(mfrc522.uid));

    if (status != MFRC522::STATUS_OK) {
        Serial.print("Authentication failed for Read: ");
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
}

void Clear() {
    for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

    byte buffer[34]={0};
    byte block;
    MFRC522::StatusCode status;
    byte len;

    block = 1;
    // Serial.println(F("Authenticating using key A..."));
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("PCD_Authenticate() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }
    else Serial.println(F("PCD_Authenticate() success: "));

    /* Write block */
    status = mfrc522.MIFARE_Write(block, buffer, 16);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Write() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }
    else Serial.println(F("MIFARE_Write() success: "));

    block = 2;
    // Serial.println(F("Authenticating using key A..."));
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("PCD_Authenticate() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }

    /* Write block */
    status = mfrc522.MIFARE_Write(block, &buffer[16], 16);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Write() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }
    else Serial.println(F("MIFARE_Write() success: "));

    block = 4;
    // Serial.println(F("Authenticating using key A..."));
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("PCD_Authenticate() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }

    /* Write block */
    status = mfrc522.MIFARE_Write(block, buffer, 16);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Write() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }
    else Serial.println(F("MIFARE_Write() success: "));

    block = 5;
    // Serial.println(F("Authenticating using key A..."));
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("PCD_Authenticate() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }

    /* Write block */
    status = mfrc522.MIFARE_Write(block, &buffer[16], 16);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Write() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }
    else {
        Serial.println(F("MIFARE_Write() success: "));
    }
}
