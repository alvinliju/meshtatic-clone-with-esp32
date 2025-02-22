#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0,  U8X8_PIN_NONE);
#define CE_PIN 4
#define CSN_PIN 5

const byte thisSlaveAddress[5] = {'R','x','A','A','A'};

RF24 radio(CE_PIN, CSN_PIN);

char dataRecived[16];
bool newData = false;


void setup(void) {
  Serial.begin(9600);
  delay(1000);
  Serial.println("SimpleRx Starting");
  radio.begin();
  radio.setDataRate(RF24_250KBPS);
  radio.openReadingPipe(1, thisSlaveAddress);
  radio.startListening();
  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB14_tr);
  u8g2.drawStr(0, 20, "radio started");
  delay(1000);
  sendData();
}

void loop(){
  getData();
  showData();
}

void sendData(){
  radio.stopListening();
  char text[] = "Hello Fuckers";
  radio.write(&text, sizeof(text));
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB14_tr);
  u8g2.drawStr(0, 20, "Data sent");
  u8g2.sendBuffer();
  radio.startListening();
}

void getData(){
  if(radio.available()){
    char dataRecived[0] ;
    radio.read(&dataRecived, sizeof(dataRecived));
    newData = true; //for state management
  }
}

void showData(){
  if(newData == true){
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB14_tr);
    u8g2.drawStr(0, 20, dataRecived);
    u8g2.sendBuffer();
    newData = false;
  }
}