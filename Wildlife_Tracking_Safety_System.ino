#include <Arduino.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <string.h>
#include <stdio.h>

// ========== HARDWARE SETUP ==========
LiquidCrystal_I2C lcd(0x27, 16, 2);
SoftwareSerial GSM(2, 3);  // Arduino RX = D2, TX = D3

#define DHTPIN 8
#define DHTTYPE DHT11

const byte led = 13;
const byte relay = 12;
const byte sosbutton = 6;

DHT dht(DHTPIN, DHTTYPE);

// ========== CONFIGURATION ==========
const char PHONE_NUMBER[] = "+91XXXXXXXXXX";  // Replace with your registered number before uploading to Arduino

// Original configured coordinates; no physical GPS module
const char LATITUDE[] = "18.651674";
const char LONGITUDE[] = "73.761536";

const float HIGH_TEMP_LIMIT = 50.0;
const float LOW_TEMP_LIMIT = 5.0;

const unsigned long SENSOR_INTERVAL = 2000;
const unsigned long DEBOUNCE_DELAY = 50;
const unsigned long SOS_COOLDOWN = 15000;

// ========== VARIABLES ==========
float temperature = -100.0;
float humidity = -100.0;

bool sensorValid = false;
bool relayOn = false;
bool automaticSOSActive = false;

int lastButtonReading = HIGH;
int stableButtonState = HIGH;

unsigned long lastDebounceTime = 0;
unsigned long lastSensorRead = 0;
unsigned long lastManualSOS = 0;
bool manualSOSPreviouslySent = false;

char incomingLine[100];
byte incomingIndex = 0;

// ========== FUNCTION DECLARATIONS ==========
void showMessage(const char *line1, const char *line2);
void updateTempHumidity();
void showMainDisplay();
void checkSOSButton();
void checkAutomaticSOS();
void processGSMCommands();
void handleCommand(char *command);
void triggerSOS(const char *alertType);
void sendSMS(const char *message);
void sendLocation();
void sendStatus();
void makeMapLink(char *buffer, size_t size);
void uppercaseText(char *text);

// ========== SETUP ==========
void setup()
{
  Serial.begin(9600);
  GSM.begin(9600);
  GSM.setTimeout(100);

  pinMode(led, OUTPUT);
  pinMode(relay, OUTPUT);
  pinMode(sosbutton, INPUT_PULLUP);

  digitalWrite(led, LOW);
  digitalWrite(relay, HIGH);  // Active-low relay OFF

  lcd.init();
  lcd.backlight();

  showMessage("WILDLIFE SAFETY", "SMART MONITOR");
  delay(1800);

  showMessage("Initializing...", "Please wait");

  dht.begin();

  GSM.listen();
  GSM.println(F("AT"));
  delay(500);

  GSM.println(F("AT+CMGF=1"));
  delay(500);

  GSM.println(F("AT+CNMI=2,2,0,0,0"));
  delay(500);

  Serial.println(F("Wildlife Safety System Ready"));
  Serial.println(F("Commands: STATUS, GETLOC, ON, OFF, HELP"));
  Serial.println(F("Automatic SOS: Above 50C / Below 5C"));
  Serial.println(F("Location uses configured demo coordinates"));

  updateTempHumidity();

  showMessage("SYSTEM READY", "Monitoring Active");
  delay(1200);

  lastSensorRead = millis();
  showMainDisplay();
}

// ========== MAIN LOOP ==========
void loop()
{
  if (millis() - lastSensorRead >= SENSOR_INTERVAL)
  {
    lastSensorRead = millis();

    updateTempHumidity();
    checkAutomaticSOS();
    showMainDisplay();
  }

  checkSOSButton();
  processGSMCommands();
}

// ========== LCD DISPLAY ==========
void showMessage(const char *line1, const char *line2)
{
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(line1);

  lcd.setCursor(0, 1);
  lcd.print(line2);
}

void showMainDisplay()
{
  lcd.setCursor(0, 0);
  lcd.print(F("WILDLIFE MONITOR"));

  lcd.setCursor(0, 1);

  if (!sensorValid)
  {
    lcd.print(F("DHT Sensor Error "));
    return;
  }

  lcd.print(F("T:"));
  lcd.print(temperature, 1);
  lcd.print(F("C H:"));
  lcd.print(humidity, 0);
  lcd.print(F("%   "));
}

