#include "DHT.h"

#include <WiFi.h>

#include <Firebase_ESP_Client.h>

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

#define WIFI_SSID "Ubee19E1"
#define WIFI_PASSWORD "interneT332211"

// Insert Firebase project API Key
#define API_KEY "AIzaSyA9Fg1TG7L0hClpFnsRRW_ps1Fn-2tghI0"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://smarthive-cf25a-default-rtdb.firebaseio.com/"

#define DHTPIN 4

#define DHTTYPE DHT11


DHT dht(DHTPIN, DHTTYPE);

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;


unsigned long sendDataPrevMillis = 0;
int intValue;
float floatValue;
int count = 0;
bool signupOK = false;





void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  //trataremos de conectarnos al wifi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }

  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

}

void loop() {
  // put your main code here, to run repeatedly:
  delay(2000);

  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 5000 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();

    //obtenemos la información del sensor
     float h = dht.readHumidity();
     float t = dht.readTemperature();

    //enviar los datos uwu

    if (isnan(h) || isnan(t)) {
     Serial.println(F("Failed to read from DHT sensor!"));
     return;
    }
    if (Firebase.RTDB.setFloat(&fbdo, "/colmenas/colmenas1/humedad", h)) {
      Serial.println("PASSED hum");
    } 
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    if (Firebase.RTDB.setFloat(&fbdo, "/colmenas/colmenas1/temperatura", t)) {
      Serial.println("PASSED temp");
    } 
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }

 
  
   }
}