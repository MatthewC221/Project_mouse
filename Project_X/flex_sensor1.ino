/*
const int FLEX_PIN_1 = A1;      //PIN THATS CONNECTED TO THE VOLTAGE DIVIDER OUTPUT
const int FLEX_PIN_2 = A2

const float VCC_1 = 4.71;       // MEASURE voltage at 5 Volts
const float RES_1 = 7.68;    // MEASURE RESISTANCE AT RESISTOR

const float VCC_2 = 4.71;       // MEASURE voltage at 5 Volts
const float RES_2 = 875;    // MEASURE RESISTANCE AT RESISTOR


const float RES_STRAIGHT_1 = 26600; // RESISTANCE WHEN STRAIGHT
const float RES_RIGHT_ANGLE_1 = 42600.0; // RESISTANCE AT 90

const float RES_STRAIGHT_2 = 26600; // RESISTANCE WHEN STRAIGHT
const float RES_RIGHT_ANGLE_2 = 42600.0; // RESISTANCE AT 90
*/

const int FLEX_PIN_1 = A1;      //PIN THATS CONNECTED TO THE VOLTAGE DIVIDER OUTPUT
const int FLEX_PIN_2 = A2

const float VCC_1 = 4.71;       // MEASURE voltage at 5 Volts
const float RES_1 = 330;    // MEASURE RESISTANCE AT RESISTOR

const float VCC_2 = 4.71;       // MEASURE voltage at 5 Volts
const float RES_2 = 330;    // MEASURE RESISTANCE AT RESISTOR


const float RES_STRAIGHT_1 = 28400; // RESISTANCE WHEN STRAIGHT
const float RES_RIGHT_ANGLE_1 = 65000.0; // RESISTANCE AT 90

const float RES_STRAIGHT_2 = 23400; // RESISTANCE WHEN STRAIGHT
const float RES_RIGHT_ANGLE_2 = 51100.0; // RESISTANCE AT 90



int value = 0;

int pin1 = 8; 

int flag = 0;




void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);         // Set pin A0 as input
  pinMode(FLEX_PIN, INPUT);   //Serial communication begin

  pinMode(pin1,OUTPUT);
 

}
void loop() {
  // put your main code here, to run repeatedly:
  value = analogRead(FLEX_PIN); //reads the analog value

  float FLEX_VOLTAGE = value * VCC / 1023.0; //voltage
  float FLEX_RES = RES * (VCC / FLEX_VOLTAGE -1); //resistance

  //Serial.println("Resistance: " + String(FLEX_RES) + " OHMS"); //prints value of resistance

  float ANGLE = map(FLEX_RES, 89795.33, 270000, 0, 90.0);

  //Serial.println("Bend: " + String(ANGLE) + " degrees");
  //Serial.println("----------------------------------------");
  
  Serial.print("(100,100)");
  if(flag == 0 && ANGLE>=20){
    
    flag = 1;
    
    Serial.print("(C1)");
  }else if (flag == 1 && ANGLE <= 20){
  
    flag = 0;
    Serial.print("(C0)");
   
  }
  

  delay(20);

  

}
  
