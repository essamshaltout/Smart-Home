// I/O Microcontroller 
#include <SPI.h> //include the SPI bus library
#include <MFRC522.h> //include the RFID reader library
#define SS_PIN 5 // ESP32 pin GIOP5
#define RST_PIN 27 // ESP32 pin GIOP27
#define access_pin 6 //ESP32 pin GIOP22 (Has been changeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeed from 22 to 6)
#define denied_pin 7 //ESP32 pin GIOP14 (Has been changeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeed from 14 to 7)
#define lamp_pin 2
#define buzzer_pin 3
#define led_control 4

MFRC522 rfid(SS_PIN, RST_PIN); // instatiate a MFRC522 reader object.

bool lamp;
bool buzzer;
unsigned long int t_access;
unsigned long int t_denied;
void setup()
{
  Serial.begin(9600);
  pinMode(lamp_pin,OUTPUT);
  pinMode(buzzer_pin,OUTPUT);
  pinMode(led_control, OUTPUT);
  pinMode(access_pin,OUTPUT);
  pinMode(denied_pin,OUTPUT);
  SPI.begin();      // Initiate  SPI bus
  rfid.PCD_Init(); // Init MFRC522 card (in case you wonder what PCD means: proximity coupling device) 
}

void loop() 
{
    // Lamp
    while(!(Serial.available()));
    lamp = Serial.read();
    digitalWrite(lamp_pin,lamp);

    // RFID
  
    // Look for new cards
    if (!rfid.PICC_IsNewCardPresent())   return;
   
    // Select one of the available cards
    if (!rfid.PICC_ReadCardSerial()) return;
    
    String user_id="";
    for (byte i=0;i<rfid.uid.size;i++) 
    {
       user_id.concat(String(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ")); 
       user_id.concat(String(rfid.uid.uidByte[i], HEX));
    }
    
    user_id.toUpperCase();
    // E8 07 C7 D2
    if (user_id == " E0 7E E5 87") //The 1st card user_id that will access
    {
      digitalWrite(access_pin,HIGH);
      t_access = millis();
    }
    else if(user_id == " 9F 30 19 89") //The 2nd card user_id that will access
    {
      digitalWrite(access_pin,HIGH);
      t_access = millis();
    }
    else
    {
      digitalWrite(denied_pin,HIGH);
      t_denied = millis();
    }
  
    if(millis() - t_access > 500)
    {
       digitalWrite(access_pin,LOW);
    }
  
    if(millis() - t_denied > 500)
    {
       digitalWrite(denied_pin,LOW);
    }
   
    rfid.PICC_HaltA(); // halt PICC
    rfid.PCD_StopCrypto1(); // Stop encryption on PCD


    // Buzzer
    while(!(Serial.available()));
    buzzer = Serial.read();
    digitalWrite(buzzer_pin,buzzer);
}  
