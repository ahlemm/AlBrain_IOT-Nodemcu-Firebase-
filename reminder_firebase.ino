
#include <ESP8266WiFi.h>
#include <time.h>
#include <LiquidCrystal_I2C.h>
#define buzzer D5
#include <FirebaseArduino.h>


#define FIREBASE_HOST "albrain-c1bcc.firebaseio.com"

const char* ssid = "bouk.etcha7ma";
const char* password = "01010101";
String c = "medecine String";
String reponse = "done:)" ;
#define FIREBASE_AUTH "fqe1LwMwL1L1E8qejIe8k3eEaSLkMdo0AL4pgcKX"

#define LED 2
int ledPin = 13;
int timezone = 1 * 3600;
int dst = 0;
float total=17 ;
const int yes = D7;//A0;
const int no = D8;//A1;
int arret = 0;
int H ;
int M ;
float take_medecine = 15 ;


LiquidCrystal_I2C lcd(0x25, 20, 4);
void setup() {
  
  pinMode(yes, INPUT_PULLUP);
  digitalWrite(yes, HIGH);
  pinMode(no, INPUT_PULLUP);
  digitalWrite(no, HIGH);
  lcd.begin(16, 2);    // The begin call takes the width and height. This
  lcd.init();          // Should match the number provided to the constructor.
  lcd.backlight();     // Turn on the backlight.
  lcd.setCursor(0, 0);
  lcd.print("ggggg");


  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  Serial.begin(115200);
  Serial.println();
  Serial.print("Wifi connecting to ");
  Serial.println( ssid );

  WiFi.begin(ssid, password);

  Serial.println();

  Serial.print("Connecting");

  while ( WiFi.status() != WL_CONNECTED ) {
    delay(500);
    Serial.print(".");
  }

  digitalWrite( ledPin , HIGH);
  Serial.println();

  Serial.println("Wifi Connected Success!");
  Serial.print("NodeMCU IP Address : ");
  Serial.println(WiFi.localIP() );
  ;

  configTime(timezone, dst, "pool.ntp.org", "time.nist.gov");
  Serial.println("\nWaiting for Internet time");
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  while (!time(nullptr)) {
    Serial.print("*");
    delay(1000);
  }
  Serial.println("\nTime response....OK");

}

void loop() {
  show_time();
  
  alarm(H, M, c);
 
  firebase();
  if ( digitalRead(yes) == LOW) {
    Serial.print("yeeees");
  }
}
void show_time() {

  time_t now = time(nullptr);
  struct tm* p_tm = localtime(&now);
  Serial.print(p_tm->tm_mday);
  Serial.print("/");
  Serial.print(p_tm->tm_mon + 1);
  Serial.print("/");
  Serial.print(p_tm->tm_year + 1900);




  Serial.print(" ");

  Serial.print(p_tm->tm_hour);
  Serial.print(":");
  Serial.print(p_tm->tm_min);
  Serial.print(":");
  Serial.println(p_tm->tm_sec);

  delay(1000);


  lcd.setCursor(0, 0); // Move the cursor characters to the right and
  lcd.print(p_tm->tm_mday);
  lcd.print("/");
  lcd.print(p_tm->tm_mon + 1);
  lcd.print("/");
  lcd.print(p_tm->tm_year + 1900);




  lcd.print(" ");              // zero characters down (line 1).
  lcd.print(p_tm->tm_hour);
  lcd.print(":");
  lcd.print(p_tm->tm_min);
  lcd.print(":");
  lcd.println(p_tm->tm_sec);

}


void alarm(int a , int b , String c1) {

  time_t now = time(nullptr);
  struct tm* p_tm = localtime(&now);
  while (p_tm->tm_hour == a && p_tm->tm_min == b && arret == 0  ) {
    Buzz();
    
  Firebase.setInt("total",( Firebase.getInt("total"))+1);
    //******* ajout
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Medicine time");
    lcd.setCursor(0, 1);
    lcd.print(" YES/NO");

    Serial.print(c1);


    if ( digitalRead(yes) == LOW  && arret == 0) {
      reponse = "done:)";
      lcd.setCursor(0, 1);
      lcd.print("                                 ");

      lcd.setCursor(0, 1);
      lcd.print("                                        ");
      lcd.setCursor(4, 1);
      lcd.print("YES");
      Serial.print("YES");

      delay(3000);
      lcd.setCursor(4, 1);
      lcd.print("                                    ");
      arret = 1;
     // Firebase.setFloat("take_medecine",(Firebase.getFloat("take_medecine"))+1);
      take_medecine = take_medecine + 1;
      total = total + 1;

    }
    if (digitalRead(no) == LOW && arret == 0) {
      reponse = "not done";
      lcd.setCursor(0, 0);
      lcd.print("                                 ");

      lcd.setCursor(0, 1);
      lcd.print("                                        ");
      lcd.setCursor(1, 1);
      lcd.print("NO");
      delay(3000);
      lcd.setCursor(0, 1);
      lcd.print("           ");
      arret = 1;
      total = total + 1;
    }
  }
}


void Buzz() {
  analogWrite(buzzer, 1000); // Send 1KHz sound signal...
  delay(100);
  digitalWrite(buzzer, LOW);
}
void firebase() {

  //Firebase.setInt("heure", 11);
  //Firebase.setInt("minute",20);
  //Firebase.setString("nom d'alarme","rappel medicaments");
  Firebase.setString("reponse", reponse);

  // set bool value
 // Firebase.setInt("medecine", take_medecine);
  int heu = Firebase.getInt("total");
  H = Firebase.getInt("hour");
  M = Firebase.getInt("minute");
  c = Firebase.getString("subject"); 
 

  float statistique =  take_medecine/total*100 ;


 Firebase.setFloat("statistcs", statistique);
  Serial.print(H);
  Serial.print("    ");

  Serial.print(M);
  Serial.print("    ");

  Serial.print(c);
  
  Serial.print("    ");
  Serial.print(statistique);


  // handle error
  if (Firebase.failed()) {
    Serial.print("setting /send failed:");
    Serial.println(Firebase.error());
    return;
  }
  delay(1000);
}
