#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>

#define ledSuccess D8
#define RST_PIN D3
#define SS_PIN D4

LiquidCrystal_I2C lcd(0x27, 16, 2);

MFRC522 mfrc522(SS_PIN, RST_PIN);

const char* machine = "new01";
const char* ssid = "ssid";
const char* password = "password";
const char* server = "http://000.000.00.000:0000/api/br/v1/modules/iot/cards/";

void setup() {
  Serial.begin(115200);
  pinMode(ledSuccess, OUTPUT);
  digitalWrite(ledSuccess, LOW);
  lcd.init();
  lcd.backlight();
  SPI.begin();
  mfrc522.PCD_Init();

  wifiConnect();
}

void wifiConnect() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("BUSCANDO REDE...");

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("CONECTADO...");
  delay(2000);

  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("PASSE O CARTAO");
}

void tagRead(MFRC522 mfrc522){
  char tag[15];
  char s[100];
  for (int i = 0; i < mfrc522.uid.size; i++){
    sprintf(s,"%x",mfrc522.uid.uidByte[i]);
    strcat( &tag[i] , s);
  }
  serverConnect(tag);
  return;
}

void serverConnect(String tag){
  HTTPClient http;
  http.begin(server + tag + "?private_key=XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX&machine=" + machine);
  int httpCode = http.GET();

  if (httpCode > 0) {
    String resultJson = http.getString();
    String line1 = resultJson.substring(10,26);
    String line2 = resultJson.substring(37,53);

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(line1);
    lcd.setCursor(0,1);
    lcd.print(line2);
    digitalWrite(ledSuccess, HIGH);

  } else {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("FALHA NA CONEXAO");
      lcd.setCursor(0,1);
      lcd.print("AO SERVIDOR...");
  }

  delay(5000);
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("PASSE O CARTAO");
  digitalWrite(ledSuccess, LOW);
  return;
}

void loop() {
  if(WiFi.status() == WL_CONNECTED){

    if ( ! mfrc522.PICC_IsNewCardPresent()) {
      return;
    }

    if ( ! mfrc522.PICC_ReadCardSerial()) {
      return;
    }

    tagRead(mfrc522);

    delay(1000);

  } else { wifiConnect(); }
}
