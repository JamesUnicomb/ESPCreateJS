#include <Create2.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <ESP8266WiFi.h>

//Set Wifi ssid and password
char ssid[] = "wifiname";
char pass[] = "wifipassword";

ESP8266WebServer server (80);

// Creates a Create2 instance to communicate over Serial1 at a baud rate of 19200
Create2 create(&Serial1, Create2::Baud19200);
const int BAUD_PIN = D1; // The pin attached to the Baud Rate Change pin on the DIN connector

//This function takes the parameters passed in the URL(the x and y coordinates of the joystick)
//and sets the motor speed based on those parameters. 
void handleJSData(){
  boolean yDir;
  int x = server.arg(0).toInt() * 10;
  int y = server.arg(1).toInt() * 10;

  int leftSpeed  = y - x/2;
  int rightSpeed = y + x/2;

  create.driveDirect(leftSpeed, rightSpeed);
  
  server.send(200, "text/plain", "");   
}

void setup()
{
  create.setBaudDefault(BAUD_PIN); // Sets baud rate of Create 2 to be 19200
  
  create.start(); // Starts the Create's OI
  create.fullMode(); // Sets the mode to be Full Mode
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  
  // Debug console
  Serial.begin(115200);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  
  //initialize SPIFFS to be able to serve up the static HTML files. 
  if (!SPIFFS.begin()){
    Serial.println("SPIFFS Mount failed");
  } 
  else {
    Serial.println("SPIFFS Mount succesfull");
  }
  //set the static pages on SPIFFS for the html and js
  server.serveStatic("/", SPIFFS, "/joystick.html"); 
  server.serveStatic("/virtualjoystick.js", SPIFFS, "/virtualjoystick.js");
  //call handleJSData function when this URL is accessed by the js in the html file
  server.on("/jsData.html", handleJSData);  
  server.begin();
}

void loop()
{
  server.handleClient();  
}
