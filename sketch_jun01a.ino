#include <DHT.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

#define DHTPIN D2
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
#define D0 16
#define ledPin  D0
const char* ssid = "AP-FL2-CHATROOM"; //สร้างตัวแปรไว้เก็บชื่อ ssid ของ AP ของเรา
const char* pass = "0815209598"; //สร้างตัวแปรไว้เก็บชื่อ password ของ AP ของเรา

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);//Turn on the LED
  Serial.begin(9600);//ตั้งค่าใช้งาน serial ที่ baudrate 9600
  
  WiFiManager wifiManager;
  wifiManager.setTimeout(180);  
  if(!wifiManager.autoConnect("AutoConnectAP")){
      Serial.println("Failed to connect and hit timeout");
      delay(3000);
      ESP.reset();
      delay(5000);
  }
  WiFi.printDiag(Serial);
  Serial.println();
  Serial.println("Connected...OK");
  WiFi.begin(ssid, pass); //ทำการเชื่อมต่อไปยัง AP
  while (WiFi.status() != WL_CONNECTED) { //รอจนกว่าจะเชื่อมต่อสำเร็จ 
    Serial.print("."); //แสดง ... ไปเรื่อยๆ จนกว่าจะเชื่อมต่อได้
    delay(500);
  } //ถ้าเชื่อมต่อสำเร็จก็จะหลุก loop while ไป
  Serial.println(""); 
  Serial.println("Wi-Fi connected"); //แสดงว่าเชื่อมต่อ Wi-Fi ได้แล้ว
  Serial.print("IP Address : ");
  Serial.println(WiFi.localIP()); //แสดง IP ของบอร์ดที่ได้รับแจกจาก AP
  dht.begin();
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) { //ถ้าเชื่อมต่อสำเร็จ
    delay(600000);//อ่านทุกๆ 10 นาที
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    float f = dht.readTemperature(true);
    String getData, Link;
    getData = "?idlocation=1&h=" + String(h) + "&c=" + String(t) + "&f=" + String(f) ; 
    Link = "http://203.151.27.229:81/IoT/temperature_add.php" + getData;
  
    if (isnan(h) || isnan(t) || isnan(f)) {
      digitalWrite(ledPin, LOW);
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }
    HTTPClient http;
    http.begin(Link);
    int httpCode = http.GET();
  
    if (httpCode > 0) { //ถ้าส่งสำเร็จ
        String payload = http.getString();// อ่านค่าผลลัพธ์
        Serial.println(payload);
        digitalWrite(ledPin, HIGH);
        delay(250);
        digitalWrite(ledPin, LOW);
        delay(250);
     }
     http.end(); //ปิดการเชื่อมต่อ
  
    float hif = dht.computeHeatIndex(f, h);
    float hic = dht.computeHeatIndex(t, h, false);
  
    Serial.print(F("Humidity: "));
    Serial.print(h);
    Serial.print(F("%  Temperature: "));
    Serial.print(t);
    Serial.print(F(" C "));
    Serial.print(f);
    Serial.print(F(" F  Heat index: "));
    Serial.print(hic);
    Serial.print(F(" C "));
    Serial.print(hif);
    Serial.println(F(" F"));
  }
}
