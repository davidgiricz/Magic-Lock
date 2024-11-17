#define BLYNK_TEMPLATE_ID "TMPL4Tf9OQj5z"
#define BLYNK_TEMPLATE_NAME "ASDASD"
#define BLYNK_AUTH_TOKEN "lCC6z9e-zZpshth4c7Q7umyT7qqgplK6"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <BlynkSimpleEsp8266.h>

char ssid[] = "Saját";
char pass[] = "12345678";

int lockPin = 0;      // Zár vezérlése
int buzzerPin = 2;    // Buzzer vezérlése
int ledPin = 13;       // LED vezérlése (fizikai 4-es pin)
int freqOn = 1000;    // Bekapcsolás frekvenciája
int freqOff = 3000;    // Kikapcsolás frekvenciája
int beepCount = 3;    // Hányszor csipogjon
int beepDuration = 25;  // Csipogás időtartama (ms)
int pauseDuration = 25; // Szünet időtartama (ms)
unsigned long startTime = 0; // Időzítés kezdete
bool isFlashing = false;     // LED villogás állapota
bool ledState = false;       // LED állapot tárolása

BLYNK_WRITE(V1)  // Ez a Blynk virtuális pin figyeli a gomb állapotát
{
  int buttonState = param.asInt();  // A gomb állapota: 1 (bekapcsolva), 0 (kikapcsolva)

  if (buttonState == 1) {
    digitalWrite(lockPin, HIGH);  // Zár kinyitása (2-es pin HIGH)
    beepBuzzer(freqOn);           // Csipogás 1000 Hz-en
    startTime = millis();         // Időzítés elindítása a 10 másodperces méréshez
    isFlashing = true;            // Villogás engedélyezése
  } else {
    digitalWrite(lockPin, LOW);   // Zár bezárása (2-es pin LOW)
    beepBuzzer(freqOff);          // Csipogás 300 Hz-en
    isFlashing = false;           // Villogás leállítása
    digitalWrite(ledPin, LOW);    // LED kikapcsolása
    ledState = false;             // LED állapotának alaphelyzetbe állítása
    Blynk.virtualWrite(V2, 0);    // LED állapotának elküldése Blynk-re (kikapcsolt)
  }
}

// Csipogási folyamat megvalósítása
void beepBuzzer(int frequency) {
  for (int i = 0; i < beepCount; i++) {
    tone(buzzerPin, frequency);      // Buzzer bekapcsolása a megadott frekvencián
    delay(beepDuration);             // Csipogási időtartam
    noTone(buzzerPin);               // Buzzer kikapcsolása
    delay(pauseDuration);            // Szünet a csipogások között
  }
}

void setup()
{
  // Soros monitor indítása a hibakereséshez
  Serial.begin(115200);

  // Blynk kapcsolat indítása
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  pinMode(lockPin, OUTPUT);  // A lockPin kimenetként van beállítva (zár vezérlés)
  pinMode(buzzerPin, OUTPUT);  // A buzzerPin kimenetként van beállítva
  pinMode(ledPin, OUTPUT);   // A ledPin (4-es fizikai pin) kimenetként van beállítva
}

void loop()
{
  Blynk.run();  // Blynk futtatása

  // Ha villogás engedélyezett és eltelt 10 másodperc a V1 bekapcsolása óta
  if (isFlashing && (millis() - startTime >= 5000)) {
    // Villogtatás fél másodpercenként
    ledState = !ledState;  // Állapot váltása
    digitalWrite(ledPin, ledState);  // LED ki/be kapcsolása
    Blynk.virtualWrite(V2, ledState ? 255 : 0); // LED állapotának elküldése Blynk-re
    delay(400);  // Fél másodperces késleltetés
  }
}
