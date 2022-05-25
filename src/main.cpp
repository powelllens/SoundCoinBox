#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Adafruit_Soundboard.h>

// Choose any two pins that can be used with SoftwareSerial to RX & TX
#define SFX_TX 8
#define SFX_RX 9

// Connect to the RST pin on the Sound Board
#define SFX_RST 7

#define PowerRelaiPin 4
#define MusicBoardPowerPin 6
#define MusicBoardActivePin 10
#define MusicSpeakerRelaiPin A1
//#define MusicBoardResetPin 7
#define CoinCounterPowerPin 5
#define ButtonPin 3
#define CoinTriggerPin 2

#define BatteryVoltagePin A0

int BatteryVoltageRaw = 0;
double BatteryVoltage = 0.0;
bool ButtonPressed = false;
int ButtonCounter = 0;
int BatteryTime = 0;
int BatteryCounter = 0;
int GlobalTimer = 0;
bool BatteryCheck = false;
bool Coindetected = false;
bool CoindetectionActive = false;
bool CoinSignal = false;
bool SoundTriggered = false;
bool SoundRunning = false;
int Zufallszahl = 0;
bool MusicActiveOld = false;
bool MusicActive = false;

// You can also monitor the ACT pin for when audio is playing!

// we'll be using software serial
SoftwareSerial ss = SoftwareSerial(SFX_TX, SFX_RX);

