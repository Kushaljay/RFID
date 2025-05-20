#include <SPI.h>
#include <MFRC522.h>

#define RSTPIN 9
#define SSPIN 10
MFRC522 rc(SSPIN, RSTPIN);

// Two allowed RFID card UIDs
byte defcard[2][4] = {
  {0xC6, 0xFD, 0xC5, 0x32},
  {0x32, 0xD7, 0x0F, 0x0B}
};

byte readcard[4];

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rc.PCD_Init();

  pinMode(6, OUTPUT); // Green LED (access granted)
  pinMode(5, OUTPUT); // Red LED (access denied)
  pinMode(7, OUTPUT); // Solenoid control (via relay or MOSFET)
  
  digitalWrite(7, LOW); // Make sure solenoid is off at start

  Serial.println("Scan your card");
}

void loop() {
  if (!getid()) return;

  bool match = false;
  for (int i = 0; i < 2; i++) {
    if (!memcmp(readcard, defcard[i], 4)) {
      match = true;
      break;
    }
  }

  if (match) {
    Serial.println("Access Granted");
    digitalWrite(6, HIGH);    // Green LED on
    digitalWrite(7, HIGH);    // Solenoid activated
    delay(2000);
    digitalWrite(6, LOW);
    digitalWrite(7, LOW);     // Solenoid off
  } else {
    Serial.println("Access Denied");
    digitalWrite(5, HIGH);    // Red LED on
    delay(2000);
    digitalWrite(5, LOW);
  }

  delay(500);
}

int getid() {
  if (!rc.PICC_IsNewCardPresent()) return 0;
  if (!rc.PICC_ReadCardSerial()) return 0;

  for (int i = 0; i < 4; i++) {
    readcard[i] = rc.uid.uidByte[i];
  }

  rc.PICC_HaltA();
  return 1;
}