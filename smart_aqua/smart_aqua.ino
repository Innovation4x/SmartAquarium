#define sensorPin 26
#include <Servo.h>
#include <SPI.h>
#include <Wire.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <Servo.h>
int val2 = 0;
int servo=0;
int pos;

Servo myservo;  // create servo object to control a servo
//MQTT Global Variables
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
IPAddress ip(172, 16, 0, 100);
IPAddress myDns(192, 168, 0, 1);
IPAddress server(10, 5, 15, 103);
EthernetClient ethClient;
PubSubClient client(ethClient);

//MQTT callback funtion
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  char messageBuffer[30];
  memcpy(messageBuffer, payload, length);
  messageBuffer[length] = '\0';
  Serial.println(messageBuffer);
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  //For convering payload to string
  char message[30];
  for(int i=0;i<length;i++){
    message[i]=payload[i];
    }
  String msg = String(message);
  if((msg.indexOf("feed")>-1)||(msg.indexOf("Feed")>-1))
  servo=1;
  else
  servo=0;
}

//MQTT Reconnect callback
void reconnect() {
  while (!client.connected()) {
  // Loop until we're reconnected
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("arduinoClient")) {
      Serial.println("connected");
      client.subscribe("home/aqua");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void ethstart()
{
  //ethernet codes
  Ethernet.init(17);  // WIZnet W5100S-EVB-Pico
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

    Serial.println("Initialize Ethernet with DHCP:");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
      while (true) {
        delay(1); // do nothing, no point running without Ethernet hardware
      }
    }
    if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
    }
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip, myDns);
  } else {
    Serial.print("  DHCP assigned IP ");
    Serial.println(Ethernet.localIP());
  }
  // give the Ethernet shield a second to initialize:
  delay(1000);
  Serial.print("connecting to ");
  Serial.print(server);
  Serial.println("...");
  client.setClient(ethClient);
  client.setServer(server, 1883);
  client.setCallback(callback);
  // Allow the hardware to sort itself out
  delay(1500);
 
}

void setup() {
  // put your setup code here, to run once:
  ethstart();
  myservo.attach(2);
  pinMode(15,OUTPUT);//pump
 pinMode(sensorPin, INPUT);
//  pinMode(9,OUTPUT);

  pinMode(12,OUTPUT); //red
  pinMode(13,OUTPUT); //buzzer
   myservo.attach(2);
 Serial.begin(9600);
}

int start_servo()
  {
    if(servo==1)
  {
    for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);
    delay(2);// tell servo to go to position in variable 'pos'
                          // waits 15ms for the servo to reach the position
  }
  for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);
    delay(2);// tell servo to go to position in variable 'pos'
                           // waits 15ms for the servo to reach the position
  }
  }
  servo=0;
  return 0;
  }


void loop() {
  int pumpstatus=0;
  if (!client.connected()) {
    reconnect();
    }
    //Subscribe to topic
   client.subscribe("home");  
 
    start_servo();
 
   val2 = analogRead(sensorPin);
  Serial.print("Water level: ");
  Serial.println(val2);
   if((val2>=400)){
    digitalWrite(12,HIGH); //green
    digitalWrite(13,LOW); //buzzer
    delay(100);
    pumpstatus=1;
    }
  else{
    pumpstatus=0;
    digitalWrite(12,LOW);
    digitalWrite(13,HIGH);
    delay(100);
    }
  if(pumpstatus==0)
  {
    digitalWrite(15,HIGH);
    delay(5000);
    digitalWrite(15,LOW);
    }
    else
    digitalWrite(15,LOW);

    client.loop();
}
