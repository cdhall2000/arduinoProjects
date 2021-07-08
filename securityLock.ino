/*
Simple 4-button locking system
Written by Conor Hall


*/

#include <EEPROM.h>
//Function Declaration
void Read();
void shifter();
void reLock();

//Variables
//Arrays
int key[4] = {1,1,1,1}; //Entered buttons buffer
int pass[4] = {1,2,2,1}; //Default password
int port[4] = {2,3,4,5}; //Input I/O Pins
int control[3] = {12,9,8}; //Reset Pass, Output, Output_b

//Logic Variables
bool set = false;
bool triggered = false;
bool rst = false;
int reset = 0;

int address = 0;
byte value;

void setup() {

  Serial.begin(9600);
  while (!Serial) { ; }
  
  //Set input pinMode()
  for (int i = 0; i < (sizeof(port)/sizeof(port[0])); i++) {
    pass[i] = EEPROM.read(i);
    Serial.println(pass[i]);
    pinMode(port[i], INPUT_PULLUP);  
  }
  
  pinMode(12, INPUT_PULLUP);

  pinMode(9, OUTPUT); //Main Output
  pinMode(8, OUTPUT); //Alternate Output
  digitalWrite(8, HIGH);
  
  Serial.println("Initiated buttons...");
}


void loop() {
  // put your main code here, to run repeatedly:

  //Read Input
  Read();
  //Check for Successful Input
  if (key[0] == pass[0] && key[1] == pass[1] && key[2] == pass[2] && key[3] == pass[3] && triggered == false) {
    digitalWrite(9, HIGH); 
    digitalWrite(8, LOW); 
    Serial.println("Unlocked..."); 
    key[0] = 5; 
    delay(400); 
    digitalWrite(9, LOW); 
    triggered = true; 
    rst = false; 
  }
  //Detect No Input
  if (digitalRead(port[3]) == HIGH && digitalRead(port[2]) == HIGH && digitalRead(port[1]) == HIGH && digitalRead(port[0]) == HIGH) { set = false; }

  //Reset Passcode
  if (digitalRead(12) == LOW && !rst) {
    pass[0] = key[0];
    pass[1] = key[1];
    pass[2] = key[2];
    pass[3] = key[3];

    EEPROM.write(0, pass[3]);
    EEPROM.write(1, pass[2]);
    EEPROM.write(2, pass[1]);
    EEPROM.write(3, pass[0]);
    
    key[0] = 5;
    for (int i = 0; i < 50; i++){
      analogWrite(9, i);
      delay(15);
    }
    digitalWrite(9, LOW);
    rst = true;
    Serial.println("Reset Password...");
  }

  if (triggered) {
    reset++;
    delay(30);
    if (digitalRead(7) == LOW) {Serial.println(reset);}
    if (reset > 100 || digitalRead(7) == LOW) {
      reset = 0;
      triggered = false;
      reLock();
    }
  }
  
  delay(30);
  
}

void Read() {

  for (int i = 0; i < (sizeof(port)/sizeof(port[0])); i++) {
    if (digitalRead(port[i]) == LOW && set == false) { shifter(); key[0] = (i+1); set = true; Serial.print(i+1); Serial.println("..."); keyPress();}
  }
  
}


void shifter(){
  key[3] = key[2];
  key[2] = key[1];
  key[1] = key[0];
  key[0] = 0;  
}

void keyPress() {
  analogWrite(9, 10);
  delay(100);
  digitalWrite(9, LOW);
}

void reLock() {
  digitalWrite(8, HIGH);
      Serial.println("Re-Locked...");
      keyPress();
      delay(50);
      keyPress();
      delay(50);
      keyPress();
}

