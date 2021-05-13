//************************************************************
// this is a simple example that uses the painlessMesh library to 
// setup a single node (this node) as a logging node
// The logClient example shows how to configure the other nodes
// to log to this server
//************************************************************
#include "painlessMesh.h"
#include <SPI.h>
#include <LoRa.h>

#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555
#define SS      18
#define RST     14
#define DI0     26
#define BAND    868E6  //915E6


String test; 
Scheduler     userScheduler; // to control your personal task
painlessMesh  mesh;
// Prototype
void receivedCallback( uint32_t from, String &msg );


struct node{
  int sensor1,sensor2,sensor3,sensor4;
}nodes;

// Send my ID every 10 seconds to inform others
Task logServerTask(10000, TASK_FOREVER, []() 
{
    DynamicJsonDocument jsonBuffer(1024);
    JsonObject msg = jsonBuffer.to<JsonObject>();
    msg["topic"] = "logServer";
    msg["nodeId"] = mesh.getNodeId();
    String str;
    serializeJson(msg, str);
    mesh.sendBroadcast(str);
    serializeJson(msg, Serial);
    Serial.printf("\n");
});
//sendlora
Task logLoraTask(15000, TASK_FOREVER, []() 
{

  LoRa.beginPacket();
  LoRa.println("earth<3kam");
  Serial.println("Lora work");
  LoRa.endPacket();

//  Serial.println("task work");
  /*
    DynamicJsonDocument jsonBuffer(1024);
    JsonObject msg = jsonBuffer.to<JsonObject>();
    msg["topic"] = "logServer";
    msg["nodeId"] = mesh.getNodeId();
    String str;
    serializeJson(msg, str);
    mesh.sendBroadcast(str);
    serializeJson(msg, Serial);
    Serial.printf("\n");
*/
});


void setup() {
  Serial.begin(115200);
    
  //mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE | DEBUG ); // all types on
  //mesh.setDebugMsgTypes( ERROR | CONNECTION | SYNC | S_TIME );  // set before init() so that you can see startup messages
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
  userScheduler.addTask(logServerTask);
  userScheduler.addTask(logLoraTask);
  logServerTask.enable();
  logLoraTask.enable();
}

void loop() {
  // it will run the user scheduler as well
  mesh.update();

}

void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("logServer: Received from %u msg=%s\n", from, msg.c_str());
  DynamicJsonDocument JsonParse(1024 + msg.length());
  DeserializationError error = deserializeJson(JsonParse, msg);
  if (error) {
    Serial.printf("DeserializationError\n");
    return;
  }
  serializeJson(JsonParse, Serial);
  Serial.print("\n");
  test = JsonParse["topic"].as<String>();
  Serial.print(test);
  Serial.print("\n");

  if(JsonParse.containsKey("topic"))
  {
    if (String("Node1").equals(JsonParse["topic"].as<String>()))
    {
      nodes.sensor1=JsonParse["value"]["sensor1"].as<int>();
     // Serial.println("\n\n\n\n\n\n\n\n");
      //Serial.println(nodes.sensor1);
      //Serial.println("\n\n\n\n\n\n\n\n");
    }
    if (String("Node2").equals(JsonParse["topic"].as<String>()))
    {
     nodes.sensor2=JsonParse["value"]["sensor2"].as<int>(); 
    }
    if (String("Node3").equals(JsonParse["topic"].as<String>()))
    {
     nodes.sensor3=JsonParse["value"]["sensor3"].as<int>(); 
    }
    if (String("Node4").equals(JsonParse["topic"].as<String>()))
    {
      nodes.sensor4=JsonParse["value"]["sensor4"].as<int>();
    }
  
  }
  /*JsonObject Receive = JsonParse.as<JsonObject>();
  test = Receive["topic"].as<String>();
  Serial.println(test);*/
}
