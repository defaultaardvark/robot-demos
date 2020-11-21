#include <AFMotor.h>

//Potentiometer Analog Pins
int potPin1 = 7;
int potPin2 = 9;
int potPin3 = 11;
int potPin4 = 13;
int potPin5 = 15;

//Potentiometer Analog Values
int potVal1 = 0;
int potVal2 = 0;
int potVal3 = 0;
int potVal4 = 0;
int potVal5 = 0;

const int ledPin = 53;
const int motorSpeed = 255;

int movementStage1 = 0;
int movementStage2 = 0;
int calibrationStage = 0;

int pot1Min = 212;
int pot1Max = 230;
int pot3Min = 25;
int pot3Max = 40;
int pot4Min = 126;
int pot4Max = 140;
int pot5Min = 175;
int pot5Max = 200; //Previously 228

int pot1Cal = 225;
int pot3Cal = 35;
int pot4Cal = 140;
int pot5Cal = 175;

int targetMultiplier = 0.1;
int camRes[] = {640, 480};
int camMidX = camRes[0]/2;
int camMidY = camRes[1]/2;
int minRangeX = camMidX - (camRes[0] * targetMultiplier)/2;
int maxRangeX = camMidX + (camRes[0] * targetMultiplier)/2;
int minRangeY = camMidY - (camRes[1] * targetMultiplier)/2;
int maxRangeY = camMidY + (camRes[1] * targetMultiplier)/2;

String stringX;
String stringY;

int centerX = 0;
int centerY = 0;

int intLen = 6;
String serIn;
int delimIndex;

AF_DCMotor M1(1, MOTOR12_1KHZ);
AF_DCMotor M3(2, MOTOR12_1KHZ);
AF_DCMotor M4(3, MOTOR34_1KHZ);
AF_DCMotor M5(4, MOTOR34_1KHZ);

void setup() {
  Serial.begin(19200);
  while(!Serial){
    Serial.println("Start");
  }

  pinMode(ledPin, OUTPUT);

  M1.setSpeed(motorSpeed);
  M3.setSpeed(motorSpeed);
  M4.setSpeed(motorSpeed);
  M5.setSpeed(motorSpeed);

  potVal1 = analogRead(potPin1);
  potVal3 = analogRead(potPin3);
  potVal4 = analogRead(potPin4);
  potVal5 = analogRead(potPin5);
  potVal1 = map(potVal1, 0, 1023, 0, 270);
  potVal3 = map(potVal3, 0, 1023, 0, 270);
  potVal4 = map(potVal4, 0, 1023, 0, 270);
  potVal5 = map(potVal5, 0, 1023, 0, 270);

  calibrationStage++;
  // movementStage1++;
}

//-----------------Function Declaration-----------------//
void robotCalibration();
void lockOn();

void loop() {
  robotCalibration();

  switch(movementStage1){
    case 1:
      lockOn();
      break;
  }
//-----------------Serial Reading----------------//

  if (Serial.available() > 0){
    if (Serial.read() == 'X'){
      centerX = Serial.parseInt();
      if (Serial.read() == 'Y'){
        centerY = Serial.parseInt();
        lockOn();
      }
    }
  }
  while (Serial.available() > 0){
    Serial.read();
  }

//-----------------Analog Pin Update-----------------//

  potVal1 = analogRead(potPin1);
  potVal3 = analogRead(potPin3);
  potVal4 = analogRead(potPin4);
  potVal5 = analogRead(potPin5);
  potVal1 = map(potVal1, 0, 1023, 0, 270);
  potVal3 = map(potVal3, 0, 1023, 0, 270);
  potVal4 = map(potVal4, 0, 1023, 0, 270);
  potVal5 = map(potVal5, 0, 1023, 0, 270);

//  Serial.print("M1: ");
//  Serial.print(potVal1);
//  Serial.print(", M3: ");
//  Serial.print(potVal3);
//  Serial.print(", M4: ");
//  Serial.print(potVal4);
//  Serial.print(", M5: ");
//  Serial.print(potVal5);
//  Serial.println();
}

void robotCalibration(){
  switch(calibrationStage){
    case 1:
      if(potVal1 > pot1Cal){
          M1.run(FORWARD); //Close Claw
      }
      else if(potVal1 < pot1Cal){
          M1.run(BACKWARD); //Open Claw
      }
      else if(potVal1 == pot1Cal){
          M1.run(RELEASE);
          calibrationStage++;
      }
    break;
    case 2:
      if(potVal3 > pot3Cal){
          M3.run(FORWARD);  //Forward
      }
      else if(potVal3 < pot3Cal){
          M3.run(BACKWARD); //Backward    
      }
      if(potVal3 == pot3Cal){
          M3.run(RELEASE);
          calibrationStage++;
      }
      break;
    case 3:
      if(potVal4 > pot4Cal){
          M4.run(BACKWARD); //Forward    
      }
      else if(potVal4 < pot4Cal){
          M4.run(FORWARD); //Backward
      }
      else if(potVal4 == pot4Cal){
          M4.run(RELEASE);
          calibrationStage++;
      }
      break;
    case 4:
      if(potVal5 > pot5Cal){
          M5.run(FORWARD); //CCW
      }
      else if(potVal5 < pot5Cal){
          M5.run(BACKWARD); //CW    
      }
      else if(potVal5 == pot5Cal){
          M5.run(RELEASE);
          calibrationStage++;
      }
      break;
    case 5:
      M1.run(RELEASE);
      M3.run(RELEASE);
      M4.run(RELEASE);
      M5.run(RELEASE);
      calibrationStage++;
      digitalWrite(ledPin, HIGH);
      delay(500);
      digitalWrite(ledPin, LOW);
      delay(500);
      break;
    default:
      if (centerX != 0 && centerY != 0){
        movementStage1++;
      }
      break;
  }
}

void lockOn(){
  if (centerX > maxRangeX){
    M5.run(BACKWARD);
  }
  else if (centerX < minRangeX){
    M5.run(FORWARD);
  }
  else {
    M5.run(RELEASE);
  }

  if (centerY > maxRangeY){
    M3.run(FORWARD);
  }
  else if (centerY < minRangeY){
    M3.run(BACKWARD);
  }
  else if (potVal3 <= pot3Cal && centerY > maxRangeY){
    M4.run(FORWARD);
  }
  else if (potVal4 <= pot4Cal && potVal3 <= pot3Cal && centerY < minRangeY){
    M4.run(BACKWARD);
  }
  else {
    M3.run(RELEASE);
    M4.run(RELEASE);
  }

  if (centerX < maxRangeX && centerX > minRangeX){
    if (centerY < maxRangeY && centerY > minRangeY){
      // digitalWrite(ledPin, HIGH);
      M1.run(RELEASE);
      M3.run(RELEASE);
      M4.run(RELEASE);
      M5.run(RELEASE);
    }
  }

  // if (centerX < maxRangeX && centerX > minRangeX){
  //   if (centerY < maxRangeY && centerY > minRangeY){
  //     digitalWrite(ledPin, HIGH);
  //   }
  // }
  // else {
  //   digitalWrite(ledPin, LOW);
  // }
}