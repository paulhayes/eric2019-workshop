#include <Arduino.h>
#include <i2cEncoderLibV2.h>
#include <Wire.h>
#include <SerialCommand.h>


/*
Addressable 5mm LED pinout
With the longest pin on the left, from left to right:

1: DOUT
2: 5V
3: GND
4: DIN

*/
#include <NeoPixelBus.h>
const uint16_t PixelCount = 4; // this example assumes 4 pixels, making it smaller will cause a failure
const uint8_t PixelPin = 5;  // make sure to set this to the correct pin, ignored for Esp8266
NeoPixelBus <NeoRgbFeature, Neo400KbpsMethod> leds(PixelCount, PixelPin);
NeoGamma<NeoGammaTableMethod> colorGamma;

/* ws2811 section ends */

const int IntPin = 7;
i2cEncoderLibV2 Encoder(0x01);

SerialCommand cmd;

const int numCols = 9;
uint32_t colors[] = {
    0xff0000,
    0x00ff00,
    0x0000ff,
    0xffff00,
    0x00ffff,
    0xff00ff,
    0x0,
    0xffffff,
    0x909090
};
const char * colorNames[] = {
    "red",
    "green",
    "blue",
    "yellow",
    "cyan",
    "magenta",
    "black",
    "white",
    "grey"
};

//Callback when the CVAL is incremented
void encoder_increment(i2cEncoderLibV2* obj) {
  //Serial.print("Increment: ");
  Serial.print("C0=");
  Serial.print(Encoder.readCounterByte());
  Serial.print(" ");
  Serial.println("+1");
}

//Callback when the CVAL is decremented
void encoder_decrement(i2cEncoderLibV2* obj) {
  //Serial.print("Decrement: ");
  Serial.print("C0=");
  Serial.print(Encoder.readCounterByte());
  Serial.print(" ");
  Serial.println("-1");
}

//Callback when the encoder is pushed
void encoder_push(i2cEncoderLibV2* obj) {
  Serial.println("B0=1");
}

//Callback when the encoder is released
void encoder_released(i2cEncoderLibV2* obj) {
  Serial.println("B0=0");
}

void setup(){

    
    pinMode(IntPin, INPUT);
    Serial.begin(115200);
    Wire.begin();
    Encoder.reset();
    Encoder.begin(
    i2cEncoderLibV2::INT_DATA | i2cEncoderLibV2::WRAP_ENABLE
    | i2cEncoderLibV2::DIRE_RIGHT | i2cEncoderLibV2::IPUP_ENABLE
    | i2cEncoderLibV2::RMOD_X1 | i2cEncoderLibV2::RGB_ENCODER);

    Encoder.writeCounter((int32_t) 0); /* Reset the counter value */
    Encoder.writeMax((int32_t) 24); /* Set the maximum threshold*/
    Encoder.writeMin((int32_t) 0); /* Set the minimum threshold */
    Encoder.writeStep((int32_t) 1); /* Set the step to 1*/
    Encoder.writeAntibouncingPeriod(20); /* Set an anti-bouncing of 200ms */
    Encoder.writeDoublePushPeriod(0); /*Set a period for the double push of 500ms */

    // Definition of the events
    Encoder.onIncrement = encoder_increment;
    Encoder.onDecrement = encoder_decrement;
    //Encoder.onMax = encoder_max;
    //Encoder.onMin = encoder_min;
    Encoder.onButtonPush = encoder_push;
    Encoder.onButtonRelease = encoder_released;
    //Encoder.onButtonDoublePush = encoder_double_push;

    Encoder.autoconfigInterrupt();
    

    cmd.addCommand("color",setColor);
    cmd.addCommand("??",info);
    cmd.setDefaultHandler(error);

    leds.Begin();
    leds.ClearTo( RgbColor(0));
    leds.Show();
}

uint32_t c= 0;
uint32_t col = 0;
void loop(){
    if (digitalRead(IntPin) == LOW) {
    /* Check the status of the encoder and call the callback */
        Encoder.updateStatus();
    }

    cmd.readSerial();

}

void setDialColor(){
    char * nextArg = cmd.next();
    char * endPtr = NULL;
    char * endPos = (nextArg+strlen(nextArg));
    uint32_t col = strtol(nextArg,&endPtr,16);
    if(endPtr<endPos){
        col = getColorFromWord(nextArg);        
        /*
        for(int i=0;i<numCols;i++){
            if( strcmp(nextArg,colorNames[i])==0 ){
                col = colors[i];
                break;
            }
            else if((i+1)==numCols){
                Serial.print("ERROR color name not found");
                Serial.println(nextArg);
            }
        }
        */
    }
    
    Encoder.writeRGBCode(col);

}

void setColor(){
    char * nextArg = cmd.next();
    char * endPtr = NULL;
    uint32_t ledIndex = strtol(nextArg,&endPtr,16);
    if(ledIndex==PixelCount){
        setDialColor();
    }
    nextArg = cmd.next();
    endPtr = nextArg;
    uint32_t ledCol = strtoul(nextArg,&endPtr,16);
    char * endPos = (nextArg+strlen(nextArg));
    
    if(endPtr<endPos){
        ledCol = getColorFromWord(nextArg);
        /*
        ledCol = 0;
        for(int i=0;i<numCols;i++){
            if( strcmp(nextArg,colorNames[i])==0 ){
                ledCol = colors[i];
                break;
            }
            else if((i+1)==numCols){
                Serial.print("ERROR color name not found");
                Serial.println(nextArg);
            }
        }
        */
    }
    
    if(ledIndex<PixelCount){
        char hexCode[8];
        sprintf(hexCode,"%06lx",ledCol);
        Serial.print("led ");
        Serial.print(ledIndex);
        Serial.print(" #");
        Serial.println(hexCode);
        leds.SetPixelColor(ledIndex,colorGamma.Correct( RgbColor( HtmlColor(ledCol) ) ) );
        leds.Show();
    }
}

uint32_t getColorFromWord(const char * word){
    uint32_t col;

    for(int i=0;i<numCols;i++){
            if( strcmp(word,colorNames[i])==0 ){
                col = colors[i];
                break;
            }
            else if((i+1)==numCols){
                Serial.print("ERROR color name not found");
                Serial.println(word);
            }
        }
    return col;
}


void info(){
    Serial.println("RotaryEncoderRGB v1.0");
}

void error(const char * command){
    Serial.print("ERROR: Unknown command ");
    Serial.println(command);
}