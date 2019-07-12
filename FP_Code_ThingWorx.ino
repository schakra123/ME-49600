#include <WiFi.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>

Servo servo1;// create servo object to control a servo
Servo servo2;
Servo servo3;

// Replace the next variables with your SSID/Password combination
const char* ssid = "NETGEAR59";
const char* password = "luckybug189";
// Add your MQTT Broker IP address, example:
const char* mqtt_server = "m16.cloudmqtt.com";
#define mqtt_user "pdbimqpo" // username
#define mqtt_password "H7TVMiue_bh3" // password
WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
char msg[50];
int value = 0;
// LED Pin
const int ledPin = 27;

// twelve servo objects can be created on most boards
int pos1 = 0;    // variable to store the servo position
int pos2 = 0;
int pos3 = 0;
int power = 0;
const int senstrig = 2;
const int sensecho = 5;
long duration;
int distance;
const int servopin1 = 13;
const int servopin2 = 12;
const int servopin3 = 14;
const int mag1 = 19;
const int mag2 = 21;
const int button = 34;
int RUN;

void setup() {
Serial.begin(9600);
setup_wifi();
client.setServer(mqtt_server, 17375);
client.setCallback(callback);
pinMode(ledPin, OUTPUT);

// Ultrasonic Distance sensor setup
pinMode(senstrig,OUTPUT);
pinMode(sensecho, INPUT);
Serial.begin(9600);
  
// attaches the servo on pin 9 to the servo object
servo1.attach(servopin1);  
servo2.attach(servopin2);
servo3.attach(servopin3);

// Electromagnet setup
pinMode(mag1, OUTPUT);
pinMode(mag2, OUTPUT);
pinMode(button, INPUT);
}

void loop() {
if (!client.connected()) {
reconnect();
}
client.loop();
long now = millis();
if (now - lastMsg > 5000) {
lastMsg = now;
}

digitalWrite(senstrig, LOW);
delayMicroseconds(2);

digitalWrite(senstrig, HIGH);
delayMicroseconds(10);

digitalWrite(senstrig, LOW);

if (RUN == 1) {
// Recording duration of pulse and finding distance travelled
duration = pulseIn(sensecho, HIGH);
distance = duration * 0.034 /2 ;

Serial.print("Distance: ");
Serial.println(distance);
if (distance < 10) {
  // Resetting all 3 motors and picking up the object
delay(5000); //Initial position and pick up 1
forward1(0,50,1); //left
forward2(0,50,1); // down (limit between 0 and 70)
forward3(0,45,1); //for this servo forward means up 
magnet(1);
//Dropping object 
delay(500); 
backward2(50,0,1);
forward1(50,140,1);
forward3(0,20,1);
forward2(0,65,1);
delay(500);
//Resetting positiong
magnet(0);
delay(1000);
backward2(65,0,1);
backward3(20,0,1);

  
}
}
}

//Function to switch Magnet on and off
void magnet(int power){
  if (power == 1){
    digitalWrite(mag1,HIGH); 
    digitalWrite(mag2, LOW); 
  }
  if (power == 0) {
    digitalWrite(mag1,LOW) ; 
    digitalWrite(mag2, LOW);
    }
    }
// Control Servo 1 forward motion
void forward1(int pos1f, int Max1, int Step1f) {
    for (pos1 = pos1f; pos1 <= Max1; pos1 += Step1f) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    servo1.write(pos1);              // tell servo to go to position in variable 'pos'
    delay(20);                       // waits 15ms for the servo to reach the position
  }
  }
//Control Servo 1 backward motion
void backward1(int pos1b, int Min1, int Step1b) {
  for (pos1 = pos1b; pos1 >= Min1; pos1 -= Step1b) { // goes from 180 degrees to 0 degrees
    servo1.write(pos1);              // tell servo to go to position in variable 'pos'
    delay(20);
}

//Control Servo 2 forward motion
}
void forward2(int pos2f, int Max2, int Step2f) {
    for (pos2 = pos2f; pos2 <= Max2; pos2 += Step2f) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    servo2.write(pos2);              // tell servo to go to position in variable 'pos'
    delay(20);                       // waits 15ms for the servo to reach the position
  }
  }

//Control Servo 2 backward motion
void backward2(int pos2b, int Min2, int Step2b) {
  for (pos2 = pos2b; pos2 >= Min2; pos2 -= Step2b) { // goes from 180 degrees to 0 degrees
    servo2.write(pos2);              // tell servo to go to position in variable 'pos'
    delay(20);
}
}

//Control Servo 3 forward motion
void forward3(int pos3f, int Max3, int Step3f){
  for(pos3 = pos3f; pos3<= Max3; pos3 += Step3f){
    servo3.write(pos3);
    delay(20);
  }
}

//Control Servo 3 backward motion
void backward3(int pos3b, int Min3, int Step3b){
  for (pos3 = pos3b; pos3 >= Min3; pos3 -= Step3b){
    servo3.write(pos3);
    delay(20);
  }
}  

void setup_wifi() {
 delay(10);
 // We start by connecting to a WiFi network
 Serial.println();
 Serial.print("Connecting to ");
 Serial.println(ssid);
 WiFi.begin(ssid, password);
 while (WiFi.status() != WL_CONNECTED) {
 delay(500);
 Serial.print(".");
 }
 Serial.println("");
 Serial.println("WiFi connected");
 Serial.println("IP address: ");
 Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* message, unsigned int length) {
 Serial.print("Message arrived on topic: ");
 Serial.print(topic);
 Serial.print(". Message: ");
 String messageTemp;
 for (int i = 0; i < length; i++) {
 Serial.print((char)message[i]);
 messageTemp += (char)message[i];
 }
 Serial.println();
 // Feel free to add more if statements to control more GPIOs with MQTT
 // If a message is received on the topic esp32/output, you check if the message is either "on" or
// "off".
 // Changes the output state according to the message
 if (String(topic) == "/Thingworx/DHT11Thing/output") {
 Serial.print("Changing output to ");
 if (messageTemp == "on") {
 Serial.println("on");
 digitalWrite(ledPin, HIGH);
 RUN = 0;
 }
 else if (messageTemp == "off") {
 Serial.println("off");
 digitalWrite(ledPin, LOW);
 RUN = 1;
 }
 }
}

void reconnect() {
 // Loop until we're reconnected
 while (!client.connected()) {
 Serial.print("Attempting MQTT connection...");
 // Attempt to connect
 if (client.connect("ESP32Client", mqtt_user, mqtt_password)) {
 Serial.println("connected");
 // Subscribe
 client.subscribe("/Thingworx/DHT11Thing/output");
 } else {
 Serial.print("failed, rc=");
 Serial.print(client.state());
 Serial.println(" try again in 5 seconds");
 // Wait 5 seconds before retrying
 delay(5000);
 }
 }
}
