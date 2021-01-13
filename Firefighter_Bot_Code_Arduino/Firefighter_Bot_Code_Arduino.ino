#include <LiquidCrystal.h>  //Includes the Liquid Crystal library for the LCD

#include <Servo.h>  //Includes the Servo motor library

const byte rs = 13, en = 12, d4 = 19, d5 = 18, d6 = 17, d7 = 16;    //sets pins for LCD
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);   //more pins for LCD

Servo SServo;     //creates servo object (basically naming your servos and differentiating them)
Servo FServo;     

//Declaration of variables
byte const BUTTON = 0;
byte const Flame = 0;
byte const Lspeed = 5;
byte const Rspeed = 6;
byte const LMotora = 1;
byte const LMotorb = 7;
byte const RMotora = 8;
byte const RMotorb = 4;
byte const LEncoder = 2;
byte const REncoder = 3;
byte const LLinesense = 10;
byte const RLinesense = 11;
byte const sensorpin = 0;                 // analog pin used to connect the sharp sensor

//Sets start distances for the SHARP sensor
int LDis = 0;
int CDis = 0;
int RDis = 0;

//Encoder control
volatile long LEncoderTicks = 0;
volatile long REncoderTicks = 0;
int diff = 0;
byte speedL = 110;
byte speedR = 110;
bool LTurn = false;
bool InRoom = false;
int forward = 0;

//Counts the number of ticks that the left wheel turns. Adds one with each voltage change
void countLeft() {
  LEncoderTicks++;
}

//Counts the number of ticks that the right wheel turns. Adds one with each voltage change
void countRight() {
  REncoderTicks++;
}

//Inputs and Outputs
void setup() {
  pinMode(BUTTON, INPUT);
  pinMode(Lspeed, OUTPUT);
  pinMode(Rspeed, OUTPUT);
  pinMode(LMotora, OUTPUT);
  pinMode(LMotorb, OUTPUT);
  pinMode(RMotora, OUTPUT);
  pinMode(RMotorb, OUTPUT);
  pinMode(LEncoder, INPUT);
  pinMode(REncoder, INPUT);
  pinMode(LLinesense, INPUT);
  pinMode(RLinesense, INPUT);
  lcd.begin(16, 2);   //sets up the columns and rows on the LCD
  SServo.attach(15);  //Attaches the sensor servo to pin 15
  FServo.attach(9);   //Attaches the flame extinguishing servo to pin 9

  FServo.write(100);  //Sets start position at 100

  attachInterrupt(digitalPinToInterrupt(LEncoder), countLeft, CHANGE);  //Interrupts for the encoders
  attachInterrupt(digitalPinToInterrupt(REncoder), countRight, CHANGE);
}

//Flame extinguishing function
void extinguish(){
  forward = 0;
  while(digitalRead(Flame) == LOW){   //while the flame detector sees the flame, the bot will swing the sponge arm and get closer until the flame is extinguished
      FServo.write(110);
      delay(200);
      FServo.write(90);
      delay(200);
      FServo.write(100);
      straight(2);
      forward += 2;
    }
}

//
void check(){
  LEncoderTicks = 0;
  REncoderTicks = 0;
  analogWrite(Lspeed, 80);
  analogWrite(Rspeed, 80);
  digitalWrite(LMotora, HIGH);
  digitalWrite(LMotorb, LOW);
  digitalWrite(RMotora, LOW);
  digitalWrite(RMotorb, HIGH);
  while(LEncoderTicks<2){
    
  }
  digitalWrite(LMotora, LOW);
  digitalWrite(LMotorb, LOW);
  digitalWrite(RMotora, LOW);
  digitalWrite(RMotorb, LOW);
}

