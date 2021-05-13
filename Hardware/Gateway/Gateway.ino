#include <SPI.h>
#include <LoRa.h>
//#include "SSD1306.h"
#include <ArduinoJson.h>
#include<WiFi.h>
#include<FirebaseESP32.h>

#define WIFI_STA_NAME "EarthiPhone"
#define WIFI_STA_PASS "eieiearth"

#define FIREBASE_HOST "https://smartparking-1cfaf-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "9j36MHk8uDwnc8uV4IoCf9eIo95v3hyV3ihP3K1U"

//Define FirebaseESP32 data object
FirebaseData fbdo;

FirebaseJson json;

int CheckThreshold(int);

// GPIO5  -- SX1278's SCK
// GPIO19 -- SX1278's MISO
// GPIO27 -- SX1278's MOSI
// GPIO18 -- SX1278's CS
// GPIO14 -- SX1278's RESET
// GPIO26 -- SX1278's IRQ(Interrupt Request)

//OLED pins to ESP32 0.96OLEDGPIOs 
//OLED_SDA -- GPIO4
//OLED_SCL -- GPIO15
//OLED_RST -- GPIO16

//SSD1306  display(0x3c, 4, 15);
#define SS      18
#define RST     14
#define DI0     26
#define BAND    915E6

int Node1_1, Node1_2, Node2_1, Node2_2, Node3_1, Node3_2, Node4_1, Node4_2;
int Node1_pklost_counter = 0, Node2_pklost_counter = 0, Node3_pklost_counter = 0, Node4_pklost_counter = 0;
String tmp_string="", tmp_rssi;
bool Node1_pklost = false, Node2_pklost = false, Node3_pklost = false, Node4_pklost = false;

void setup() {
  Serial.begin(115200);
  while (!Serial);
  
  //connecting lora
  Serial.println("LoRa Receiver");
  SPI.begin(5, 19, 27, 18);
  LoRa.setPins(SS, RST, DI0);
  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("Starting LoRa Successed!");
  Serial.println("");
  
  //connecting wifi
  Serial.print("Starting WiFi");
  Serial.print("Connecting to ");
  Serial.println(WIFI_STA_NAME);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_STA_NAME, WIFI_STA_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("");
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  //Set database read timeout to 1 minute (max 15 minutes)
  Firebase.setReadTimeout(fbdo, 1000 * 60);
  
  //tiny, small, medium, large and unlimited.
  //Size and its write timeout e.g. tiny (1s), small (10s), medium (30s) and large (60s).
  Firebase.setwriteSizeLimit(fbdo, "small");

  //optional, set the decimal places for float and double data to be stored in database
  //Firebase.setFloatDigits(2);
  //Firebase.setDoubleDigits(6);
}

