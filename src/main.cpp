#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0,  U8X8_PIN_NONE);
#define CE_PIN 4
#define CSN_PIN 5

const byte reciverAddress[5] = {'R','x','A','A','A'};

RF24 radio(CE_PIN, CSN_PIN);

char dataRecived[16];
bool newData = false;

void displayMessage(const char* message){
  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB14_tr);
  u8g2.drawStr(0, 20, message); 
  u8g2.sendBuffer();
  delay(1000);
}

void sendData() {
  displayMessage("Sending...");
  
  radio.stopListening();
  char text[] = "Hello World!"; // Keep messages under 16 chars
  radio.write(&text, sizeof(text));
  
  if(!radio.write(&text, sizeof(text))) {
    displayMessage("mesage Failed!");
    
  } else {
    displayMessage("Sent OK!");
  }
  delay(2000);
}

void setup(void) {

  Serial.begin(9600);
  Serial.println("SimpleRx Starting");

  displayMessage("Radio initializing..");

  bool radioSucess = radio.begin();

  if(!radioSucess){
    displayMessage("Radio failed to start");
  }
  
  radio.setDataRate(RF24_250KBPS);
  radio.openWritingPipe(reciverAddress); // Receiver's address
  radio.setRetries(3, 5); // Retry 3x with 5ms delay
  
  displayMessage("Radio started");

}


void getData(){
  if(radio.available()){
    radio.read(&dataRecived, sizeof(dataRecived));
    displayMessage(dataRecived);
    newData = true; //for state management
  }
}

void showData(){
  if(newData == true){
    displayMessage(dataRecived);
    newData = false;
  }
}

void loop(){
  // getData();
  // showData();
  sendData(); 
}

