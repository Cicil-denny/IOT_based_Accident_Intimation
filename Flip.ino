#include <SoftwareSerial.h>
#include<Wire.h>
int Rx_pin = 9;
int Tx_pin = 8;
SoftwareSerial SerialGPS(Rx_pin, Tx_pin);
SoftwareSerial GSM(10, 11);
#define BUZ 7
#define LED 3
int button = 2; 
double roll;
#include <math.h>

int minVal = 265;
int maxVal = 402;

double x;
double y;
double z;


const int MPU_addr = 0x68;
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;


const int x_out = A1; /* connect x_out of module to A1 of UNO board */
const int y_out = A2; /* connect y_out of module to A2 of UNO board */
const int z_out = A3; /* connect z_out of module to A3 of UNO board */

void setup() {
  pinMode(button, INPUT); //Set the sensor pin as INPUT
  Serial.begin(9600);//Set baud rate for serial communication
  SerialGPS.begin(9600);
  GSM.begin(9600);   // Setting the baud rate of GSM Module
  delay(100);
  pinMode(BUZ, OUTPUT);

  
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);// PWR_MGMT_1 register
  Wire.write(0); // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true); Wire.begin();
  Serial.begin(9600);
  delay(1000);
}


void prntmpu()
{
  Serial.print(AcX);
  Serial.print("   ");
  Serial.print(AcY);
  Serial.print("   ");
  Serial.print(AcZ);
  Serial.print("   ");
  Serial.print(Tmp);
  Serial.print("   ");
  Serial.print(GyX);
  Serial.print("   ");
  Serial.print(GyY);
  Serial.print("   ");
  Serial.print(GyZ);
  Serial.println("   ");
  Serial.println("--------------------------------------------------");
  Serial.println(AcX+ AcY+ AcZ+ Tmp+ GyX+ GyY+ GyZ);
  Serial.println("-----------------------------------------------------");
}

void roll_check()
{

 
  int x = map(AcZ, minVal, maxVal, -90, 90);
  Serial.println(x);
  
  if ( AcZ<-1000)
  {
    Serial.println("asdf");
    roll = 150;
  } 
  else
    roll = 210;
}

void loop(){
  Serial.println("------------------------------------------------------------------------------------------");
  GetMpuValue1(MPU_addr);
//  prntmpu();
  roll_check();
//     
//  if (digitalRead(button) == LOW){
//    Serial.println("Collision");//print collision
//    delay(300);
////    SendMessage();
////    buzled();
//    delay(500);
////    exit(0);
//    }
//    axc();
    Serial.println(roll);
   
   if ((roll < 180)||(roll>240)){
    Serial.println("Car Flipped");//print collision
    delay(300);
    SendMessage();
    buzled();
    delay(500);
//    exit(0);
   }
   else{
    Serial.println("Car isnt Flipped");//print collision
   }
   delay(1000);
}


void buzled(){
  int i=0;
  while (i<2) {
    i+=1;
    delay(1000);
    digitalWrite(LED, HIGH);
    digitalWrite(BUZ, HIGH);
    delay(1000);
    digitalWrite(BUZ, LOW);
    digitalWrite(LED, LOW);}
    }

void SendMessage(){
  //GSM.println("ATD8825413527");
  //delay(300);
  GSM.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(300);
  GSM.println("AT+CMGS=\"8248469940\"\r");
  delay(300);
    GSM.println("!!!! There has been as car accident at");// The SMS text you want to send
    delay(300);
    gps();
    delay(300);
    GSM.println("----------Details of the passenger----------");
    delay(300);
    GSM.println("Name: ");
    delay(300);
    GSM.println("Blood Group: ");
    delay(300);
    GSM.println("Address: ");
    delay(300);
    GSM.println("Family phone Number: ");
    delay(300);
    GSM.println("Medical Conditions: ");
    delay(300);
    GSM.println("--------Vehicle Details-------");
    delay(300);
    GSM.println("Car license Number:");
    delay(300);
    GSM.println("Car color:");
    if ((roll < 180)||(roll>240)) {
      GSM.println("EMERGENCY: The Car is flipped, call the medical authority and a rescue team immediately to save the person from the car");
      Serial.println("EMERGENCY: The Car is flipped, call the medical authority and a rescue team immediately to save the person from the car");}
    else if((roll > 180)||(roll<240)) {
      GSM.println("The Car isnt flipped");
      Serial.println("The Car isnt flipped");}
    
    delay(300);
    GSM.println("!!!!Kindly inform the authorized person to take further actions to save a life!!!! ");
    delay(300);
    GSM.println((char)26);// ASCII code of CTRL+Z
  
}


void GetMpuValue1(const int MPU) {

  Wire.beginTransmission(MPU);
  Wire.write(0x3B); // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 14, true); // request a total of 14 registers

  AcX = Wire.read() << 8 | Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
  AcY = Wire.read() << 8 |  Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ = Wire.read() << 8 | Wire.read(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)

  Tmp = Wire.read() << 8 | Wire.read(); // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)

  int xAng = map(AcX, minVal, maxVal, -90, 90);
  int yAng = map(AcY, minVal, maxVal, -90, 90);
  int zAng = map(AcZ, minVal, maxVal, -90, 90);

  GyX = Wire.read() << 8 | Wire.read(); // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY = Wire.read() << 8 | Wire.read(); // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ = Wire.read() << 8 | Wire.read(); // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)

  x = RAD_TO_DEG * (atan2(-yAng, -zAng) + PI) + 4; //offset by 4 degrees to get back to zero
  y = RAD_TO_DEG * (atan2(-xAng, -zAng) + PI);
  z = RAD_TO_DEG * (atan2(-yAng, -xAng) + PI);

}


void gps(){
  GSM.println("Accident Location: 10.9027N, 76.9006E");
  while(false){
  while (SerialGPS.available() > 0)
  Serial.write(SerialGPS.read());
  }
  
}