//When the robot enters the room
void room(){
  
  if(digitalRead(Flame) == LOW){
    FServo.write(90);
    delay(300);
    FServo.write(100);
    extinguish();
    backup(100);
    if(LTurn == true){
      FServo.write(110);
      delay(300);
      FServo.write(100);
      turnright();
      delay(100);
      straight(10);
      return;
    }
    else{
      FServo.write(110);
      delay(300);
      FServo.write(100);
      turnleft();
      delay(100);
      straight(10);
      return;
    }
  }
  else{
    straight(15);
    FServo.write(90);
    delay(300);
    FServo.write(100);
    extinguish();
    FServo.write(110);
    delay(300);
    FServo.write(100);
    delay(250);
    lcd.clear();
    for(int x=0; x<24; x++){
      check();
      if(digitalRead(Flame) == LOW){
        FServo.write(90);
        delay(300);
        lcd.print(x);
        FServo.write(100);
        extinguish();
        for(forward>1; forward -= 2;){
          backup(2);
          delay(100);
        }
      }
      lcd.setCursor(0, 0);
      lcd.print(x);
      delay(250);
    }
    delay(500);
    backup(100);
    if(LTurn == true){
      turnright();
      delay(100);
      straight(14);
      return;
    }
    else{
      turnleft();
      delay(100);
      straight(14);
      return;
    } 
  }
}



//The robot will square up, using the white line each time it enters and exits a room
void square () {
  if(digitalRead(LLinesense) == LOW || digitalRead(RLinesense) == LOW){
    digitalWrite(LMotora, LOW);
    digitalWrite(LMotorb, LOW);
    digitalWrite(RMotora, LOW);
    digitalWrite(RMotorb, LOW);
    lcd.clear();
    lcd.print("stop");
    delay(500);
    digitalWrite(LMotora, HIGH);
    digitalWrite(LMotorb, LOW);
    digitalWrite(RMotora, HIGH);
    digitalWrite(RMotorb, LOW);
    delay(500);
    digitalWrite(LMotora, LOW);
    digitalWrite(LMotorb, LOW);
    digitalWrite(RMotora, LOW);
    digitalWrite(RMotorb, LOW);
    delay(500);
    analogWrite(Lspeed, 70);
    analogWrite(Rspeed, 70);
    digitalWrite(LMotora, LOW);
    digitalWrite(LMotorb, HIGH);
    digitalWrite(RMotora, LOW);
    digitalWrite(RMotorb, HIGH);
    while(true){
      if(digitalRead(LLinesense) == LOW && digitalRead(RLinesense) == LOW){
        return;
      }
      if(digitalRead(LLinesense) == LOW){     //If the line sensor goes low in the left side, it stops, backs up, drives forward until it hits it again, stops the left motor and brings the right wheel around until the right sensor goes off and then proceeds into or out of the room
        analogWrite(Rspeed, 100);
        digitalWrite(LMotora, LOW);
        digitalWrite(LMotorb, LOW);
        digitalWrite(RMotora, LOW);
        digitalWrite(RMotorb, HIGH);
        lcd.print(" Left");
        while(digitalRead(RLinesense) == HIGH){
          
        }
        
        digitalWrite(RMotorb, LOW);
        
        return;
      }
      if(digitalRead(RLinesense) == LOW){     //If the line sensor goes low in the right side, it stops, backs up, drives forward until it hits it again, stops the right motor and brings the left wheel around until the left sensor goes off and then proceeds into or out of the room
        analogWrite(Lspeed, 100);
        digitalWrite(LMotora, LOW);
        digitalWrite(LMotorb, HIGH);
        digitalWrite(RMotora, LOW);
        digitalWrite(RMotorb, LOW);
        lcd.print(" Right");
        while(digitalRead(LLinesense) == HIGH){
          
        }
        
        digitalWrite(LMotorb, LOW);
        
        return;
      }
    }
  } 
}

