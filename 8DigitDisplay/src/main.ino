#include <Arduino.h>
#include <LedControl.h>
#include <SerialCommand.h>

LedControl lc=LedControl(10,16,14,1);

SerialCommand cmd;

enum Align {
    left,
    center,
    right
};

const char * alignmentNames[] = {"left","center","right"};

Align currentAlign = right;

void setup(){
  /*
   The MAX72XX is in power-saving mode on startup,
   we have to do a wakeup call
   */
  lc.shutdown(0,false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0,8);
  /* and clear the display */
  lc.clearDisplay(0);

  cmd.addCommand("PRINT",print);
  cmd.addCommand("print",print);
  cmd.addCommand("ALIGN",align);
  cmd.addCommand("align",align);
  cmd.setDefaultHandler(error);
}


void loop(){
    delay(100);

    cmd.readSerial();
}


void error(const char * str){
    Serial.print("ERROR unknown command ");
    Serial.println(str);
}

void align(){
    char * str = cmd.next();
    for(int i=0;i<3;i++){
        if(strcmp(str,alignmentNames[i])==0){
            currentAlign = (Align)i;
            Serial.print("aligning ");
            Serial.println(alignmentNames[i]);
            return;
        }
    }
    Serial.print("ERROR unknown alignment: ");
    Serial.println(str);
}

void print(){
    char buffer[128];

    char * buf = cmd.next();
    char * dest = buffer;
    char * str = buffer;

    int remaining = 128;
    strcpy(str," ");

    while(buf!=NULL){
        if(buf[0]=='"'){
            buf++;
        }
        int bufLen = strlcpy(dest,buf,remaining);
        bufLen=min(remaining,bufLen);
        remaining-=bufLen;
        dest+=bufLen-1;
        if(dest[0]!='"'){
            dest++;
        }else {
            dest[0]='\0';
            remaining++;
        }        
        Serial.println(str);
        buf = cmd.next();
        if(buf!=NULL){
            dest[0]=' ';
            dest++;        
        }
    }

    lc.clearDisplay(0);
    
    int len = strlen(str);
    while(len>8){
        if(currentAlign == left){
            len--;
        }
        else {
            str++;
            len--;
        }
    }
    int i=min( len-1, 7);
    if(currentAlign==left){
        i=7;
    }else if(currentAlign==center){
        i=3+(len/2);
    }
    bool makeDot;
    
    while(str[0]!=0 && i>=0){
        makeDot = false;
        char c = str[0];
        switch(c){
            case 'g': c='6'; break;
            case 'l': c='1'; break;
            case 'I': c='1'; break;
            case 'i': c='1'; break;
            case 'O': c='0'; break;
            case 's': c='5'; break;
            case 'S': c='5'; break;
        }
        if(c=='.'){
            c = ' ';
            makeDot = true;
        }
        lc.setChar(0,i,c,makeDot);
        i--;
        str++;
    }
}