void loop() {
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  // received a packet
  if (packetSize) {
    // read packet
    while (LoRa.available()) {
      tmp_string += (char)LoRa.read();      
    }
    Serial.println("Received packet: " + tmp_string);

    //print RSSI of packet
    tmp_rssi = LoRa.packetRssi();
    Serial.println("RSSI: " + tmp_rssi);
  
    //Parse Packet to JsonObject variable
    DynamicJsonDocument jsonBuffer(1024 + tmp_string.length());
    DeserializationError error = deserializeJson(jsonBuffer, tmp_string);
    if (error) {
      Serial.println("DeserializationError\n");
      tmp_string="";
      return;
    }
    JsonObject PacketJsonObject = jsonBuffer.as<JsonObject>();
    
    //parse object to setting data that will send to firebase
    if (PacketJsonObject.containsKey("topic")) {
      //check if Packet is comform server node or not
      if (String("log_server").equals(PacketJsonObject["topic"].as<String>())) {
        //Check if Packet form client lost
        //node1
        if(String("Packet_Lost").equals(PacketJsonObject["Node1"].as<String>()))
        {
          Serial.println("Node 1 Packet_Lost");
          Node1_pklost = true;
          Node1_pklost_counter += 1;
          Serial.println((String) "Node 1 Lost Counter = " + Node1_pklost_counter);
        }
        else
        {
          Serial.println("Node 1 Packet_Found");
          Node1_pklost = false;
          Node1_pklost_counter = 0;
          Serial.println("Node1_Sensor1=" + PacketJsonObject["Node1"]["Sensor1"].as<String>() + ", Node1_Sensor2=" + PacketJsonObject["Node1"]["Sensor2"].as<String>());
        }
        Serial.println("");
        
        //node2
        if(String("Packet_Lost").equals(PacketJsonObject["Node2"].as<String>()))
        {
          Serial.println("Node 2 Packet_Lost");
          Node2_pklost = true;
          Node2_pklost_counter += 1;
          Serial.println((String) "Node 2 Lost Counter = " + Node2_pklost_counter);
        }
        else
        {
          Serial.println("Node 2 Packet_Found");
          Node2_pklost = false;
          Node2_pklost_counter = 0;
          Serial.println("Node2_Sensor1=" + PacketJsonObject["Node2"]["Sensor1"].as<String>() + ", Node2_Sensor2=" + PacketJsonObject["Node2"]["Sensor2"].as<String>());
        }
        Serial.println("");
        
        //node3
        if(String("Packet_Lost").equals(PacketJsonObject["Node3"].as<String>()))
        {
          Serial.println("Node 3 Packet_Lost");
          Node3_pklost = true;
          Node3_pklost_counter += 1;
          Serial.println((String) "Node 3 Lost Counter = " + Node3_pklost_counter);
        }
        else
        {
          Serial.println("Node 3 Packet_Found");
          Node3_pklost = false;
          Node3_pklost_counter = 0;
          Serial.println("Node3_Sensor1=" + PacketJsonObject["Node3"]["Sensor1"].as<String>() + ", Node3_Sensor2=" + PacketJsonObject["Node3"]["Sensor2"].as<String>());
        }
        Serial.println("");
        
        //node4
        if(String("Packet_Lost").equals(PacketJsonObject["Node4"].as<String>()))
        {
          Serial.println("Node 4 Packet_Lost");
          Node4_pklost = true;
          Node4_pklost_counter += 1;
          Serial.println((String) "Node 4 Lost Counter = " + Node4_pklost_counter);
        }
        else
        {
          Serial.println("Node 4 Packet_Found");
          Node4_pklost = false;
          Node4_pklost_counter = 0;
          Serial.println("Node4_Sensor1=" + PacketJsonObject["Node4"]["Sensor1"].as<String>() + ", Node4_Sensor2=" + PacketJsonObject["Node4"]["Sensor2"].as<String>());
        }
      }
      Serial.println("");

      //Starting to push data to firebase
      if(Node1_pklost == false)
      {
        Node1_1 = CheckThreshold(PacketJsonObject["Node1"]["Sensor1"].as<int>());
        if (Firebase.setInt(fbdo, "/Node1/Sensor1", Node1_1 ))
        {
          Serial.println((String)"Push Node1 Sensor1 : " + PacketJsonObject["Node1"]["Sensor1"].as<int>() + " (" + Node1_1 + ")" + " Successed" );
        }
        else
        {
          Serial.println("Push Node1 Sensor1 FAILED");
          Serial.println("ERROR REASON: " + fbdo.errorReason());
        }

        Node1_2 = CheckThreshold(PacketJsonObject["Node1"]["Sensor2"].as<int>());
        if (Firebase.setInt(fbdo, "/Node1/Sensor2", Node1_2 ))
        {
          Serial.println((String)"Push Node1 Sensor2 : " + PacketJsonObject["Node1"]["Sensor2"].as<int>() + " (" + Node1_2 + ")" + " Successed");
        }
        else
        {
          Serial.println("Push Node1 Sensor2 FAILED");
          Serial.println("ERROR REASON: " + fbdo.errorReason());
        }
        
        if (Firebase.setInt(fbdo, "/Node1/Flag",1))
        {
          Serial.println("Push Node1Flag : 1 Successed");
        }
        else
        {
          Serial.println("Push Node1Flag FAILED");
          Serial.println("ERROR REASON: " + fbdo.errorReason());
        }   
      }
      else if (Node1_pklost == true && Node1_pklost_counter > 20)
      {
        if (Firebase.setInt(fbdo, "/Node1/Flag",0))
        {
          Serial.println("Push Node1Flag : 0 Successed");
        }
        else
        {
          Serial.println("Push Node1Flag FAILED");
          Serial.println("ERROR REASON: " + fbdo.errorReason());
        }
      }

      //Node2
      if(Node2_pklost == false)
      {
        Node2_1 = CheckThreshold(PacketJsonObject["Node2"]["Sensor1"].as<int>());
        if (Firebase.setInt(fbdo, "/Node2/Sensor1", Node2_1 ))
        {
          Serial.println((String)"Push Node2 Sensor1 : " + PacketJsonObject["Node2"]["Sensor1"].as<int>() + " (" + Node2_1 + ")" + " Successed");
        }
        else
        {
          Serial.println("Push Node2 Sensor1 FAILED");
          Serial.println("ERROR REASON: " + fbdo.errorReason());
        }

        Node2_2 = CheckThreshold(PacketJsonObject["Node2"]["Sensor2"].as<int>());
        if (Firebase.setInt(fbdo, "/Node2/Sensor2", Node2_2))
        {
          Serial.println((String)"Push Node2 Sensor2 : " + PacketJsonObject["Node2"]["Sensor2"].as<int>() + " (" + Node2_2 + ")"  + " Successed");
        }
        else
        {
          Serial.println("Push Node2 Sensor2 FAILED");
          Serial.println("ERROR REASON: " + fbdo.errorReason());
        }
        
        if (Firebase.setInt(fbdo, "/Node2/Flag",1))
        {
          Serial.println("Push Node2Flag : 1 Successed");
        }
        else
        {
          Serial.println("Push Node2FlagFAILED");
          Serial.println("ERROR REASON: " + fbdo.errorReason());
        }   
      }
      else if (Node2_pklost == true && Node2_pklost_counter > 20)
      {
        if (Firebase.setInt(fbdo, "/Node2/Flag",0))
        {
          Serial.println("Push Node2Flag : 0 Successed");
        }
        else
        {
          Serial.println("Push Node2FlagFAILED");
          Serial.println("ERROR REASON: " + fbdo.errorReason());
        }
      }

      //Node3
      if(Node3_pklost == false)
      {
        Node3_1 = CheckThreshold(PacketJsonObject["Node3"]["Sensor1"].as<int>());
        if (Firebase.setInt(fbdo, "/Node3/Sensor1", Node3_1 ))
        {
          Serial.println((String)"Push Node3 Sensor1 : " + PacketJsonObject["Node3"]["Sensor1"].as<int>() + " (" + Node3_1 + ")" + " Successed");
        }
        else
        {
          Serial.println("Push Node3 Sensor1 FAILED");
          Serial.println("ERROR REASON: " + fbdo.errorReason());
        }

        Node3_2 = CheckThreshold(PacketJsonObject["Node3"]["Sensor2"].as<int>());
        if (Firebase.setInt(fbdo, "/Node3/Sensor2", Node3_2))
        {
          Serial.println((String)"Push Node3 Sensor2 : " + PacketJsonObject["Node3"]["Sensor2"].as<int>() + " (" + Node3_2 + ")"  + " Successed");
        }
        else
        {
          Serial.println("Push Node3 Sensor2 FAILED");
          Serial.println("ERROR REASON: " + fbdo.errorReason());
        }
        
        if (Firebase.setInt(fbdo, "/Node3/Flag",1))
        {
          Serial.println("Push Node3Flag : 1 Successed");
        }
        else
        {
          Serial.println("Push Node3FlagFAILED");
          Serial.println("ERROR REASON: " + fbdo.errorReason());
        }   
      }
      else if (Node3_pklost == true && Node3_pklost_counter > 20)
      {
        if (Firebase.setInt(fbdo, "/Node3/Flag",0))
        {
          Serial.println("Push Node3Flag : 0 Successed");
        }
        else
        {
          Serial.println("Push Node3FlagFAILED");
          Serial.println("ERROR REASON: " + fbdo.errorReason());
        }
      }

      //Node4
      if(Node4_pklost == false)
      {
        Node4_1 = CheckThreshold(PacketJsonObject["Node4"]["Sensor1"].as<int>());
        if (Firebase.setInt(fbdo, "/Node4/Sensor1", Node4_1 ))
        {
          Serial.println((String)"Push Node4 Sensor1 : " + PacketJsonObject["Node4"]["Sensor1"].as<int>() + " (" + Node4_1 + ")" + " Successed");
        }
        else
        {
          Serial.println("Push Node4 Sensor1 FAILED");
          Serial.println("ERROR REASON: " + fbdo.errorReason());
        }

        Node4_2 = CheckThreshold(PacketJsonObject["Node4"]["Sensor2"].as<int>());
        if (Firebase.setInt(fbdo, "/Node4/Sensor2", Node4_2))
        {
          Serial.println((String)"Push Node4 Sensor2 : " + PacketJsonObject["Node4"]["Sensor2"].as<int>() + " (" + Node4_2 + ")"  + " Successed");
        }
        else
        {
          Serial.println("Push Node4 Sensor2 FAILED");
          Serial.println("ERROR REASON: " + fbdo.errorReason());
        }
        
        if (Firebase.setInt(fbdo, "/Node4/Flag",1))
        {
          Serial.println("Push Node4Flag : 1 Successed");
        }
        else
        {
          Serial.println("Push Node4FlagFAILED");
          Serial.println("ERROR REASON: " + fbdo.errorReason());
        }   
      }
      else if (Node4_pklost == true && Node4_pklost_counter > 20)
      {
        if (Firebase.setInt(fbdo, "/Node4/Flag",0))
        {
          Serial.println("Push Node4Flag : 0 Successed");
        }
        else
        {
          Serial.println("Push Node4FlagFAILED");
          Serial.println("ERROR REASON: " + fbdo.errorReason());
        }
      }
    }
    tmp_string ="";
  }
}

int CheckThreshold(int Sensor_Value)
{
  if(Sensor_Value>=20 && Sensor_Value<=100)
  {
     return 1;
  }
  else
  {
    return 0;
  }
}
