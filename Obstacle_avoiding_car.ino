#include <LiquidCrystal.h> // Include LCD library

// Define pins for ultrasonic sensors
const int trigPinLeft = 2;
const int echoPinLeft = 3;
const int trigPinRight = 4;
const int echoPinRight = 5;

// Define pin for IR sensor
const int irSensorPin = 6;

// Define pin for gas sensor
const int gasSensorPin = A0;

// Define pins for relays
const int relayPin1 = 7;
const int relayPin2 = 8;
const int relayPin3 = 9;
const int relayPin4 = 10;

// Define pins for LCD
const int rs = 11;
const int en = 12;
const int d4 = A1;
const int d5 = A2;
const int d6 = A3;
const int d7 = 13;

// buzzer pin
const int buzz= A5;
// Initialize the LCD library
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Track the last movement state to avoid duplicate messages
String lastMovement = "";

void setup() {
  // Initialize serial communication
  Serial.begin(9600);

  // Initialize the LCD
  lcd.begin(16, 2);
  Serial.println("System is Ready");
  lcd.setCursor(0, 0);
  lcd.print("Autonomous");
  lcd.setCursor(0, 1);
  lcd.print("Vehicle");
  delay(3000);
  lcd.clear();
  lcd.print("System Ready");
  delay(2000);
  lcd.clear();

  // Set ultrasonic sensor pins as input/output
  pinMode(trigPinLeft, OUTPUT);
  pinMode(echoPinLeft, INPUT);
  pinMode(trigPinRight, OUTPUT);
  pinMode(echoPinRight, INPUT);

  // Set IR sensor pin as input
  pinMode(irSensorPin, INPUT);

  // Set gas sensor pin as input
  pinMode(gasSensorPin, INPUT);

  // Set relay pins as output
  pinMode(relayPin1, OUTPUT);
  pinMode(relayPin2, OUTPUT);
  pinMode(relayPin3, OUTPUT);
  pinMode(relayPin4, OUTPUT);
  // buzzer pi
  pinMode(buzz,OUTPUT);
}

long readUltrasonicDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  long distance = duration * 0.034 / 2; // Convert to cm
  return distance;
}

void loop() {
  // Read distances from ultrasonic sensors
  long distanceLeft = readUltrasonicDistance(trigPinLeft, echoPinLeft);
  long distanceRight = readUltrasonicDistance(trigPinRight, echoPinRight);

  // Prepare messages for display
  String leftStatus = (distanceLeft < 20) ? "YES" : "NO ";
  String rightStatus = (distanceRight < 20) ? "YES" : "NO ";

  // Check IR sensor for path hole
  int irValue = digitalRead(irSensorPin);
  String pathHoleStatus = (irValue == HIGH) ? "NO " : "YES";

  // Check gas sensor for smoke
  int gasValue = digitalRead(gasSensorPin);
  Serial.print(gasValue);
  String smokeStatus = (gasValue == HIGH) ? "YES" : "NO ";
  // Print messages in a structured format
  lcd.setCursor(0, 0);
  lcd.print("Lo:" + leftStatus + " Ro:" + rightStatus);

  lcd.setCursor(0, 1);
  lcd.print("Hole:" + pathHoleStatus + " Sm:" + smokeStatus);

  // Control relay based on sensor status
  
if(irValue == HIGH && gasValue == LOW){
		if(distanceRight <20 && distanceLeft <20) {
		controlRelay(1, 1, 1, 1, "Stopping");
    digitalWrite(buzz,HIGH);
    delay(1000);
    digitalWrite(buzz,LOW);
		}
		else if(distanceRight <20 && distanceLeft >20){
			controlRelay(0, 1, 1, 0, "Moving Left");
		}
		else if(distanceRight >20 && distanceLeft <20){
			controlRelay(1, 0, 0, 1, "Moving Right");
		}
		else{
		controlRelay(1, 0, 1, 0, "Moving Forward");

	}

}

else{
controlRelay(1, 1, 1, 1, "Stopping");
digitalWrite(buzz,HIGH);
delay(1000);
digitalWrite(buzz,LOW);
}
delay(500);

}

void controlRelay(bool R1, bool R2, bool R3, bool R4, String movement) {
  // Update relay states
  digitalWrite(relayPin1, R1);
  digitalWrite(relayPin2, R2);
  digitalWrite(relayPin3, R3);
  digitalWrite(relayPin4, R4);

  // Print movement status if it changes
  if (movement != lastMovement) {
    Serial.println("Action: " + movement);
    lastMovement = movement;
  }
}



///////   forward   0 1 0 1   -> 1 0 1 0   
///////   backward  1 0 1 0   -> 0 1 0 1
///////   right     0 1 1 0   -> 1 0 0 1
///////   left      1 0 0 1   -> 0 1 1 0
///////   stop     `0 0 0 0   -> 1 1 1 1