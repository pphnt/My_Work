#include <ArduinoJson.h>

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
    
}

void loop() {
  char json[]= "{\"topic\":\"Sensor\",\"value\":[50,80]}";
  while(!Serial) continue;
  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, json);//การรับข้อมูลเราจะแกะข้อมูลที่ได้รับมา เรียกว่าการทำ Deserialize
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }
  
  const char* Topic = doc["topic"];
  int value_1 = doc["value"][0];
  int value_2 = doc["value"][1];
  
  delay(5000);
  Serial.println(Topic);
  Serial.println(value_1);
  Serial.println(value_2);
  delay(5000);
  //Serial.println(doc);

  // put your main code here, to run repeatedly: 
  
}
