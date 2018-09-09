//vasanth.l  ece Rajalakshmi Engineering College
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <SoftwareSerial.h>

#define DEBUG true
 
//SSID and Password of your WiFi router
const char* ssid = "";
const char* password = "";
int smoke_sensor_pin = A0;
 
ESP8266WebServer server(80); 
 
SoftwareSerial wifi_module(2,3);

void setup(){
  Serial.begin(115200);
  
  WiFi.begin(ssid, password);     //Connect to your WiFi router
  Serial.println("");
 
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
      pinMode(smoke_sensor_pin, INPUT);

  esp8266_command("AT+RST\r\n",2000,DEBUG); // reset module

  esp8266_command("AT+CWMODE=2\r\n",1000,DEBUG); // configure as access point

  esp8266_command("AT+CIFSR\r\n",1000,DEBUG); // get ip address

  esp8266_command("AT+CIPMUX=1\r\n",1000,DEBUG); // configure for multiple connections

  esp8266_command("AT+CIPSERVER=1,80\r\n",1000,DEBUG); // turn on server on port 80

  }
 
  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP
 
      //Which routine to handle at root location
 
  server.begin();                  //Start server
  Serial.println("HTTP server started");
}

void loop(){
  server.handleClient();   //Handle client requests
   int analogSensor = analogRead(smoke_sensor_pin);

  if(wifi_module.available()) 

  {

    if(wifi_module.find("+IPD,"))

    {

     delay(1000);

     int connectionId = wifi_module.read()-48;     

     String webpage = "<h1>IOT Smoke Detection System</h1>";

      webpage +="<p>Smoke Value is ";

      webpage += analogSensor;

      webpage +="</p>";

      if (analogSensor > 400)

  {

    webpage +="<h5>DANGER! Move Somewhere Else</h5>";

  }

  else

  {

    webpage +="<h4>Everything Normal</h4>";

  }

     String cipSend = "AT+CIPSEND=";

     cipSend += connectionId;

     cipSend += ",";

     cipSend +=webpage.length();

     cipSend +="\r\n";

     

     esp8266_command(cipSend,1000,DEBUG);

     esp8266_command(webpage,1000,DEBUG);

     

     String closeCommand = "AT+CIPCLOSE="; 

     closeCommand+=connectionId; // append connection id

     closeCommand+="\r\n";

     

     esp8266_command(closeCommand,3000,DEBUG);

    }

  }

}

String esp8266_command(String command, const int timeout, boolean debug)

{

    String response = "";

    wifi_module.print(command); 

    long int time = millis();

    while( (time+timeout) > millis())

    {

      while(wifi_module.available())

      {

        

        char c = wifi_module.read(); 

        response+=c;

      }  

    }

    

    if(debug)

    {

      Serial.print(response);

    }

    return response;

}
