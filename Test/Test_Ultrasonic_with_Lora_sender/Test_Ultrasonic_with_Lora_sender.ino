#include "SSD1306.h"
#include <SPI.h>
#include <LoRa.h>

// GPIO5  -- SX1278's SCK
// GPIO19 -- SX1278's MISO
// GPIO27 -- SX1278's MOSI
// GPIO18 -- SX1278's CS
// GPIO14 -- SX1278's RESET
// GPIO26 -- SX1278's IRQ(Interrupt Request)
//OLED pins to ESP32 0.96OLEDGPIOs via this connecthin:
//OLED_SDA -- GPIO4
//OLED_SCL -- GPIO15
//OLED_RST -- GPIO16

SSD1306  display(0x3c, 4, 15);

#define SS      18
#define RST     14
#define DI0     26
#define BAND    868E6  //915E6

void setup() {
  Serial.begin(9600);
  
  pinMode(16, OUTPUT);
  digitalWrite(16, LOW);    // set GPIO16 low to reset OLED
  delay(50);
  digitalWrite(16, HIGH); // while OLED is running, must set GPIO16 in high
  
  SPI.begin(5, 19, 27, 18);
  LoRa.setPins(SS, RST, DI0);
  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_24);
}
void loop() {

  long duration, cm;

  pinMode(21, OUTPUT);
 
  digitalWrite(21, LOW);
  delayMicroseconds(2);
  digitalWrite(21, HIGH);
  delayMicroseconds(5);
  digitalWrite(21, LOW);
  pinMode(13, INPUT);
  duration = pulseIn(13, HIGH);
   
  cm = microsecondsToCentimeters(duration);

  Serial.print(cm);
  Serial.print("cm");
  Serial.println();

  LoRa.beginPacket();
  LoRa.print(cm);
  LoRa.print(" cm");
  LoRa.endPacket();
  
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(10, 5, String(cm)+" cm");
  // write the buffer to the display
  display.display();
 
  delay(100);                       
}

long microsecondsToCentimeters(long microseconds)
{
// The speed of sound is 340 m/s or 29 microseconds per centimeter.
// The ping travels out and back, so to find the distance of the
// object we take half of the distance travelled.
return microseconds / 29 / 2;
}
