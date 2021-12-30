 // Libraries
#include <Wire.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h> 

#define buzzerPin 4
#define servoPin 5
#define trigPin 6
#define echoPin 7
#define RST_PIN 9 
#define SS_PIN 10

// Variables
struct member{
  String id;
  String nama;
  int umur;
  bool stat;
};
struct member objectMember[4];

byte readCard[4];
String cardID = "";
int jumlahMember = sizeof(objectMember)/sizeof(objectMember[0]);
int jumlahMemberDiRuangan = 0;

LiquidCrystal_I2C lcd(0x27, 16, 2); 
MFRC522 mfrc522(SS_PIN, RST_PIN);
Servo servo;

void setup(){
    objectMember[0].id = "184164139";
    objectMember[0].nama = "John";
    objectMember[0].umur = 21;
    objectMember[0].stat = false;
  
    objectMember[1].id = "49170234";
    objectMember[1].nama = "Fitri";
    objectMember[1].umur = 22;
    objectMember[1].stat = false;

    objectMember[2].id = "453112114";
    objectMember[2].nama = "Ariq";
    objectMember[2].umur = 21;
    objectMember[2].stat = false;

    objectMember[3].id = "4116134106";
    objectMember[3].nama = "Dita";
    objectMember[3].umur = 21;
    objectMember[3].stat = false;
    
    Serial.begin(9600);
    lcd.init();
    lcd.backlight();
    printLCD("Scan kartu...", 0);
    SPI.begin(); 
    mfrc522.PCD_Init(); 
    delay(4); 
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    pinMode(buzzerPin, OUTPUT);
    servo.attach(5);
    servo.write(150);
} 

void loop(){
    if (!mfrc522.PICC_IsNewCardPresent()) { 
      return 0;
    }
   
    if (!mfrc522.PICC_ReadCardSerial()) {
      return 0; 
    }
    
    lcd.clear();
    printLCD("Scanned UID", 0);
    Serial.println(F("Scanned PICC's UID:"));

    // Print ID kartu ke LCD
    byte cur = 0;
    for (byte i = 0; i < 4; i++) {
      readCard[i] = mfrc522.uid.uidByte[i]; 
      Serial.print(readCard[i]);
      Serial.print(" ");
      lcd.setCursor(cur, 1); 
      lcd.print(readCard[i]); 
      lcd.print(" ");
      cardID += (String)readCard[i];
      delay(250); 
      cur += 3;
    }

    // Cek status kartu aktif atau tidak
    for(byte i = 0; i < jumlahMember; i++){
       if(cardID == objectMember[i].id){
         if(objectMember[i].stat == true){
           objectMember[i].stat = false;
           jumlahMemberDiRuangan--;
           
           lcd.clear();
           printLCD("Sampai berjumpa", 0);
           printLCD("lagi " + objectMember[i].nama + " !", 1);
           Serial.println("Sampai berjumpa lagi!");
           delay(1000);
           break;
         } else {
           // Cek isi ruangan sudah melebihi batas atau tidak
           if(jumlahMemberDiRuangan >= jumlahMember / 2){
             buzzerOn();
             lcd.clear(); 
             printLCD("Ruangan Penuh!", 0);
             Serial.println("Ruangan Penuh!");
             delay(1000);
             break;
           } else {
             objectMember[i].stat = true;
             jumlahMemberDiRuangan++; 
             
             lcd.clear(); 
             printLCD("Selamat datang", 0);
             printLCD(objectMember[i].nama + " " + objectMember[i].umur, 1);
             delay(1000);
              
             printLCD("Silahkan Masuk ", 0);
             printLCD("Isi ruangan ", 1);
             lcd.print(jumlahMemberDiRuangan - 1);
             Serial.println("Silahkan Masuk");
              
             openServo();

             while(true){
               Serial.println("Waiting...");
               if(checkIfCustomerHaveEntered()){
                break;
               }
             }
             break;
           }
         }
       } else {
         if(i == jumlahMember - 1){
           buzzerOn();
           lcd.clear();
           printLCD("Daftar dulu gan", 0);
           Serial.println("Daftar dulu gan");
           delay(2000);
           break;
         }
         continue;
       } 
    }
      
    cardID = "";
    Serial.println("");
    lcd.clear();
    printLCD("Scan kartu...", 0);
} 



// Functions
void openServo(){
    for (int degree = 150; degree >= 1; degree--){                              
     servo.write(degree);
     delay(5);                                         
    }
}

void closeServo(){
    for (int degree = 1; degree < 150; degree++){                              
     servo.write(degree);                                        
     delay(5);
    }
}

void printLCD(String message, int line){
    lcd.setCursor(0,line);
    lcd.print(message);
}

void buzzerOn(){
  int delayTime = 100;
  digitalWrite(buzzerPin, HIGH);
  delay(delayTime);
  digitalWrite(buzzerPin, LOW);
  delay(delayTime);
  digitalWrite(buzzerPin, HIGH);
  delay(delayTime);
  digitalWrite(buzzerPin, LOW);
  delay(delayTime);
  digitalWrite(buzzerPin, HIGH);
  delay(delayTime);
  digitalWrite(buzzerPin, LOW);
  delay(delayTime);
  digitalWrite(buzzerPin, HIGH);
  delay(delayTime);
  digitalWrite(buzzerPin, LOW);
  delay(delayTime);
}

bool checkIfCustomerHaveEntered(){
    float duration, distance;
    digitalWrite(trigPin,LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance = duration * 0.034 / 2;
  
    if (distance < 5){
      Serial.println(distance);
      closeServo();
      Serial.println("Sudah masuk");
      return true;
    }

    return false;
}