// pass the software serial to Adafruit_soundboard, the second
// argument is the debug port (not used really) and the third
// arg is the reset pin
Adafruit_Soundboard sfx = Adafruit_Soundboard(&ss, NULL, SFX_RST);
// can also try hardware serial with
// Adafruit_Soundboard sfx = Adafruit_Soundboard(&Serial1, NULL, SFX_RST);
char SoundName[13];
int TracksPlayed[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

/************************ MENU HELPERS ***************************/

void flushInput()
{
  // Read all available serial input to flush pending data.
  uint16_t timeoutloop = 0;
  while (timeoutloop++ < 40)
  {
    while (ss.available())
    {
      ss.read();
      timeoutloop = 0; // If char was received reset the timer
    }
    delay(1);
  }
}

void CoinTrigger_ISR()
{
  CoinSignal = not digitalRead(CoinTriggerPin);
  if (CoinSignal)
  {
    GlobalTimer = 0;
    Serial.println("50ct Coin Detected!");
    Coindetected = true;
    digitalWrite(CoinCounterPowerPin, LOW);
  }
}

int getRandomNewNumber()
{
  bool nodouble = false;
  int Number;
  if (TracksPlayed[8] != 0)
  {
    TracksPlayed[0] = TracksPlayed[8];
    for (int i = 1; i <= 8; i++)
    {
      TracksPlayed[i] = 0;
    }
    Serial.println("new");
  }
  do
  {

    Number = random(2, 11);
    nodouble = true;
    for (int i = 0; i <= 8; i++)
    {
      if (TracksPlayed[i] == Number)
      {
        nodouble = false;
      }
      else if ((TracksPlayed[i] == 0) && (nodouble == true))
      {
        TracksPlayed[i] = Number;
        break;
      }
    }

  } while (not nodouble);
  return Number;
}

void setup()
{
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(PowerRelaiPin, OUTPUT);
  pinMode(MusicBoardPowerPin, OUTPUT);
  pinMode(MusicSpeakerRelaiPin, OUTPUT);
  pinMode(CoinCounterPowerPin, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(ButtonPin, INPUT);
  pinMode(BatteryVoltagePin, INPUT);
  pinMode(CoinTriggerPin, INPUT);
  pinMode(MusicBoardActivePin, INPUT);

  digitalWrite(ButtonPin, HIGH);
  digitalWrite(CoinTriggerPin, HIGH);
  digitalWrite(LED_BUILTIN, LOW);

  digitalWrite(MusicBoardPowerPin, LOW);
  digitalWrite(MusicSpeakerRelaiPin, LOW);
  digitalWrite(CoinCounterPowerPin, LOW);
  digitalWrite(PowerRelaiPin, HIGH);

  Serial.begin(115200);

  attachInterrupt(digitalPinToInterrupt(CoinTriggerPin), CoinTrigger_ISR, CHANGE);

  BatteryVoltageRaw = analogRead(BatteryVoltagePin);
  BatteryVoltage = 0.0208 * BatteryVoltageRaw + 0.083;
  Serial.println("Voltage:" + String(BatteryVoltage));
  if (BatteryVoltage < 6.95)
  {
    digitalWrite(PowerRelaiPin, LOW);
    delay(1000);
  }
  else
  {
    digitalWrite(MusicBoardPowerPin, HIGH);
    digitalWrite(CoinCounterPowerPin, HIGH);
    delay(200);
  }

  Serial.println("Adafruit Sound Board!");
  // Softwareserial at 9600 baud
  ss.begin(9600);
  // can also do Serial1.begin(9600)

  if (!sfx.reset())
  {
    Serial.println("Not found");
    digitalWrite(PowerRelaiPin, LOW);
  }
  Serial.println("SFX board found");
  flushInput();
  char TrackNameMain[12]; // Declare the array
  strcpy(TrackNameMain, "T00     OGG");
  if (!sfx.playTrack(TrackNameMain))
  {
    Serial.println("Failed to play track 00");
  }
  digitalWrite(MusicSpeakerRelaiPin, HIGH);
  CoindetectionActive = true;
}

// the loop function runs over and over again forever
void loop()
{
  ButtonPressed = not digitalRead(ButtonPin);
  digitalWrite(LED_BUILTIN, ButtonPressed);

  if (ButtonPressed)
  {
    ButtonCounter = ButtonCounter + 1;
    if (ButtonCounter == 40)
    {
      BatteryVoltageRaw = analogRead(BatteryVoltagePin);
      BatteryVoltage = 0.0208 * BatteryVoltageRaw + 0.083;
      Serial.println("Voltage:" + String(BatteryVoltage));
      char TrackNameMain[12]; // Declare the array
      strcpy(TrackNameMain, "T01     OGG");
      if (!sfx.playTrack(TrackNameMain))
      {
        Serial.println("Failed to play track 01");
      }
      BatteryCheck = true;
      BatteryCounter = 25; // 0%
      if (BatteryVoltage > 8.0)
      {
        BatteryCounter = 40; // 20%
      }
      if (BatteryVoltage > 9.0)
      {
        BatteryCounter = 60; // 40%
      }
      if (BatteryVoltage > 10.0)
      {
        BatteryCounter = 80; // 60%
      }
      if (BatteryVoltage > 11.0)
      {
        BatteryCounter = 100; // 80%
      }
      if (BatteryVoltage > 11.8)
      {
        BatteryCounter = 120; // 100%
      }
      BatteryTime = 0;
      while (BatteryCheck)
      {
        BatteryTime = BatteryTime + 1;
        if (BatteryTime > BatteryCounter)
        {
          BatteryCheck = false;
          sfx.stop();
        }
        delay(100);
      }
    }
  }
  else
  {
    ButtonCounter = 0;
  }

  flushInput();
  randomSeed(analogRead(BatteryVoltagePin));

  if (Coindetected)
  {
    while (not digitalRead(MusicBoardActivePin))
    {
      delay(10);
    }
    delay(1000);
    Zufallszahl = getRandomNewNumber();
    char TitleName[3];
    String Zufallstr;
    Zufallstr = String(Zufallszahl);
    Zufallstr.toCharArray(TitleName, 3);
    if (Zufallszahl < 10)
    {
      TitleName[1] = TitleName[0];
      TitleName[0] = 48;
    }
    String Track = "T" + String(TitleName) + "     OGG";
    char TrackName[12];
    Track.toCharArray(TrackName, 12);
    if (!sfx.playTrack(TrackName))
    {
      Serial.print("Failed to play track " + Zufallszahl);
    }
    SoundTriggered = true;
    Coindetected = false;
    delay(500);
  }

  SoundRunning = not digitalRead(MusicBoardActivePin);
  if ((SoundTriggered == true) and (SoundRunning == false))
  {
    Serial.println("Reset");
    digitalWrite(CoinCounterPowerPin, HIGH);
    delay(500);
    SoundTriggered = false;
  }

  GlobalTimer = GlobalTimer + 1;
  if (GlobalTimer > 250)
  {
    digitalWrite(PowerRelaiPin, LOW);
    digitalWrite(MusicBoardPowerPin, LOW);
    digitalWrite(CoinCounterPowerPin, LOW);
    digitalWrite(MusicSpeakerRelaiPin, LOW);
  }
  delay(10);
}