//This is the general backup function but when the robot is backing out of the room, it’ll square itself and then leave
void backup (byte tdis){
  LEncoderTicks = 0;
  REncoderTicks = 0;
  digitalWrite(LMotora, HIGH);
  digitalWrite(LMotorb, LOW);
  digitalWrite(RMotora, HIGH);
  digitalWrite(RMotorb, LOW);
  analogWrite(Lspeed, speedL);
  analogWrite(Rspeed, speedR);
  unsigned long numTickL;
  unsigned long numTickR;
  int TTicks = 0;

  while(true){
    
    TTicks += LEncoderTicks;
    lcd.setCursor(8,0);
    lcd.print(TTicks);
    lcd.print("hi");
    if(TTicks >= tdis){
      digitalWrite(LMotora, LOW);
      digitalWrite(LMotorb, LOW);
      digitalWrite(RMotora, LOW);
      digitalWrite(RMotorb, LOW);
      return;
    }
    numTickL = LEncoderTicks;
    numTickR = REncoderTicks;
  
    diff = numTickL - numTickR;

    lcd.setCursor(0,0);
    
    lcd.print(diff);

    if(diff>5){
      speedL -= 1;
      speedR += 1;
    }
    if(diff<-5){
      speedL += 1;
      speedR -= 1;
    }

   
    analogWrite(Lspeed, speedL);
    analogWrite(Rspeed, speedR);
    LEncoderTicks = 0;
    REncoderTicks = 0;
    for(int x=0; x < 100; x++){
      if(digitalRead(LLinesense) == LOW || digitalRead(RLinesense) == LOW){
        square();
        delay(500);
        analogWrite(Lspeed, speedL);
        analogWrite(Rspeed, speedR);
        digitalWrite(LMotora, HIGH);
        digitalWrite(LMotorb, LOW);
        digitalWrite(RMotora, HIGH);
        digitalWrite(RMotorb, LOW);
        delay(500);
        digitalWrite(LMotora, LOW);
        digitalWrite(LMotorb, LOW);
        digitalWrite(RMotora, LOW);
        digitalWrite(RMotorb, LOW);
        lcd.clear();
        lcd.print("outofroom");
        InRoom = false;
        delay(500);
        return;
      }
      delay(1);
    }
  }
}

void straight(byte tdis){
  //drive straight
  LEncoderTicks = 0;
  REncoderTicks = 0;
  digitalWrite(LMotora, LOW);
  digitalWrite(LMotorb, HIGH);
  digitalWrite(RMotora, LOW);
  digitalWrite(RMotorb, HIGH);
  analogWrite(Lspeed, speedL);
  analogWrite(Rspeed, speedR);
  unsigned long numTickL;
  unsigned long numTickR;
  int TTicks = 0;

  while(true){
    
    TTicks += LEncoderTicks;
    lcd.setCursor(8,0);
    lcd.print(TTicks);
    if(TTicks >= tdis){
      digitalWrite(LMotora, LOW);
      digitalWrite(LMotorb, LOW);
      digitalWrite(RMotora, LOW);
      digitalWrite(RMotorb, LOW);
      return;
    }
    numTickL = LEncoderTicks;
    numTickR = REncoderTicks;
  
    diff = numTickL - numTickR;

    lcd.setCursor(0,0);
    
    lcd.print(diff);

    if(diff>5){   //Here, the encoders are looking for a difference in ticks. If found, it will adjust the speed of the motors accordingly so that the robot can now drive straight
      speedL -= 1;
      speedR += 1;
    }
    if(diff<-5){
      speedL += 1;
      speedR -= 1;
    }

   
    analogWrite(Lspeed, speedL);
    analogWrite(Rspeed, speedR);
    LEncoderTicks = 0;
    REncoderTicks = 0;
    for(int x=0; x < 100; x++){
      if(digitalRead(LLinesense) == LOW || digitalRead(RLinesense) == LOW){
        square();
        delay(500);
        analogWrite(Lspeed, speedL);
        analogWrite(Rspeed, speedR);
        digitalWrite(LMotora, LOW);
        digitalWrite(LMotorb, HIGH);
        digitalWrite(RMotora, LOW);
        digitalWrite(RMotorb, HIGH);
        delay(500);
        digitalWrite(LMotora, LOW);
        digitalWrite(LMotorb, LOW);
        digitalWrite(RMotora, LOW);
        digitalWrite(RMotorb, LOW);
        lcd.clear();
        lcd.print("done");
        InRoom = true;  //If it’s determined that the bot is in the room, it’ll proceed to the room function

        
        room();
        return;
      }
      delay(1);
    }
  }
}

