#include "MeOrion.h"
#include "HX711.h"
// HX711.DOUT  - pin #
// HX711.PD_SCK - pin #
MePort port(PORT_3);
int s1 = port.pin1();
int s2 = port.pin2();
HX711 scale(s1, s2);
MeRGBLed led(PORT_6);
void setup() {
  Serial.begin(115200);
  scale.set_scale(2280.f);
  scale.tare();  
}

int waterStatus = 2;
unsigned long prevTime = 0;
bool prevTouch = false;
int gray = 0;

float j, f, k;
unsigned long checkTime = 0;
int EMPTY_CUP = 0;
int WEIGHT_CUP = 1;
int cups = 0;
float weight = 0;
float weights[20];
float waterPerformance = 600;
void loop() {
  if(millis()-checkTime>1000){
    weight = scale.get_units();
    if(weight<100){
      waterStatus = EMPTY_CUP;
    }else{
      waterStatus = WEIGHT_CUP;
      for(int i=0;i<19;i++){
        weights[i] = weights[i+1];
      }
      weights[19] = weight;
      if(cups>19){
        waterPerformance -= 0.2;
        if(waterPerformance<500){
          waterPerformance = 500;
        }
        parseWeight();
      }else{
        cups++;
      }
    }
    checkTime = millis();
    Serial.print(waterPerformance);
    Serial.print(":");
    Serial.println(weight);
  }
  if(waterStatus==EMPTY_CUP){
    lightOff();
  }else if(waterStatus==WEIGHT_CUP){
    colorloop();
  }
}
void parseWeight(){
  int i,maxIndex,minIndex;
  float maxWeight = 0.0;
  float minWeight = 1000.0;
  for(i=0;i<20;i++){
    if(weights[i]>maxWeight){
      maxWeight = weights[i];
      maxIndex = i;
    }
    if(weights[i]<minWeight){
      minWeight = weights[i];
      minIndex = i;
    }
  }
  if(maxIndex<minIndex){
    if(maxIndex<10){
      float w = 0;
      for(i=minIndex;i<20;i++){
        w += weights[i]/(20-minIndex);
      }
      waterPerformance += (maxWeight-w)*2.5; 
      for(i=0;i<20;i++){
        weights[i] = 0;
      }
      cups = 0;
    }
  }
}
void lightOff(){
  led.setColor(0,0,0);
  led.show();
}
void lightOn(){
  led.setColor(128,128,128);
  led.show();
}
void colorloop(){
  if(micros()-prevTime>30000){
    prevTime = micros();
    uint8_t red  = (1000-waterPerformance) * (1 + sin(j / 4.0) )/25;
    uint8_t green = (waterPerformance-500) * (1 + sin(f / 9.0 + 2.1) )/25;
    uint8_t blue = 0 * (1 + sin(k / 14.0 + 4.2) );
    led.setColor(red, green, blue);
    led.show();
    j += random(1, 6) / 6.0;
    f += random(1, 6) / 6.0;
    k += random(1, 6) / 6.0;
  }
}

