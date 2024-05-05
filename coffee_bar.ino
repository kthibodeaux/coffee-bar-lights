#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_NeoPixel.h>
#include <EasyButton.h>

#define BUTTON_PIN D6
#define STRIP_LED_PIN D7
#define MSG_BUFFER_SIZE (50)

const char* ssid = "";
const char* password = "";
const char* mqtt_server = "";

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
char msg[MSG_BUFFER_SIZE];

EasyButton button(BUTTON_PIN);
bool stripLED = LOW;
const int numberOfPixels = 39;
Adafruit_NeoPixel pixels(numberOfPixels, STRIP_LED_PIN, NEO_GRB + NEO_KHZ800);

void turnOn() {
  pixels.clear();
  for (int i = 0; i < numberOfPixels; i++) {
    pixels.setPixelColor(i, pixels.Color(255, 255, 255));
    pixels.show();
  }
}

void turnOff() {
  pixels.clear();
  pixels.show();
}

void toggleLights() {
  Serial.println("Button pressed");
  stripLED = !stripLED;

  if (stripLED == HIGH) {
    turnOn();
  } else {
    turnOff();
  }
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    String clientId = "CoffeeBarClient-";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {
      client.subscribe("ha-coffee-bar");
    } else {
      delay(1000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  if ((char)payload[0] == '1') {
    turnOn();
  } else {
    turnOff();
  }
}

void setup() {
  Serial.begin(115200);

  pixels.begin();
  button.begin();
  button.onPressed(toggleLights);

  setup_wifi();

  client.setServer(mqtt_server, 8006);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  button.read();
}
