#include <SoftwareSerial.h> 
#include <dht.h>

SoftwareSerial ESP8266(10,11);//RX,TX

#define SSID "Zul"
#define PASS "abcdef1234"

#define IP "184.106.153.149" // thingspeak.com IP address; see their website for more detail
String GET = "GET /update?api_key=IAIUO36JCDJ3X4JJ&field1="; // you channel will have unique code
String field1="1=";
String field2="2=";
String field3="3=";
String field4="4=";

dht DHT;
#define DHT11_PIN 3

int count = 0;
bool lastinstate;
bool lastoutstate;
bool instate;
bool outstate;

int DELAY_LONG_MS = 30000;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); // this is to start serial monitoring with your Arduino IDE Serial monitor.
  ESP8266.begin(9600); // this is to start serial communication with the ESP via Software Serial.
  Serial.println("HASHBEE IOT TEST ");
  Serial.println();
  Serial.println("Humidity (%),\tTemperature (C),\tBee Count");
  ESP8266.println("AT+RST"); // this resets the ESP8266.
  Serial.println("AT+RST"); // this is just echoing the command to your serial monitor so you can
  if(ESP8266.find("ready")){ Serial.println("Ready");}
  delay(2000);
  ESP8266.println("AT");
  Serial.println("AT");
  
  delay(5000);
  if(ESP8266.find("OK")){
      Serial.println("OK");  
      Serial.println("ESP8266 Connected ");
      connectToWiFi();
  }
  else{
    Serial.println("Failed ESP8266 Response");
    setup();
  }


}

void loop() {
  ESP8266.println("AT+CIPSTATUS");
  //if(ESP8266.available()) { Serial.write(ESP8266.read()); }
  //if(ESP8266.find("STATUS:2")){ Serial.println("Wifi OK"); }
  //else if(ESP8266.find("STATUS:3")){ Serial.println("Wifi OK"); }
  //else {
   // Serial.println("Wifi not OK");
  //  connectToWiFi(); }
  
   unsigned long startMillis = millis();
  while((millis() - startMillis) < DELAY_LONG_MS){
  
  int sensorin = analogRead(A0);
  int sensorout = analogRead(A1);
  //Serial.print(sensorout);
  //Serial.print("   ");
  //Serial.print(sensorin);

  if(sensorin > 50) instate=true; else instate=false;
  if(sensorout > 50) outstate=true; else outstate=false;

  if(lastinstate==!instate) count++;
  if(lastoutstate==!outstate) count--;

  lastinstate = instate;
  lastoutstate = outstate;
  //Serial.print("   ");
  //Serial.println(count);

  }
  int chk = DHT.read11(DHT11_PIN);
  Serial.print(DHT.humidity);
  Serial.print(",\t");
  Serial.print(DHT.temperature);
  Serial.print(",\t");
  Serial.println(count);
  sendTelemetry(DHT.temperature,DHT.humidity,count);
}

boolean connectToWiFi(){

  //ESP8266.println("AT+CWQAP");
  delay(3000);
  ESP8266.println("AT+CWMODE=1");
  delay(2000);
  if(ESP8266.find("OK")){Serial.println("Set Mode OK");}
  ESP8266.println("AT+CWDHCP=1,1");
  delay(2000);
  if(ESP8266.find("OK")){Serial.println("Set DHCP Mode OK");}
  String cmd="AT+CWJAP=\"";
  cmd+=SSID;
  cmd+="\",\"";
  cmd+=PASS;
  cmd+="\"";
  Serial.println(cmd);
  ESP8266.println(cmd);  
  delay(8000);
  if(ESP8266.find("WIFI CONNECTED")){
    Serial.print("Connected, IP address:  ");
    delay(2000);
    ESP8266.println("AT+CIFSR");
  delay(2000);
   Serial.println("All good!");
  ESP8266.println("AT+CIPMUX=1");
    return true;
  }else{
    Serial.println("Not good!");
    ESP8266.println("AT+CIPMUX=1");
   
    return false;
  }
}

void sendTelemetry(int tempF, int humP, int cnT){
  String cmd = "AT+CIPSTART=4,\"TCP\",\"";
  cmd += IP;
  cmd += "\",80";
  Serial.println(cmd);
  ESP8266.println(cmd);
  delay(5000);
  if(ESP8266.find("Error")){
    Serial.println("Not good!");
    return;}
  else if(ESP8266.find("OK")){
    Serial.println("Can connect");
  }
  cmd = GET;
  cmd += tempF;
  cmd += "&field2=";
  cmd += humP;
  cmd += "&field3=";
  cmd += cnT;
  cmd += "\r\n";
  ESP8266.print("AT+CIPSEND=4,");
  ESP8266.println(cmd.length());
  Serial.println(cmd);
  delay(3000);
  if(ESP8266.find(">")){
      Serial.println("sending command... ");
    ESP8266.print(cmd);
  }else{
   Serial.println("closing");
    ESP8266.println("AT+CIPCLOSE");
  }
}