void turnleft() {
  //turn left
  lcd.clear();
  if(InRoom != true){
    LTurn = true;
  }
  for(int x = 0; x<6; x++){
    LEncoderTicks = 0;
    REncoderTicks = 0;
    digitalWrite(LMotora, HIGH);
    digitalWrite(LMotorb, LOW);
    digitalWrite(RMotora, LOW);
    digitalWrite(RMotorb, HIGH);
    analogWrite(Lspeed, 80);
    analogWrite(Rspeed, 80);
    while(LEncoderTicks<2){
    
    
    }
    digitalWrite(LMotora, LOW);
    digitalWrite(LMotorb, LOW);
    digitalWrite(RMotora, LOW);
    digitalWrite(RMotorb, LOW);
    lcd.setCursor(0, 0);
    lcd.print(x);
    delay(200);
  }
}

void turnright() {
  //turn right
  lcd.clear();
  if(InRoom != true){
    LTurn = false;
  }
  for(int x = 0; x<6; x++){
    LEncoderTicks = 0;
    REncoderTicks = 0;
    digitalWrite(LMotora, LOW);
    digitalWrite(LMotorb, HIGH);
    digitalWrite(RMotora, HIGH);
    digitalWrite(RMotorb, LOW);
    analogWrite(Lspeed, 80);
    analogWrite(Rspeed, 80);
    while(REncoderTicks<2){
    
    }
    digitalWrite(LMotora, LOW);
    digitalWrite(LMotorb, LOW);
    digitalWrite(RMotora, LOW);
    digitalWrite(RMotorb, LOW);
    lcd.setCursor(0, 0);
    lcd.print(x);
    delay(200);
  }
}

void loop() {
  lcd.print("wait");
//Waiting for the button press
  while (true) {
    digitalRead(BUTTON);
      if (digitalRead(BUTTON) == LOW){
        delay(5); //contact bounce
       while (digitalRead(BUTTON) == LOW){
        //delay(5000);
        }
      break;}
  }
  lcd.clear();
  
  //The Robot reads the distance from it to the walls to the left, center and right and then prints it to the serial monitor
  while(true){
    lcd.clear();
    SServo.write(0);
    delay(400);
    LDis = analogRead(sensorpin);       // reads the value of the sharp sensor
    lcd.setCursor(0, 1);            // prints the value of the sensor to the serial monitor
    lcd.print(LDis);
       
    SServo.write(80);
    delay(400);
    CDis = analogRead(sensorpin);       // reads the value of the sharp sensor
    lcd.setCursor(4, 1);            // prints the value of the sensor to the serial monitor
    lcd.print(CDis);
    
    SServo.write(165);
    delay(400);
    RDis = analogRead(sensorpin);       // reads the value of the sharp sensor
    lcd.setCursor(8, 1);            // prints the value of the sensor to the serial monitor
    lcd.print(RDis);
    
    if(RDis < 125){
      lcd.setCursor(0, 0);
      lcd.print("Room Right");
      if(CDis < 300){
        straight(6);
        lcd.print("forward");   //If there’s enough room, the bot will move forward
      }
      if(CDis > 500){
        backup(4);
        lcd.print("backup");  //If there’s a wall really close to the bot, it’ll backup to create room
      }
      delay(200);
      turnright();
      straight(18);
    }
    else if(LDis < 125){
      lcd.setCursor(0, 0);
      lcd.print("Room Left");
      if(CDis < 300){
        straight(6);
        lcd.print("forward");   //If there’s enough room, the bot will move forward
      }
      if(CDis > 500){
        backup(4);
        lcd.print("backup");  //If there’s a wall really close to the bot, it’ll backup to create room
      }
      delay(200);
      turnleft();
      straight(18);
    }
    else{
      straight(22);
      
    }
  }
}
