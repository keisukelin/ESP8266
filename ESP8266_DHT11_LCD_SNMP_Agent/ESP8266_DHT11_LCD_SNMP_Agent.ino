/*
 * ESP8266 with DHT11 as SNMP agent
 * DHT11 pin D1
 * LCD pin D3,D4
 */

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Arduino_SNMP.h>
#include <SimpleDHT.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3F, 16, 2);

const char* ssid = "SSID";
const char* password = "PASSWORD";

int pinDHT11 = 5;  //GPIO5(D1)
SimpleDHT11 dht11;

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
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Temperature "); // 在屏幕显示SSID
    lcd.setCursor(14,0);
    lcd.print(" C");
    lcd.setCursor(0,1);
    lcd.print("Humidity    "); // 在屏幕显示IP
    lcd.setCursor(14,1);
    lcd.print(" %");
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
   lcd.setCursor(12,0);
   lcd.print((int)temperature);
   lcd.setCursor(12,1);
   lcd.print((int)humidity);
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
}
