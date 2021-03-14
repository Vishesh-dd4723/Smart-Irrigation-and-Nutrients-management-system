#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 2 ,1, 0, 4, 5, 6, 7, 3, POSITIVE);

bool raining = 0;
bool will_rain = 0;
// Below data is for Cotton crop.
// Corresponds to 1024
int expected_value = 700;
int min_value = 200;
int good_value = 1000;
// int moisture = 1;  // From Sensor
int N = 800;
int P = 400;
int K = 400;    // in Kg/ha
String crop="Cotton";
int crop_N_min = 600; 
int crop_N_max = 1000;
int crop_P_min = 300;
int crop_P_max = 500;
int crop_K_min = 250;
int crop_K_max = 400;

// My Wifi Crediatials, should be changed accordingly.
char *ssid = "Jai Shri Radhey!";
char *pass = "KrO0Bl165";  

String createUrl(int date,int months,int years){
  String dateString = String(date);
  String monthString = String(months);
  String yearString = String(years);
  String dt_string = yearString + '-' + monthString + '-' + dateString;
  String baseUrl = "http://api.weatherapi.com/v1/forecast.json?key=6138e9fd28844856b87160752202011";
  String place = "Hodal"; 
  int hours = 13;
  String resultUrl = baseUrl + '&' + "q=" + place + '&' + "dt=" + dt_string + '&' + "hour=" + hours;
  //Serial.println("Result Url = " + resultUrl);
  return(resultUrl); 
}

void nutrients_checker(){
    digitalWrite(D5, 0);   
    digitalWrite(D6, 0);
    N = analogRead(A0);  //  Taking value of Soil Moisture, when S1S0 = 00
    delay(100);
    digitalWrite(D5, 1);
    digitalWrite(D6, 0);
    P = analogRead(A0);  //  Taking value of Soil Moisture, when S1S0 = 01
    delay(100);
    digitalWrite(D5, 0);
    digitalWrite(D6, 1);
    K = analogRead(A0);  //  Taking value of Soil Moisture, when S1S0 = 10
    delay(100);
    lcd.setCursor(0,2);
    lcd.print("N: OK P: OK K: OK");
   if(N<crop_N_min || P<crop_P_min || K<crop_K_min){   //some nutrients is in less quantity
       String msg="You should use fertilizer that is rich in";
       if(N<crop_N_min){
        msg+=" Nitrogen ";
        lcd.setCursor(0,2);
        lcd.print("N: L ");
       }
       if(P<crop_P_min){
        msg+=" Phosphorus ";
        lcd.setCursor(6,2);
        lcd.print("P: L ");
       }
       if(K<crop_K_min){
        msg+=" Potassium ";
        lcd.setCursor(12,2);
        lcd.print("K: L ");
       }
       Serial.println(msg);
   }
   else if(N>crop_N_max || P>crop_P_max || K>crop_K_max){
       String msg="Your soil contains high amount of";
       if(N>crop_N_max){
        msg+=" Nitrogen ";
        lcd.setCursor(0,2);
        lcd.print("N: H ");
       }
       if(P>crop_P_max) {
        msg+=" Phosphorus ";
        lcd.setCursor(6,2);
        lcd.print("P: H ");
       }
       if(K>crop_K_max){
        msg+=" Potassium ";
        lcd.setCursor(12,2);
        lcd.print("K: H ");
       }
       Serial.println(msg);
   }
   else{
       String msg="Your soil is good for growing "+crop;
       Serial.println(msg);
   }  
}

void watering(){
  // Taking value of Soil Moisture, when S1S0 = 11
  digitalWrite(D5, 1);
  digitalWrite(D6, 1);
  //Serial.println(analogRead(A0));
  if (raining){
      lcd.setCursor(0,1);
      lcd.print("Rain, Sprinkler OFF");
      Serial.println("Its raining!!");
      Serial.println("No need to turn on sprinkler.");
  }
  else if( analogRead(A0) <min_value){
    lcd.setCursor(0,1);
    lcd.print("Sprinkler ON       ");
    Serial.println("Emergency!!,Sprinkler should ON");
    while(expected_value > analogRead(A0)){
       digitalWrite(D0,1);
       break;
     }
    digitalWrite(D0,0);
  }
  else if(analogRead(A0) < expected_value){
    if(will_rain){
       Serial.println("Wait for rain");
       lcd.setCursor(0,1);
       lcd.print("Sprinklers OFF    ");
    }else{
       Serial.println("No chance of rain,sprinkler should ON");
       while(good_value > analogRead(A0)){
        lcd.setCursor(0,1);
        lcd.print("Sprinklers ON   ");
        digitalWrite(D0,1);
        break;
       }
       digitalWrite(D0,0);
     }    
   }
 else{
        lcd.setCursor(0,1);
        lcd.print("Soil Moisture: GOOD");
        Serial.println("Soil Mosture is good");
     }
}
void setup() {
  Wire.begin(D2, D1); 
  lcd.begin(20,4);
  pinMode(D0, OUTPUT);  // Relay 
  pinMode(D5,OUTPUT); // For controlling MUX Dataline S0
  pinMode(D6,OUTPUT); // For controlling MUX Dataline S1
  Serial.begin(9600);
  Serial.print("\nConnecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while(WiFi.status() != WL_CONNECTED){
    Serial.print('.');
    delay(1000);
  }
  Serial.print("\nConnected to ");
  Serial.println(ssid);
}

void loop() 
{
  if (WiFi.status() == WL_CONNECTED) 
  {
    HTTPClient http; //Object of class HTTPClient
    http.begin(createUrl(22,11,2020));
    int httpCode = http.GET();
   
   // Serial.println(httpCode);
    
    if (httpCode > 0){
      const size_t bufferSize = JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(8) + 370;
      DynamicJsonBuffer jsonBuffer(bufferSize);
      JsonObject& root = jsonBuffer.parseObject(http.getString()); 
      String place = root["location"]["name"];
      String current_day = root["current"]["condition"]["text"];
      int day1 = root["forecast"]["forecastday"][0]["day"]["daily_will_it_rain"];
      int day2 = root["forecast"]["forecastday"][1]["day"]["daily_will_it_rain"];
      current_day.toLowerCase();
      raining = current_day.indexOf("rain") != -1;
      will_rain = day1 || day2;
      lcd.setCursor(0,0);
      lcd.print("Place:");
      lcd.print(place);
      Serial.println("Place: "+place);
      Serial.println(current_day);
      watering();
      nutrients_checker();
      lcd.setCursor(0,3);
      lcd.print("     By Agri-Venture");
    }
    http.end(); //Close connection

  }
  
  delay(20000);  // 20 sec
}
