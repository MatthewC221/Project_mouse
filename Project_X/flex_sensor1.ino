const int FLEX_PIN = A3;      //PIN THATS CONNECTED TO THE VOLTAGE DIVIDER OUTPUT

const float VCC = 4.97;       // MEASURE voltage at 5 Volts
const float RES = 797;    // MEASURE RESISTANCE AT RESISTOR


const float RES_STRAIGHT = 26600; // RESISTANCE WHEN STRAIGHT
const float RES_RIGHT_ANGLE = 42600.0; // RESISTANCE AT 90

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
  
