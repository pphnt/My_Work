#include "painlessMesh.h"
#include <SPI.h>
#include <LoRa.h>

#define   MESH_PREFIX     "SmartParkingWithMobileApp"
#define   MESH_PASSWORD   "Peach_isVeryHandSome555"
#define   MESH_PORT       5555

#define SS      18
#define RST     14
#define DI0     26
#define BAND    915E6

String Node1_1, Node1_2, Node2_1, Node2_2, Node3_1, Node3_2, Node4_1 ,Node4_2;
bool Node1_rcv = false, Node2_rcv = false, Node3_rcv = false, Node4_rcv = false;
int TaskSelector = 0;
Scheduler userScheduler;
painlessMesh  mesh;

// Received from Client Node
void receivedCallback( uint32_t from, String &msg );

Task LogServerAndLoRa(7500, TASK_FOREVER, []() 
{
    if(TaskSelector%2 == 0)
    {
      //LoggingServer
      DynamicJsonDocument jsonBuffer(1024);
      JsonObject msg = jsonBuffer.to<JsonObject>();
      msg["topic"] = "logServer";
      msg["nodeId"] = mesh.getNodeId();
      String str;
      serializeJson(msg, str);
      mesh.sendBroadcast(str);
      serializeJson(msg, Serial);
      Serial.printf("\n");
    }
    else if(TaskSelector%2 == 1)
    {
      //LoRa Sending
      DynamicJsonDocument jsonBuffer_2(1024);
      JsonObject lora_msg = jsonBuffer_2.to<JsonObject>();
      lora_msg["topic"] = "log_server";
      
      //check if Packet from client1 lost
      if(Node1_rcv == true)
      {
        JsonObject nested_1 = lora_msg.createNestedObject("Node1");
        nested_1["Sensor1"] = Node1_1;
        nested_1["Sensor2"] = Node1_2;
        Node1_rcv = false;
      }
      else
      {
        lora_msg["Node1"] = "Packet_Lost";
      }
  
      //check if Packet from client2 lost
      if(Node2_rcv == true)
      {
        JsonObject nested_2 = lora_msg.createNestedObject("Node2");
        nested_2["Sensor1"] = Node2_1;
        nested_2["Sensor2"] = Node2_2;
        Node2_rcv = false;
      }
      else
      {
        lora_msg["Node2"] = "Packet_Lost";
      }

      //check if Packet from client3 lost
      if(Node3_rcv == true)
      {
        JsonObject nested_3 = lora_msg.createNestedObject("Node3");
        nested_3["Sensor1"] = Node3_1;
        nested_3["Sensor2"] = Node3_2;
        Node3_rcv = false;
      }
      else
      {
        lora_msg["Node3"] = "Packet_Lost";
      }

      //check if Packet from client4 lost
      if(Node4_rcv == true)
      {
        JsonObject nested_4 = lora_msg.createNestedObject("Node4");
        nested_4["Sensor1"] = Node4_1;
        nested_4["Sensor2"] = Node4_2;
        Node4_rcv = false;
      }
      else
      {
        lora_msg["Node4"] = "Packet_Lost";
      }
  
      String str2;
      serializeJson(lora_msg, str2);
      serializeJson(lora_msg, Serial);
      Serial.printf("\n");
      
      LoRa.beginPacket();
      LoRa.print(str2);
      LoRa.endPacket();  
    } 
    TaskSelector++;
});

void setup() {
  Serial.begin(115200);
  mesh.setDebugMsgTypes( ERROR | CONNECTION | S_TIME );  // set before init() so that you can see startup messages

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT, WIFI_AP_STA, 6 );
  mesh.onReceive(&receivedCallback);

  mesh.onNewConnection([](size_t nodeId) {
    Serial.printf("New Connection %u\n", nodeId);
  });

  mesh.onDroppedConnection([](size_t nodeId) {
    Serial.printf("Dropped Connection %u\n", nodeId);
  });

  SPI.begin(5, 19, 27, 18);
  LoRa.setPins(SS, RST, DI0);
  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("LoRa Initial OK!");

  // Add the task to the your scheduler
  userScheduler.addTask(LogServerAndLoRa);
  LogServerAndLoRa.enable();
}

void loop() {
  // it will run the user scheduler as well
  mesh.update();
}
void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("logServer: Received from %u ", from);
  DynamicJsonDocument JsonParse(1024 + msg.length());
  DeserializationError error = deserializeJson(JsonParse, msg);
  if (error) {
    Serial.printf("DeserializationError\n");
    return;
  }
  serializeJson(JsonParse, Serial);
  Serial.print("\n");
  if(JsonParse.containsKey("topic"))
  {
    if (String("Node1").equals(JsonParse["topic"].as<String>()))
    {
      Serial.println("Found the node 1 : ");
      Node1_1 = JsonParse["value"]["sensor1"].as<String>();
      Node1_2 = JsonParse["value"]["sensor2"].as<String>();
      Serial.println("Node1_1 = " + Node1_1 + " cm");
      Serial.println("Node1_2 = " + Node1_2 + " cm");
      Node1_rcv = true;
    }
    else if (String("Node2").equals(JsonParse["topic"].as<String>()))
    {
      Serial.println("Found the node 2");
      Node2_1 = JsonParse["value"]["sensor1"].as<String>();
      Node2_2 = JsonParse["value"]["sensor2"].as<String>();
      Serial.println("Node2_1 = " + Node2_1 + " cm");
      Serial.println("Node2_2 = " + Node2_2 + " cm");
      Node2_rcv = true;
    }
    else if (String("Node3").equals(JsonParse["topic"].as<String>()))
    {
      Serial.println("Found the node 3");
      Node3_1 = JsonParse["value"]["sensor1"].as<String>();
      Node3_2 = JsonParse["value"]["sensor2"].as<String>();
      Serial.println("Node3_1 = " + Node3_1 + " cm");
      Serial.println("Node3_2 = " + Node3_2 + " cm");
      Node3_rcv = true;
    }
    else if (String("Node4").equals(JsonParse["topic"].as<String>()))
    {
      Serial.println("Found the node 4");
      Node4_1 = JsonParse["value"]["sensor1"].as<String>();
      Node4_2 = JsonParse["value"]["sensor2"].as<String>();
      Serial.println("Node4_1 = " + Node4_1 + " cm");
      Serial.println("Node4_2 = " + Node4_2 + " cm");
      Node4_rcv = true;
    }
  }
}
