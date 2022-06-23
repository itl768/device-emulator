
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#ifndef STASSID
#define STASSID "Test" //wifi username
#define STAPSK  "123456789" // wifi password
#endif
#define serverAddress "http://stage-api.anyiot.io/api/iot" // api endpoint
#define accessToken "934112b9-05a0-4a4d-9fe3-247565d95ba2" //access token


const unsigned long SECOND = 1000;
const unsigned long HOUR = 3600 * SECOND;

const int delayHours = 2; //delay between datapush in hours
char *deviceIds[] = {"a82e7138-4923-4ad3-b74e-5be0ccb878ea"}; // array of device ids
int numOfDevices = 0; //number of devices defined in the array

String binSensorId = "a82e7138-4923-4ad3-b74e-5be0ccb878ea";

const int trigPin = 5;  //D4
const int echoPin = 4;  //D3
const int analogInPin = A0; 
int sensorValue = 0;

// defines variables
long duration;
int distance;

void setup() {

  Serial.begin(115200);

  Serial.println();
  Serial.println();
  Serial.println();
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  WiFi.begin(STASSID, STAPSK);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());

}

void loop() {
  // wait for WiFi connection
  if ((WiFi.status() == WL_CONNECTED)) {

    WiFiClient client;

    for (int i = 0; i < numOfDevices; i++) {

      HTTPClient http;

      Serial.print("[HTTP] begin...\n");
      // configure traged server and url
      http.begin(client, serverAddress); //HTTP
      http.addHeader("Content-Type", "application/json");
      http.addHeader("access-token", accessToken);
      http.addHeader("device-token", deviceIds[i]);

      String httpRequestData = "{\"Level\":\"" + String(random(1000)) + "\",\"Voltage\":\"" + String(random(100)) + "\"}";
      Serial.print("[HTTP] POST...\n");
      int httpCode = http.POST(httpRequestData);

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] POST... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK) {
          const String& payload = http.getString();
          Serial.println("received payload:\n<<");
          Serial.println(payload);
          Serial.println(">>");
        }
      } else {
        Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }

      http.end();
    }




    ///send level from bin sensor

    HTTPClient http;

    Serial.print("[HTTP] begin...\n");
    // configure traged server and url
    http.begin(client, serverAddress); //HTTP
    http.addHeader("Content-Type", "application/json");
    http.addHeader("access-token", accessToken);
    http.addHeader("device-token", binSensorId);

   int level=readDistance();
   sensorValue = analogRead(analogInPin);
   int percentage=(sensorValue/1024)*100;
   
   int batteryPercentage=(sensorValue);
 Serial.println(level);
    String httpRequestData = "{\"Level\":\"" + String(level) + "\",\"Voltage\":\"" + String(percentage) + "\"}";
    Serial.print("[HTTP] POST...\n");
    int httpCode = http.POST(httpRequestData);

    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] POST... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        const String& payload = http.getString();
        Serial.println("received payload:\n<<");
        Serial.println(payload);
        Serial.println(">>");
      }
    } else {
      Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();

  }
Serial.println("sleeping");
 /// delay(delayHours * HOUR);
  ESP.deepSleep(delayHours * HOUR); 
}



int readDistance() {

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;

  return distance;
}
