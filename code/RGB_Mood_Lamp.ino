#include "BluetoothSerial.h"

/* Check if Bluetooth configurations are enabled in the SDK */
/* If not, then you have to recompile the SDK */
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

const int pot_R=27;
const int pot_G=26;
const int pot_B=25;

const int red=19;
const int green=18;
const int blue=17;

const int redChannel=1;
const int greenChannel=2;
const int blueChannel=3;

const int man=15;

uint8_t r=0;
uint8_t g=0;
uint8_t b=0;

uint8_t avg_r = 0;
uint8_t avg_g = 0;
uint8_t avg_b = 0;


//state=0 -> potentiometer mode
//state=1 -> bluetooth mode
int state = 0;  

//Interrupt Service Routine
void IRAM_ATTR ISR(){
  
  state = digitalRead(man);

  if(state == 0){

    SerialBT.disconnect();
    SerialBT.end();
    
   }

   else{
    SerialBT.begin("MOOD LAMP");
    Serial.println("Bluetooth Started! Ready to pair...");
   }
}

void setup() {
  Serial.begin(115200);
  /* If no name is given, default 'ESP32' is applied */
  /* If you want to give your own name to ESP32 Bluetooth device, then */
  /* specify the name as an argument SerialBT.begin("myESP32Bluetooth"); */
   SerialBT.begin("MOOD LAMP");
   Serial.println("Bluetooth Started! Ready to pair...");

   pinMode(man,INPUT_PULLDOWN);
   attachInterrupt(man, ISR, CHANGE);
   
 //Configuring the PWM channels
   ledcAttachPin(red,redChannel);
   ledcAttachPin(green,greenChannel);
   ledcAttachPin(blue,blueChannel);
  
   ledcSetup(redChannel,5000, 8);
   ledcSetup(greenChannel,5000, 8);
   ledcSetup(blueChannel,5000, 8);
}

void loop() {
  // put your main code here, to run repeatedly:
  state = digitalRead(man);

  if(state == 0){  //Potentiometer mode
    for(int i=0;i<256;i++)
    {
    //Mapping 12-bit values to 8-bit
      r = analogRead(pot_R)>>4;
      g = analogRead(pot_G)>>4;
      b = analogRead(pot_B)>>4;

      avg_r += r;
      avg_g += g;
      avg_b += b;
    }
  //taking average to mitigate fluctuations
    avg_r /= 256;
    avg_g /= 256;
    avg_b /= 256;
  }
  else{           //Bluetooth Mode
    if(SerialBT.available()){
      r = SerialBT.read();
      g = SerialBT.read();
      b = SerialBT.read();  
    }
  }
 
//Writing the duty cycles to respective
//channels
  ledcWrite(redChannel, r);
  ledcWrite(greenChannel, g);
  ledcWrite(blueChannel, b);
  
}
