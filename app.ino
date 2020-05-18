/*
 WiFiEsp example: WebClient

 This sketch connects to google website using an ESP8266 module to
 perform a simple web search.

 For more details see: http://yaab-arduino.blogspot.com/p/wifiesp-example-client.html
*/

#include "WiFiEsp.h"
#include "PubSubClient.h"
#include "SPI.h"
// Emulate Serial1 on pins 6/7 if not present
#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(6, 7); // RX, TX
#endif

char ssid[] = "androidAP";            // your network SSID (name)
char pass[] = "12345678";        // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status

//char server[] = "arduino.cc";
IPAddress server(192,168,43,11);
// Initialize the Ethernet client object
WiFiEspClient client;
PubSubClient pubClient(client);
int temp = 0;
void setup()
{
  // initialize serial for debugging
  Serial.begin(115200);
  // initialize serial for ESP module
  Serial3.begin(115200);
  // initialize ESP module
  WiFi.init(&Serial3);

  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }
WiFi.scanNetworks();
  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
  }

  // you're connected now, so print out the data
  Serial.println("You're connected to the network");

  
 // printWifiStatus();

  Serial.println();
  //Connect to MQTT Server
  pubClient.setServer(server,1883);
  pubClient.setCallback(callback);
  /*
  Serial.println("Starting connection to server...");
  // if you get a connection, report back via serial
  if (client.connect(server, 80)) {
    Serial.println("Connected to server");
    // Make a HTTP request
    client.println("GET /asciilogo.txt HTTP/1.1");
    client.println("Host: arduino.cc");
    client.println("Connection: close");
    client.println();
  }
  */
}
void callback(char* topic, byte* playload,unsigned int length){
  Serial.print("Message Arrived [");
  Serial.print(topic);
  Serial.print("]");
  for(int i=0;i<length;i++){
    char reciverChar = (char)playload[i];
    Serial.print(reciverChar);
    if(reciverChar == '0'){
      Serial.println("OFF");
    }
    if(reciverChar == '1'){
      Serial.println("ON");
    }
  }
   Serial.println();
}
void loop()
{
  if(!pubClient.connected()){
    reconnectCon();
  }
  else{
    
  delay(5000);
      String playload= "{";
    playload+= "\"temp\":";
    playload+= temp;
    playload+=",";
    playload+="\"hum\":";
    playload+="66";
    playload+="}";
    char attributter[100];
    playload.toCharArray(attributter,100);
    pubClient.publish("v1/devices/me/attributes",attributter);
temp++;
  }
  pubClient.loop();
  Serial.println("Publishing Data...");

  /*
  // if there are incoming bytes available
  // from the server, read them and print them
  while (client.available()) {
    char c = client.read();
    Serial.write(c);
  }

  // if the server's disconnected, stop the client
  if (!client.connected()) {
    Serial.println();
    Serial.println("Disconnecting from server...");
    client.stop();

    // do nothing forevermore
    while (true);
  }
  */
  
}
void reconnectCon(){
  while(!pubClient.connected()){
    Serial.print("Attemping Conection...");
    if(pubClient.connect("5ebcb16c6d41df335bbd7446","3wSizw75ZNEYfmF4mRW7",NULL)){
        Serial.print("CONNECTED");
    //Once connected, publish an announcement
    String playload= "{";
    playload+= "\"temp\":";
    playload+= temp;
    playload+=",";
    playload+="\"hum\":";
    playload+="66";
    playload+="}";
    char attributter[100];
    playload.toCharArray(attributter,100);
    pubClient.publish("v1/devices/me/attributes",attributter);
temp++;
    //add Subscribe
    pubClient.subscribe("v1/devices/me/attributes/control");

    }
    else{
       Serial.println("FAILED");
       Serial.print(pubClient.state());
       //Try again in 5 seconds
       delay(5000);
    }

  }
}

void printWifiStatus()
{
  // print the SSID of the network you're attached to
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength
  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
