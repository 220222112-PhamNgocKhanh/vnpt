#include <WiFi.h>
#include <WebServer.h>

const char *ssid = "hi";
const char *password = "123456789";
const int ledPin = 4;
bool ledState = 0;
WebServer server(80);

// html page for web control
String htmlPage()
{
  String html = R"rawliteral(
    <!DOCTYPE html>
    <html lang="en">
    <head>
      <meta charset="UTF-8">
      <title>ESP32 LED Control</title>
      <style>
        body {
          font-family: 'Segoe UI', sans-serif;
          background-color: #f0f2f5;
          display: flex;
          flex-direction: column;
          align-items: center;
          justify-content: center;
          height: 100vh;
          margin: 0;
        }
        h1 {
          color: #333;
          margin-bottom: 30px;
        }
        .button-container {
          display: flex;
          gap: 20px;
        }
        button {
          padding: 15px 40px;
          font-size: 18 px;
          border: none;
          border-radius: 8px;
          cursor: pointer;
          transition: all 0.3s ease;
        }
        .on-btn {
          background-color: #4CAF50;
          color: white;
        }
        .on-btn:hover {
          background-color: #45a049;
        }
        .off-btn {
          background-color: #f44336;
          color: white;
        }
        .off-btn:hover {
          background-color: #da190b;
        }
      </style>
    </head>
    <body>
      <h1>ESP32 LED Web Control</h1>
      <div class="button-container">
        <a href="/on"><button class="on-btn">Turn ON</button></a>
        <a href="/off"><button class="off-btn">Turn OFF</button></a>
      </div>
    </body>
    </html>
  )rawliteral";
  return html;
}

void handleRoot()
{
  server.send(200, "text/html", htmlPage());
}

void handleOn() // xu ly su kien khi nhan nut ON
{
  ledState = true;
  digitalWrite(ledPin, HIGH);
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleOff() // xu ly su kien khi nhan nut OFF
{
  ledState = false;
  digitalWrite(ledPin, LOW);
  server.sendHeader("Location", "/");
  server.send(303);
}

void setup()
{
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  WiFi.begin(ssid, password);
  Serial.print("connecting wifi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n connected to Wifi");
  Serial.print(" URL:  ");
  Serial.println(WiFi.localIP());
  server.on("/", handleRoot);
  server.on("/on", handleOn);
  server.on("/off", handleOff);
  server.begin();
  Serial.println(" web is running");
}

void loop()
{
  server.handleClient();
}
