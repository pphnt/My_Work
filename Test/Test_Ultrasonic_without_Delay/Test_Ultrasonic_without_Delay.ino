const int trigPin =19; //set pin variables
const int echoPin = 18;
int duration, distance, trigState = LOW; //state of trigPin
unsigned long previousMicrosHigh = 0, previousMicrosLow = 0, currentMicros;

void setup()
{
  Serial.begin(9600); //begin serial communication
  pinMode(trigPin,OUTPUT); //set pinmodes
  pinMode(echoPin,INPUT);
  digitalWrite(trigPin,LOW);
}

void loop()
{
  currentMicros = micros();
  if (currentMicros-previousMicrosHigh >= 5000000) 
  {
    previousMicrosHigh = currentMicros;
    digitalWrite(trigPin,HIGH);
  }
  if(currentMicros-previousMicrosLow >= 5000050){
    previousMicrosLow = currentMicros-50;
    duration = pulseIn(echoPin,HIGH);
    distance = (duration/2) / 29.1;
    Serial.print(distance);
    Serial.println("cm");   
    digitalWrite(trigPin,LOW);
  }
}
    
