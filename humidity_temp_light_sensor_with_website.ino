#include <DHT.h>
#include <SPI.h>
#include <Ethernet.h>
#include <OneWire.h>
int sensorPin = A1;   
int sensorValue = 0;

#define DHTPIN 2  // what digital pin we're connected to
// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT11  // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
#define BUZZER 5 //buzzer pin position 2
#define LED_R 4 //define red LED pin position 4


// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.
DHT dht(DHTPIN, DHTTYPE);



byte mac[] = {
  0x48, 0xC2, 0xA1, 0xF3, 0x8D, 0xB7
};
byte ip[] = {
  192, 168, 137, 198
};

EthernetServer server = EthernetServer(80);
void setup() {
  Serial.begin(9600);
  Serial.println("DHTxx test!");

  Ethernet.begin(mac, ip);
  server.begin();
  dht.begin();
  pinMode(LED_R, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  noTone(BUZZER);
}

void loop() {
  // Wait a few seconds between measurements.
  
  delay(2000);
  noTone(BUZZER);
  digitalWrite(LED_R, LOW);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)

  

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);
  float f = dht.readTemperature(true);
  sensorValue = analogRead(sensorPin);
  // turn the ledPn
 
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print("Heat index: ");
  Serial.print(hic);
  Serial.print(" *C ");
  Serial.println(sensorValue);
  if (sensorValue > 700){
    Serial.println("Lights off");
    tone(BUZZER, 400);
    digitalWrite(LED_R, HIGH);
  }
   if (sensorValue > 700){
            if(t > 27.90){
            tone(BUZZER, 400);
            digitalWrite(LED_R, HIGH);
            }
          }
  if (sensorValue < 700){
    Serial.println("Lights on");
    digitalWrite(LED_R, LOW);
    noTone(BUZZER);
  }
  
  
  EthernetClient client = server.available();
  if (client) {
    boolean BlankLine = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();

        if (c == '\n' && BlankLine) {
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html\n");
          client.println("<html><head><META HTTP-EQUIV="
                         "refresh"
                         " CONTENT="
                         "5"
                         ">\n");
          client.println("<title>Makerbay Energy</title></head>");
          client.println("<h1 style =\"color:#34495E\">\n");
          client.println("<h1 style =\"font-family: Impact\">\n");
          client.println("<body style=\"background-color:#D2B4DE\">\n");
          client.println("<center><b><h1>Makerbay Energy Log</h1></b></center>");
          client.println("<h3>Temperature</h3>");

          client.println(t);
          client.println("&deg;C");
          client.println("<br><br><br>");
          client.println("<h4>Humidity</h4>");
          client.println(h);
          client.println("%\t");
          client.println("<br>");
          client.println("<h4>Heat Index</h4>");
          client.println(hic);
          client.println("&deg;C");
          client.println("<br>");
          client.println("<h4>Light Intensity</h4>");
          client.println(sensorValue);
          
          client.println("<br>");
          client.println("<br>");
          if (sensorValue > 700){
            client.println("<h2 style =\"color:#DC143C\">\n");
            client.println("<h2>Light off</h2>");
          }
          if (sensorValue > 700){
            if(t > 27.90){
            client.println("<h2 style =\"color:#DC143C\">\n");
            client.println("<h2>Light off, AC left on</h2>");
            }
          }
          if (sensorValue < 700){
            client.println("<h2 style =\"color:#BDE772\">\n");
            client.println("<h2>Light on</h2>");
          }
          client.println("</body>\n</html>");

          break;
        }
        if (c == '\n') {
          BlankLine = true;
        } else if (c != '\r') {
          BlankLine = false;
        }
      }
    }
    delay(10);
    client.stop();
  }
}
