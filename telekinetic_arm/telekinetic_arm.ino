// Brain Serial Test
// Description: Grabs brain data from the serial RX pin and sends CSV out over the TX pin (Half duplex.)
// Seeeduino Xiao
#include <Brain.h>
#include <SoftwareSerial.h>

#include <SPI.h>  // for radio module
#include <nRF24L01.h>
#include <RF24.h>

// Set up the brain parser, pass it the serial object you want to listen on.
// For Arduino UNO, this should be the hardware serial. For Arduino Micro, this should be the software serial.
Brain brain(Serial1);

RF24 radio(5, 4); // CE, CSN
const byte address[6] = "00001";

byte meditation;
byte prev_meditation;
byte prev_attention;
byte attention;
byte attention_threshold=85;
byte meditation_threshold=70;



int angleValue;

void setup() {
  // Start the hardware serial.
  Serial.begin(9600);

  // Start the software serial.
  Serial1.begin(9600);

  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();

  pinMode(3, OUTPUT);
}

void loop() {
  //digitalWrite(3, LOW);
  if (Serial1.available() > 0) {
    // Expect packets about once per second.
    // The .readCSV() function returns a string (well, char*) listing the most recent brain data, in the following format:
    // "signal strength, attention, meditation, delta, theta, low alpha, high alpha, low beta, high beta, low gamma, high gamma"
    if (brain.update()) {
      Serial.println(brain.readErrors());
      Serial.println(brain.readCSV());
      

      if (brain.readSignalQuality() != 0) {
        Serial.println("Bad signal quality.");
        digitalWrite(3, HIGH);
        return;
      }
      digitalWrite(3, LOW);
      prev_meditation = meditation;
      meditation = brain.readMeditation();
      prev_attention = attention;
      attention = brain.readAttention();

      Serial.print("Prev Meditation Val: ");
      Serial.println(prev_meditation);
      Serial.print("Current Meditation Val: ");
      Serial.println(meditation);

      Serial.print("Prev Attention Val: ");
      Serial.println(prev_attention);
      Serial.print("Current Attention Val: ");
      Serial.println(attention);

      if (prev_meditation>meditation_threshold && 
                meditation>meditation_threshold && 
                prev_attention<attention_threshold && 
                attention<attention_threshold){
                    Serial.print("Drink Water");
                    angleValue = 160;  
                    radio.write(&angleValue, sizeof(angleValue));                  
                    delay(5000);                
                  }
      else {
        angleValue = 30;
        radio.write(&angleValue, sizeof(angleValue));
        }
 
    }
  }
}
