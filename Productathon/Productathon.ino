#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 2 ,1, 0, 4, 5, 6, 7, 3, POSITIVE);

int expected_value = 4;
int min_value = 2;
int good_value = 6;
int moisture = 1;
bool raining = 0;
bool will_rain = 0;
int N,P,K;    // in Kg/ha
String crop="Rice";
int crop_N_min,crop_N_max;
int crop_P_min,crop_P_max;
int crop_K_min,crop_K_max;

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
  Serial.println("Result Url = " + resultUrl);
  return(resultUrl); 
}

void nutrients_checker(){
   if(N<crop_N_min || P<crop_P_min || K<crop_K_min){   //some nutrients is in less quantity
       String msg="You should use fertilizer that is rich in";
       if(N<crop_N_min) msg+=" Nitrogen ";
       if(P<crop_P_min) msg+=" Phosphorus ";
       if(K<crop_K_min) msg+=" Potassium ";
       Serial.println(msg);
   }
   else if(N>crop_N_max || P>crop_P_max || K>crop_K_max){
       String msg="Your soil contains high amount of";
       if(N>crop_N_max) msg+=" Nitrogen ";
       if(P>crop_P_max) msg+=" Phosphorus ";
       if(K>crop_K_max) msg+=" Potassium ";
       Serial.println(msg);
   }
   else{
       String msg="Your soil is good for growing "+crop;
   }  
}

void watering(){
  Serial.println(analogRead(A0));
  lcd.setCursor(0,0);
  lcd.print("Raining ");
  lcd.print(raining);
  if (raining){
      Serial.println("Its raining!!");
      Serial.println("No need to turn on sprinkler.");
  }
  else if( analogRead(A0) <min_value){
    Serial.println("Emergency!!,Sprinkler should ON");
    while(expected_value > analogRead(A0)){
       digitalWrite(D0,1);
     }
    digitalWrite(D0,0);
  }
  else if(analogRead(A0)<expected_value){
    if(will_rain){
       Serial.println("Wait for rain");
       lcd.setCursor(0,1);
        lcd.print("Sprinklers OFF  ");
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
        Serial.println("Soil Mosture is good");
     }
}
void setup() {
  Wire.begin(D2, D1); 
  lcd.begin(20,4);
  pinMode(D0, OUTPUT);  // Relay 
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
    //http.begin("http://api.weatherapi.com/v1/forecast.json?key=6138e9fd28844856b87160752202011&q=Hodal&days=2&dt="+"2020-11-23"+"&hour=12");
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
      Serial.println(place);
      Serial.println(current_day);
      Serial.print(day1);
      Serial.println(day2);
      watering();
    }
    http.end(); //Close connection

  }
  
  delay(60000);  // 15 sec
}
