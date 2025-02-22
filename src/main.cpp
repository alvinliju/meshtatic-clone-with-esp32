#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);
#define CE_PIN 22
#define CSN_PIN 21

byte address[][6] = {"1Node", "2Node"}; // Fixed spelling

RF24 radio(CE_PIN, CSN_PIN);

void displayMessage(const char* message) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB14_tr);
  u8g2.drawStr(0, 20, message); 
  u8g2.sendBuffer();
}

void setup(void) {
  Serial.begin(9600);
  Wire.begin(25,26);  // SDA, SCL
  u8g2.begin();  // Initialize display ONCE here
  
  displayMessage("Initializing...");
  Serial.println("\n\nStarting...");
  delay(5000);


  // Radio initialization
  if (!radio.begin()) {
    displayMessage("Radio FAIL");
    Serial.println("Radio: FAIL");
    while(1); // Halt on failure
  }

  if( !radio.isChipConnected() ) {
    Serial.println("Chip: NOT FOUND");
    displayMessage("Radio FAIL");
    delay(2000);
    while(1); // Halt on failure
  }

  radio.setPALevel(RF24_PA_MIN);
  radio.setDataRate(RF24_2MBPS);
  radio.setChannel(124);


  radio.openWritingPipe(address[1]);
  radio.openReadingPipe(1, address[0]);

  randomSeed(analogRead(A0)); // Randomize the seed
  
  radio.stopListening(); // Critical for transmitter! 
  
  displayMessage("Radio ready");
  
}

void loop() {

  unsigned char data = random(0, 255);

  radio.stopListening();

  if(!radio.write(&data, sizeof(unsigned char))) {
   displayMessage("Failed to send");
    delay(2000);
    while(1); 
  }

  if (!radio.write( &data, sizeof(unsigned char) )) {
    displayMessage("Failed to send");
    delay(2000);
  }

  radio.startListening();
  
  // But we won't listen for long, 200 milliseconds is enough
  unsigned long started_waiting_at = millis();

  // Loop here until we get indication that some data is ready for us to read (or we time out)
  while ( ! radio.available() ) {

    // Oh dear, no response received within our timescale
    if (millis() - started_waiting_at > 200 ) {
      displayMessage("Timeout");
      Serial.println("No response received - timeout!");
      return;
    }
  }

  
  displayMessage("Message sent");
  delay(1000);
  
}
 