// ========== DHT11 SENSOR ==========
void updateTempHumidity()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (!isnan(h) && !isnan(t))
  {
    humidity = h;
    temperature = t;
    sensorValid = true;

    Serial.print(F("Temperature: "));
    Serial.print(temperature, 1);
    Serial.print(F(" C | Humidity: "));
    Serial.print(humidity, 0);
    Serial.println(F(" %"));
  }
  else
  {
    sensorValid = false;
    Serial.println(F("DHT11 read failed; check wiring"));
  }
}

// ========== SHARED SOS FUNCTION ==========
void triggerSOS(const char *alertType)
{
  digitalWrite(led, HIGH);

  showMessage("!! SOS ALERT !!", "ANIMAL IN DANGER");

  char mapLink[75];
  char message[160];

  makeMapLink(mapLink, sizeof(mapLink));

  snprintf(message, sizeof(message),
           "WILDLIFE SOS! ANIMAL IN DANGER. %s. Map: %s",
           alertType, mapLink);

  Serial.println(F("SOS ALERT ACTIVATED"));
  Serial.println(message);

  sendSMS(message);

  showMessage("SOS ACTIVATED", "Alert Requested");
  delay(1000);

  digitalWrite(led, LOW);
  showMainDisplay();
}

// ========== MANUAL SOS BUTTON ==========
void checkSOSButton()
{
  int reading = digitalRead(sosbutton);

  if (reading != lastButtonReading)
  {
    lastDebounceTime = millis();
    lastButtonReading = reading;
  }

  if (millis() - lastDebounceTime >= DEBOUNCE_DELAY)
  {
    if (reading != stableButtonState)
    {
      stableButtonState = reading;

      // Trigger once when the button is pressed
      if (stableButtonState == LOW)
      {
        if (!manualSOSPreviouslySent ||
            millis() - lastManualSOS >= SOS_COOLDOWN)
        {
          manualSOSPreviouslySent = true;
          lastManualSOS = millis();

          Serial.println(F("Manual SOS button pressed"));
          triggerSOS("MANUAL SOS");
        }
        else
        {
          Serial.println(F("Manual SOS cooldown active"));
          showMessage("SOS COOLDOWN", "Please wait");
          delay(700);
          showMainDisplay();
        }
      }
    }
  }
}

// ========== AUTOMATIC TEMPERATURE SOS ==========
void checkAutomaticSOS()
{
  if (!sensorValid)
    return;

  // One automatic SOS per extreme-temperature event
  if (temperature > HIGH_TEMP_LIMIT)
  {
    if (!automaticSOSActive)
    {
      automaticSOSActive = true;
      Serial.println(F("HIGH TEMPERATURE - AUTO SOS"));
      triggerSOS("HIGH TEMP ABOVE 50C");
    }
  }
  else if (temperature < LOW_TEMP_LIMIT)
  {
    if (!automaticSOSActive)
    {
      automaticSOSActive = true;
      Serial.println(F("LOW TEMPERATURE - AUTO SOS"));
      triggerSOS("LOW TEMP BELOW 5C");
    }
  }
  else
  {
    // Rearm after temperature returns to 5-50C
    automaticSOSActive = false;
  }
}

// ========== GSM INCOMING SMS ==========
void processGSMCommands()
{
  GSM.listen();

  while (GSM.available() > 0)
  {
    char c = GSM.read();

    if (c == '\r' || c == '\n')
    {
      if (incomingIndex > 0)
      {
        incomingLine[incomingIndex] = '\0';

        // Ignore modem headers such as +CMT:
        if (incomingLine[0] != '+')
        {
          uppercaseText(incomingLine);
          handleCommand(incomingLine);
        }

        incomingIndex = 0;
      }
    }
    else if (incomingIndex < sizeof(incomingLine) - 1)
    {
      incomingLine[incomingIndex++] = c;
    }
    else
    {
      incomingIndex = 0;
    }
  }
}

