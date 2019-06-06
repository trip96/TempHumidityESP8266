
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <DHT.h>
#include <PubSubClient.h>

#define DHTTYPE DHT22
#define DHTPIN D4

// WiFi Credentials
const char* ssid     = "YOUR_OWN_SSID";
const char* password = "PASSWORD_FOR_NETWORK";

// MQTT PubSub Initialize
WiFiClient espClient;
PubSubClient client(espClient);

// MQTT server address
const char* mqtt_server = "192.168.1.159";

//MQTT Message initial values
long lastMsg = 0;
char msg[50];
int value = 0;
char data[50];
char comma[] = " ";

//// Sleep Seconds
//const int sleepSeconds = 30;

//Initialize DHT
DHT dht(DHTPIN, DHTTYPE);

float humidity, temperature;                 // Raw float values from the sensor
char str_humidity[10], str_temperature[10];  // Rounded sensor values and as strings
// Generally, you should use "unsigned long" for variables that hold time
//unsigned long previousMillis = 0;            // When the sensor was last read
//const long interval = 2000;                  // Wait this long until reading again

void read_sensor() {
  Serial.print("In Reading Now...");
  //  // Wait at least 2 seconds seconds between measurements.
  //  // If the difference between the current time and last time you read
  //  // the sensor is bigger than the interval you set, read the sensor.
  //  // Works better than delay for things happening elsewhere also.
//  unsigned long currentMillis = millis();
//
//  if (currentMillis - previousMillis >= interval) {
//    // Save the last time you read the sensor
//    previousMillis = currentMillis;
//
//    // Reading temperature and humidity takes about 250 milliseconds!
//    // Sensor readings may also be up to 2 seconds 'old' (it's a very slow sensor)
//

    humidity = dht.readHumidity();        // Read humidity as a percent
    temperature = dht.readTemperature();  // Read temperature as Celsius

    
//  }
//
//  // Check if any reads failed and exit early (to try again).
//  if (isnan(humidity) || isnan(temperature)) {
//    Serial.println("Failed to read from DHT sensor!");
//    return;
//  }

  // Convert the floats to strings and round to 2 decimal places
  dtostrf(humidity, 1, 2, str_humidity);
  dtostrf(temperature, 1, 2, str_temperature);

  Serial.print("Humidity: ");
  Serial.print(str_humidity);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(str_temperature);
  Serial.println(" Â°C");

}

//void callback(char* topic, byte* payload, unsigned int length) {
//  Serial.print("Message arrived [");
//  Serial.print(topic);
//  Serial.print("] ");
//  for (int i = 0; i < length; i++) {
//    Serial.print((char)payload[i]);
//  }
//  Serial.println();
//
//  // Switch on the LED if an 1 was received as first character
//  if ((char)payload[0] == '1') {
//    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
//    // but actually the LED is on; this is because
//    // it is acive low on the ESP-01)
//  } else {
//    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
//  }
//
//}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
//      // Once connected, publish an announcement...
//      client.publish("outTopic", "hello world");
//      // ... and resubscribe
//      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void setup(void)
{
  WiFi.persistent(false);
  // Open the Arduino IDE Serial Monitor to see what the code is doing
  Serial.begin(9600);
  dht.begin();

  client.setServer(mqtt_server, 1883);
//  client.setCallback(callback);

  Serial.println("WeMos DHT Server");
  Serial.println("");

  // Connect to your WiFi network
  WiFi.begin(ssid, password);
  Serial.print("Connecting");

  // Wait for successful connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to: ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("");

  // Connect to MQTT
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  //  ESP.deepSleep(sleepSeconds * 1000000);
}

void loop(void)
{

  // Connect to MQTT
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Read Values From DHT
  Serial.print("Getting DHT Values");
  char data[50] = {0};
  read_sensor();
  strcat(data, str_temperature);
  strcat(data, comma);
  strcat(data, str_humidity);

  client.publish("tent", data);
  delay(30000);
}

