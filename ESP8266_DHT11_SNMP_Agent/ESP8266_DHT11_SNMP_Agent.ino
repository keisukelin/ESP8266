/*
 * ESP8266 with DHT11 as SNMP agent
 */

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Arduino_SNMP.h>
#include <SimpleDHT.h>

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

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    
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
    changingNumber++;
    //Serial.println(changingNumber);
}
