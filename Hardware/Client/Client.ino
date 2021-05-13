#include "painlessMesh.h"

#define   MESH_PREFIX     "SmartParkingWithMobileApp"
#define   MESH_PASSWORD   "Peach_isVeryHandSome555"
#define   MESH_PORT       5555

#define trigPin1 19
#define echoPin1 18

#define trigPin2 26
#define echoPin2 25

int count=0;
String Sensor1 = "0", Sensor2 = "0";
unsigned long previousMicrosHigh1 = 0, previousMicrosLow1 = 0, previousMicrosHigh2 = 0, previousMicrosLow2 = 0, currentMicros;

Scheduler     userScheduler; // to control your personal task
painlessMesh  mesh;

void receivedCallback( uint32_t from, String &msg );

size_t logServerId = 0;

// Send message to the logServer every 7 seconds 
Task myLoggingTask(7000, TASK_FOREVER, []() 
{
    DynamicJsonDocument jsonBuffer(1024);
    JsonObject msg = jsonBuffer.to<JsonObject>();
    msg["topic"] = "NodeX";
    JsonObject nested = msg.createNestedObject("value");
    nested["sensor1"] = Sensor1;
    nested["sensor2"] = Sensor2;
    String str;
    serializeJson(msg, str);
    if (logServerId == 0)
        mesh.sendBroadcast(str);
    else
        mesh.sendSingle(logServerId, str);
    serializeJson(msg, Serial);
    Serial.printf("\n");
});

void setup() {
  Serial.begin(115200);
  pinMode(trigPin1,OUTPUT);
  pinMode(echoPin1,INPUT);
  digitalWrite(trigPin1,LOW);

  pinMode(trigPin2,OUTPUT);
  pinMode(echoPin2,INPUT);
  digitalWrite(trigPin2,LOW);
  
  mesh.setDebugMsgTypes( ERROR | STARTUP | CONNECTION );  // set before init() so that you can see startup messages

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT, WIFI_AP_STA, 6 );
  mesh.onReceive(&receivedCallback);

  // Add the task to the your scheduler
  userScheduler.addTask(myLoggingTask);
  myLoggingTask.enable();
}

void loop() {
  mesh.update();

  currentMicros = micros();

  if (currentMicros-previousMicrosHigh1 >= 3000000) 
  {
    previousMicrosHigh1 = currentMicros;

    //Sensor type 2
    if(count%2==0){
      digitalWrite(trigPin1,HIGH);
      delayMicroseconds(5);  
      digitalWrite(trigPin1,LOW);
      Sensor1 = String((pulseIn(echoPin1, HIGH))/2/29);
      Serial.print(Sensor1);
      Serial.println("cm1");   
    }
    else if(count%2==1){
      digitalWrite(trigPin2,HIGH);
      delayMicroseconds(5);
      digitalWrite(trigPin2,LOW);
      Sensor2 = String((pulseIn(echoPin2, HIGH))/2/29);
      Serial.print(Sensor2);
      Serial.println("cm2");    
    }
    count++;
  }
}

void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("logClient: Received from %u msg=%s\n", from, msg.c_str());
  
  DynamicJsonDocument jsonBuffer(1024 + msg.length());
  DeserializationError error = deserializeJson(jsonBuffer, msg);
  if (error) {
    Serial.printf("DeserializationError\n");
    return;
  }
  JsonObject root = jsonBuffer.as<JsonObject>();
  if (root.containsKey("topic")) {
      if (String("logServer").equals(root["topic"].as<String>())) {
          // check for on: true or false
          logServerId = root["nodeId"];
          Serial.printf("logServer detected!!!\n");
      }
      Serial.printf("Handled from %u msg=%s\n", from, msg.c_str());
  }
}
