#ifndef BLUETOOTH_HANDLER_H
#define BLUETOOTH_HANDLER_H

#include <SoftwareSerial.h>
#include <EEPROM.h>

// External references from main file
extern SoftwareSerial mySerial;
extern const int EEPROM_ADDRESS;

// Function declarations
void saveUserId(String userId);
String readUserId();
void captureBluetoothData();
void sendKeepAliveSignal();

void captureBluetoothData() {
    if (mySerial.available()) {  
        String receivedMessage = "";  

        while (mySerial.available()) {  
            char receivedChar = mySerial.read();  
            receivedMessage += receivedChar;  
            delay(5);  
        }

        Serial.print("Received: ");  
        Serial.println(receivedMessage);  

        // Extract UserId from received message
        int userIdIndex = receivedMessage.indexOf("\"userId\": \"");
        int actionIndex = receivedMessage.indexOf("\"action\": \"");

        String extractedUserId = "";
        String extractedAction = "";

        if (userIdIndex != -1) {
            int startIndex = userIdIndex + 11; 
            int endIndex = receivedMessage.indexOf("\"", startIndex);
            extractedUserId = receivedMessage.substring(startIndex, endIndex);
            Serial.print("Extracted UserId: ");
            Serial.println(extractedUserId);
        }

        if (actionIndex != -1) {
            int startIndex = actionIndex + 10;
            int endIndex = receivedMessage.indexOf("\"", startIndex);
            extractedAction = receivedMessage.substring(startIndex, endIndex);
            Serial.print("Extracted Action: ");
            Serial.println(extractedAction);

            // Check if action is "auth" before saving UserId
            if (extractedAction == "auth") {
                Serial.println("Auth action received! Saving UserId...");
                saveUserId(extractedUserId);
            }
        }

        // Check if UserId matches the one stored in EEPROM
        String storedUserId = readUserId();
        if (extractedUserId == storedUserId) {
            Serial.println("UserId Matched: Verified Successfully!");
            mySerial.println("OK");  // Send response to keep connection alive
        } else {
            Serial.print("Stored UserId: ");
            Serial.println(storedUserId);
            Serial.println("UserId is NOT Matched!");
        }

        // Keep the connection alive by sending a signal
        sendKeepAliveSignal();
    }
}

// Function to save UserId in EEPROM
void saveUserId(String userId) {
    Serial.println("Saving UserId to EEPROM...");
    for (int i = 0; i < userId.length(); i++) {
        EEPROM.write(EEPROM_ADDRESS + i, userId[i]);
    }
    EEPROM.write(EEPROM_ADDRESS + userId.length(), '\0');
    Serial.println("UserId Saved!");
}

// Function to read UserId from EEPROM
String readUserId() {
    String userId = "";
    char ch;
    for (int i = 0; i < 20; i++) {
        ch = EEPROM.read(EEPROM_ADDRESS + i);
        if (ch == '\0') break;
        userId += ch;
    }
    Serial.print("Stored UserId from EEPROM: ");
    Serial.println(userId);
    return userId;
}

// Function to send a keep-alive signal to prevent disconnection
void sendKeepAliveSignal() {
    Serial.println("Sending keep-alive signal...");
    mySerial.println("KEEP_ALIVE");  // Send a dummy message to keep the connection alive
}

#endif
