/*
 * ESP8266 with DHT11 as SNMP agent
 */

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Arduino_SNMP.h>
#include <SimpleDHT.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3F, 16, 2);

const char* ssid = "SSID";
const char* password = "PASSWORD";

int pinSHOW = 14;  //GPIO14(D5)
int showIP = 0;
int pinDHT11 = 2;  //GPIO2(D1)
SimpleDHT11 dht11;

int ledPin = LED_BUILTIN;
WiFiServer server(80);

WiFiUDP udp;
SNMPAgent snmp = SNMPAgent("public");  // Starts an SMMPAgent instance with the community string 'public'

int changingNumber = 1;
int settableNumber1 = 0;
int settableNumber2 = 0;
int settableNumber3 = 0;

void setup(){
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    Serial.println("");

    pinMode(showIP,INPUT);
    
    Wire.begin(2, 0);
    lcd.init(); // 初始化LCD
    lcd.backlight(); // 启动或打开背光

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        lcd.setCursor(0,0);
        lcd.print("Wait connect to SSID: "); // 在屏幕显示SSID
        lcd.setCursor(0,1);
        lcd.print(ssid);
    }
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("SSID: "); // 在屏幕显示SSID
    lcd.setCursor(6,0);
    lcd.print(ssid);
    lcd.setCursor(0,1);
    lcd.print(WiFi.localIP());

    // Start the server
    server.begin();
    Serial.println("Server started");
 
    // Print the IP address
    Serial.print("Use this URL to connect: ");
    Serial.print("http://");
    Serial.print(WiFi.localIP());
    Serial.println("/");
    
    // give snmp a pointer to the UDP object
    snmp.setUDP(&udp);
    snmp.begin();
    
    // add 'callback' for an OID - pointer to an integer
    snmp.addIntegerHandler(".1.3.6.1.4.1.5.0", &changingNumber);
    
    // Using your favourite snmp tool:
    // snmpget -v 1 -c public <IP> 1.3.6.1.4.1.5.0 
    
    // you can accept SET commands with a pointer to an integer (or string)
    snmp.addIntegerHandler(".1.3.6.1.4.1.2566.10.2.2.1.20.0.2", &settableNumber1, true);
    snmp.addIntegerHandler(".1.3.6.1.4.1.2566.10.2.2.1.20.0.9", &settableNumber2, true);
    snmp.addIntegerHandler(".1.3.6.1.4.1.2566.10.2.2.1.20.0.10", &settableNumber3, true);
    //.1.3.6.1.4.1.2566.10.2.2.1.20.0.2
    //.1.3.6.1.4.1.2566.10.2.2.1.20.0.9
    // snmpset -v 1 -c public <IP> 1.3.6.1.4.1.5.0 i 99
    delay(3000);
}

void loop(){
  byte temperature = 0;
  byte humidity = 0;
  int err = SimpleDHTErrSuccess;

  if ((err = dht11.read(pinDHT11, &temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
      delay(500);
      Serial.print("Read DHT11 failed, err="); Serial.println(err);
      return;
   }
        
   settableNumber1 = (int)temperature;
   settableNumber2 = (int)humidity;
   Serial.print("Humidity = ");   
   Serial.print((int)humidity);   
   Serial.print("% , ");   
   Serial.print("temperature = ");   
   Serial.print((int)temperature);   
   Serial.println("C ");
   lcd.setCursor(0,0);
   lcd.print("Temperature "); // 在屏幕显示SSID
   lcd.setCursor(12,0);
   lcd.print((int)temperature);
   lcd.setCursor(14,0);
   lcd.print(" C");
   lcd.setCursor(0,1);
   lcd.print("Humidity    "); // 在屏幕显示IP
   lcd.setCursor(12,1);
   lcd.print((int)humidity);
   lcd.setCursor(14,1);
   lcd.print(" %");
   delay(1000);
   
   snmp.loop(); // must be called as often as possible
    if(snmp.setOccurred){
          
        if (settableNumber1!=0) {
           Serial.printf("Number has been set to value: %i", settableNumber1);
           settableNumber1=0 ; 
           }
        if (settableNumber2!=0) {
           Serial.printf("Number has been set to value: %i", settableNumber2); 
           settableNumber2=0 ; 
           }
        snmp.resetSetOccurred();
    }
    /*else{
   
      // Check if a client has connected
      WiFiClient client = server.available();
      if (!client) {
         delay(1000);
         return;
      }else{
        // Wait until the client sends some data
        Serial.println("new client");
        while(!client.available()){
          delay(1);
        }
 
        // Read the first line of the request
        String request = client.readStringUntil('\r');
        Serial.println(request);
        client.flush();
 
        // Return the response
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println(""); //  do not forget this one
        client.println("<!DOCTYPE HTML>");
        client.println("<html>");
 
        client.print("Humidity = ");   
        client.print((int)humidity);   
        client.print("% , ");   
        client.print("temperature = ");   
        client.print((int)temperature);   
        client.println("C ");
        client.println("</html>");  
        delay(1);
        Serial.println("Client disonnected");
        Serial.println("");
        delay(1000);
      }
   }*/
   /*if(digitalRead(showIP)!= HIGH){*/
   if(0){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("SSID: "); // 在屏幕显示SSID
    lcd.setCursor(6,0);
    lcd.print(ssid);
    lcd.setCursor(0,1);
    lcd.print(WiFi.localIP());
   }
}
