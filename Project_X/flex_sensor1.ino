
const int FLEX_PIN = A0;      //PIN THATS CONNECTED TO THE VOLTAGE DIVIDER OUTPUT

const float VCC = 4.98;       // MEASURE voltage at 5 Volts
const float RES = 40000.0;    // MEASURE RESISTANCE AT RESISTOR


const float RES_STRAIGHT = 37300.0; // RESISTANCE WHEN STRAIGHT
const float RES_RIGHT_ANGLE = 90000.0; // RESISTANCE AT 90

int value = 0;

int pin1 = 8; 




void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);         // Set pin A0 as input
  pinMode(FLEX_PIN, INPUT);   //Serial communication begin

  pinMode(pin1,OUTPUT);
 

}

void loop() {
  // put your main code here, to run repeatedly:
  value = analogRead(FLEX_PIN); //reads the analog value
  Serial.println(value);      //prints analog value

  if(value >= 0){
    digitalWrite(pin1, HIGH);// turns led on if bend is a certain value
  }
   
 
 

  float FLEX_VOLTAGE = value * VCC / 1023.0; //voltage
  float FLEX_RES = RES * (VCC / FLEX_VOLTAGE -1); //resistance

  Serial.println("Resistance: " + String(FLEX_RES) + "OHMS"); //prints value of resistance

  float ANGLE = map(FLEX_RES, RES_STRAIGHT, RES_RIGHT_ANGLE, 0, 90.0);

  Serial.println("Bend: " + String(ANGLE) + "degrees");
  Serial.println("----------------------------------------");


  delay(100);
  

  
  
  
  

}