void uppercaseText(char *text)
{
  while (*text)
  {
    if (*text >= 'a' && *text <= 'z')
      *text = *text - 'a' + 'A';

    text++;
  }
}

// ========== SMS COMMANDS ==========
void handleCommand(char *command)
{
  Serial.print(F("Received command: "));
  Serial.println(command);

  if (strcmp(command, "GETLOC") == 0)
  {
    sendLocation();
  }
  else if (strcmp(command, "STATUS") == 0)
  {
    sendStatus();
  }
  else if (strcmp(command, "HELP") == 0)
  {
    sendSMS(
      "Commands: STATUS, GETLOC, ON, OFF, HELP. "
      "SOS: button or temperature above 50C/below 5C."
    );
  }
  else if (strcmp(command, "ON") == 0)
  {
    digitalWrite(relay, LOW);
    relayOn = true;

    showMessage("DEMO OUTPUT", "Relay: ON");
    sendSMS("Wildlife Monitor: Relay ON");
  }
  else if (strcmp(command, "OFF") == 0)
  {
    digitalWrite(relay, HIGH);
    relayOn = false;

    showMessage("DEMO OUTPUT", "Relay: OFF");
    sendSMS("Wildlife Monitor: Relay OFF");
  }
}

// ========== GOOGLE MAPS LOCATION ==========
void makeMapLink(char *buffer, size_t size)
{
  snprintf(buffer, size,
           "https://maps.google.com/?q=%s,%s",
           LATITUDE, LONGITUDE);
}

void sendLocation()
{
  char mapLink[75];
  char message[140];

  makeMapLink(mapLink, sizeof(mapLink));

  snprintf(message, sizeof(message),
           "Animal location: %s. Configured demo coordinates, not live GPS.",
           mapLink);

  sendSMS(message);
}

// ========== STATUS SMS ==========
void sendStatus()
{
  char message[120];
  char tempText[12];
  char humText[12];

  if (sensorValid)
  {
    dtostrf(temperature, 4, 1, tempText);
    dtostrf(humidity, 3, 0, humText);

    snprintf(message, sizeof(message),
             "WILDLIFE STATUS: Temp %sC, Humidity %s%%. Relay: %s.",
             tempText, humText, relayOn ? "ON" : "OFF");
  }
  else
  {
    snprintf(message, sizeof(message),
             "WILDLIFE STATUS: DHT sensor error. Relay: %s.",
             relayOn ? "ON" : "OFF");
  }

  sendSMS(message);
}

// ========== SEND SMS ==========
void sendSMS(const char *message)
{
  GSM.listen();

  // Set SMS text mode
  GSM.println(F("AT+CMGF=1"));
  delay(300);

  // Request recipient
  GSM.print(F("AT+CMGS=\""));
  GSM.print(PHONE_NUMBER);
  GSM.println(F("\""));

  // Wait for the SMS prompt
  unsigned long startTime = millis();
  bool promptReceived = false;

  while (millis() - startTime < 5000)
  {
    while (GSM.available())
    {
      char c = GSM.read();

      if (c == '>')
      {
        promptReceived = true;
        break;
      }
    }

    if (promptReceived)
      break;
  }

  if (!promptReceived)
  {
    Serial.println(F("SMS failed: no modem prompt"));
    return;
  }

  // Send SMS text and Ctrl+Z
  GSM.print(message);
  GSM.write(26);

  // Wait for modem acknowledgement
  startTime = millis();
  char response[40];
  byte index = 0;
  bool accepted = false;

  while (millis() - startTime < 15000)
  {
    while (GSM.available())
    {
      char c = GSM.read();

      if (c == '\r' || c == '\n')
      {
        response[index] = '\0';

        if (strstr(response, "+CMGS:") != NULL)
        {
          accepted = true;
          break;
        }

        index = 0;
      }
      else if (index < sizeof(response) - 1)
      {
        response[index++] = c;
      }
    }

    if (accepted)
      break;
  }

  if (accepted)
    Serial.println(F("Modem accepted SMS"));
  else
    Serial.println(F("SMS unconfirmed; check GSM network"));
}