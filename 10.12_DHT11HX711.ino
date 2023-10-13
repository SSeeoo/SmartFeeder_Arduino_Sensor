#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <HX711.h>

// WiFi 정보
const char* ssid = "205";
const char* password = "";

// 웹 서버 정보
const char* serverName = "http://ec2-3-39-230-67.ap-northeast-2.compute.amazonaws.com:5000"; // AWS EC2 Public DNS

// DHT 센서 설정
#define DHTPIN 2   // 0번 -> 2번
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// HX711 센서 설정
#define DOUT 13 // 4번(Flash) -> 13번
#define CLK 12 // 2번핀 내장 플래시 -> 12번핀으로 변경
HX711 scale;

void setup() {
  Serial.begin(115200);
  
  // WiFi 연결
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("WiFi 연결 시도...");
  }
  Serial.println("WiFi 연결 성공");

  // 센서 초기화
  dht.begin();
  scale.begin(DOUT, CLK);
}

void loop() {
  // DHT 센서에서 데이터 읽기
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // HX711 센서에서 데이터 읽기
  float weight = scale.get_units(5);  // 5번 측정하여 평균값 계산

  // 데이터 전송
  if(WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverName);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
    String httpRequestData = "temperature=" + String(temperature) + "&humidity=" + String(humidity) + "&weight=" + String(weight);
    int httpResponseCode = http.POST(httpRequestData);
    
    if (httpResponseCode>0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
    }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  }
  else {
    Serial.println("WiFi 연결 끊김");
  }
  delay(10000);  // 10초마다 데이터 전송
}
