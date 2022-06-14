#include <Wire.h> // specify use of Wire.h library
#define ASD1115 0x48  //Address of I2C device
#define ASD1115_1 0x49

byte setting[3];

byte channel [4];
byte buffer[3];
const int ledPin = 26;

const int freq = 5000;
const int ledChannel = 0;

const int resolution = 8;
const int zone = 5000;
const int dutyCycle = 200;
const int gain = 4.096; //gain one=4.096;gain two= 2.048;

unsigned long previoustime;
int period = 50; // 1/Frequency *1000ms
int val = 0;
int val_1 = 0;
int readADS(byte asd, int channel);
int count = 0;

const int enM = 32;
int pumpPin = 25;

void setup()   {
  //SETTING ADS INTERNAL PROPERTY//
  setting[0] = 1;  setting[1] = 0; setting[2] = 0b11100101;
  //setting[0] = 1;  setting[1] = 0b11110010; setting[2] = 0b11100101; // use it with setting array to control two ads with different input channel
  channel[0] = 0b11000010 ; channel[1] = 0b11010010; channel[2] = 0b11100010; channel[3] = 0b11110010 ;

  // config register is 1
  // bit 15 flag bit for single shot
  // Bits 14-12 input selection:
  // 100 >> ANC0    101 >> ANC1
  // 110 >> ANC2    111 >> ANC3
  // Bits 11-9 Amp gain: (Default to 010 here 001 P19)
  // 000 >> FS = ±6.144V    100 >> FS = ±0.512V
  // 001 >> FS = ±4.096V    101 >> FS = ±0.256V
  // 010 >> FS = ±2.048V    110 >> FS = ±0.256V
  // 011 >> FS = ±1.024V    111 >> FS = ±0.256V
  // Bit 8 Operational mode of the ADS1115:
  // 0 : Continuous conversion mode
  // 1 : Power-down single-shot mode (default)
  // Bits 7-5 data rate default to 100 for 128SPS(Sample per second):
  // 000 >> 8SPS   100 >> 128SPS (default)
  // 001 >> 16SPS    101 >> 250SPS
  // 010 >> 32SPS    110 >> 475SPS
  // 011 >> 64SPS    111 >> 860SPS
  // Bits 4-0  comparator functions see spec sheet.
  //DONE SETTING//

  Serial.begin(115200);
  Wire.begin(); // begin I2C


  pinMode(ledPin, OUTPUT);
  pinMode(pumpPin, OUTPUT);
  pinMode(enM, OUTPUT); // Switch on Heater (LILYGO only)
  ledcSetup(ledChannel, freq, resolution);
  ledcAttachPin(ledPin, ledChannel);
  ledcWrite(ledChannel, dutyCycle);
  digitalWrite(enM, HIGH);
  digitalWrite(pumpPin, HIGH);
  Serial.println("Hello");
}

void loop() {
  //Read one channel only;
  if (millis() > previoustime + period) { // Sample with customed Frequency
    previoustime = millis();
    readADS(ASD1115, 3);
////////////////////////////////////////////////////////////////////////////////////////
//    //Read mutiple channel from two ads simutaneously
//    if (millis() > previoustime + period) { // Sample with customed Frequency
//      previoustime = millis();
//      if (count % 2 == 0) {
//        if (count % 8 == 0) {
//          readADS(ASD1115, 0);
//        } else if (count % 8 == 2) {
//          readADS(ASD1115, 1);
//        } else if (count % 8 == 4) {
//          readADS(ASD1115, 2);
//        } else if (count % 8 == 6) {
//          readADS(ASD1115, 3);
//        }
//      } else {
//        if (count % 8 == 1) {
//          readADS(ASD1115_1, 0);
//        } else if (count % 8 == 3) {
//          readADS(ASD1115_1, 1);
//        } else if (count % 8 == 5) {
//          readADS(ASD1115_1, 2);
//        } else if (count % 8 == 7) {
//          readADS(ASD1115_1, 3);
//        }
//      }
//      count = count + 1;
//    }
//////////////////////////////////////////////////////////////////////////////////////
  }
}
  int readADS(byte asd, int buff){
    switch (asd) {
      case 0x48:
        Wire.beginTransmission(ASD1115);  // ADC
        setting[1] = channel[buff];
        Wire.write(setting[0]); Wire.write(setting[1]); Wire.write(setting[2]);
        Wire.endTransmission();
        
        buffer[0] = 0; // pointer
        Wire.beginTransmission(asd);
        Wire.write(buffer[0]);  // pointer
        Wire.endTransmission();
        delay(10);
        Wire.requestFrom(asd, 2);
        buffer[1] = Wire.read(); buffer[2] = Wire.read();
        delay(10);
        Wire.endTransmission();
        delay(10);
        val = buffer[1] << 8 | buffer[2]; // convert display results
        delay(10);
        Serial.println(val);
        switch (buff) {
          case 0:
            Serial.print("ADS 1_0: "); Serial.print(val); Serial.println("\t\t");
            break;
          case 1:
            Serial.print("ADS 1_1: "); Serial.print(val); Serial.println("\t\t");
            break;
          case 2:
            Serial.print("ADS 1_2: "); Serial.print(val); Serial.println("\t\t");
            break;
          case 3:
            Serial.print("ADS 1_3: "); Serial.print(val); Serial.println("\t\t");
            break;
        }
        break;
      case 0x49:
        Wire.beginTransmission(ASD1115_1);  // ADC
        setting[1] = channel[buff];
        Wire.write(setting[0]); Wire.write(setting[1]); Wire.write(setting[2]);

        Wire.beginTransmission(ASD1115_1);
        buffer[0] = 0; // pointer
        Wire.write(buffer[0]);  // pointer
        Wire.endTransmission();

        Wire.requestFrom(ASD1115_1, 2);
        buffer[1] = Wire.read(); buffer[2] = Wire.read();
        delay(10);
        Wire.endTransmission();

        val_1 = buffer[1] << 8 | buffer[2];
        delay(10);// convert display results
        switch (buff) {
          case 0:
            Serial.print("ADS 2_0: "); Serial.println(val_1);
            break;
          case 1:
            Serial.print("ADS 2_1: "); Serial.println(val_1);
            break;
          case 2:
            Serial.print("ADS 2_2: "); Serial.println(val_1);
            break;
          case 3:
            Serial.print("ADS 2_3: "); Serial.println(val_1);
            break;
        }
        break;
    }
  }
