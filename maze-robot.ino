#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include<math.h>
Adafruit_MPU6050 mpu;

int leftecho = 9;
int midecho = 5;
int rightecho = 3;

int leftrig = 8;
int midtrig = 4;
int righttrig = 2;

long duration;
int distance;
//right motor
int motorR1=12;//forward
int motorR2=13;
int ENA=6;
//left motor 
int motorL1=11;//forward
int motorL2=7;
int ENB=10;


float Kp=7;
float Kd=0.03;
float Ki=0.1;

unsigned long newtime = 0;
unsigned long oldtime = 0;
float dt = 0;
float error ;
float lasterror ;
float I;
float D;
int correction ;

int rightspeed;
int leftspeed;

int target;
double angle = 0;

int basespeed=100;
double baiz=-0.019687;




void setup() {
  Serial.begin(115200);

  pinMode(leftecho, INPUT);
  pinMode(midecho, INPUT);
  pinMode(rightecho, INPUT);

  pinMode(leftrig, OUTPUT);
  pinMode(midtrig, OUTPUT);
  pinMode(righttrig, OUTPUT);

  //right motor
  pinMode(motorR1,OUTPUT);
  pinMode(motorR2,OUTPUT);
  pinMode(ENA,OUTPUT);
  //left motor
  pinMode(motorL1,OUTPUT);
  pinMode(motorL2,OUTPUT);
  pinMode(ENB,OUTPUT);

  Wire.begin();
  

  if(!mpu.begin()){
    Serial.println("mpu not found");
    while(1){
      delay(10);
    }
  }
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ); 
  Serial.println("START");
  Serial.println("mpu is ready.");
  oldtime = micros();
  angle = 0;

}

void loop() {
  target =90;
  newtime = micros();
  dt = (newtime - oldtime) / 1000000.0;
  oldtime = newtime;
  I+=error*dt ;
  D = error/dt ;

  sensors_event_t a, g, t;
  mpu.getEvent(&a, &g, &t);

  angle += (g.gyro.z-baiz) * dt;
  float degreez = (angle * 180.0 / PI) ;
  error =target - degreez;
  correction =Kp*error+Kd*D+Ki*I;

  rightspeed =basespeed -correction;
  leftspeed =basespeed +correction ;
 
  rightspeed = constrain(rightspeed, 0, 120);
  leftspeed = constrain(leftspeed, 0, 120);
  


  if (abs(error) <= 5) {

    motormove(0, 0, LOW, LOW, LOW, LOW);


  }
  else if (degreez < target) {

    motormove(rightspeed, leftspeed, HIGH, LOW, LOW, HIGH);

  }
  else if (degreez > target ){
    motormove(rightspeed, leftspeed, LOW, HIGH, HIGH, LOW);

  }
  else {
      motormove(rightspeed, leftspeed, HIGH, LOW, LOW, HIGH);
  }



Serial.print("gyroZ: ");
Serial.print(g.gyro.z);

Serial.print("   degree: ");
Serial.println(degreez);


}

void motormove(int RightSpeed, int LeftSpeed, int Rightforward, int Rightbackward, int Leftforward, int Leftbackward) {

  digitalWrite(motorR1, Rightforward);
  digitalWrite(motorR2, Rightbackward);

  digitalWrite(motorL1, Leftforward);
  digitalWrite(motorL2, Leftbackward);

  analogWrite(ENA, RightSpeed);
  analogWrite(ENB, LeftSpeed);
}

