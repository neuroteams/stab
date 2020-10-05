#include "HX711-multi.h"
#include <avr/io.h>
#include <avr/interrupt.h>

// Pins to the load cell amp

#define CLK 9      // clock pin to the load cell amp
#define DOUT1 A0    // data pin to the first lca
#define DOUT2 A1    // data pin to the second lca
#define DOUT3 A2    // data pin to the third lca
#define DOUT4 A3    // data pin to the third lca


#define TARE_TIMEOUT_SECONDS 4

byte DOUTS[4] = {DOUT1, DOUT2, DOUT3, DOUT4};

#define CHANNEL_COUNT sizeof(DOUTS)/sizeof(byte)

long results[5]={0,0,0,0,0};

HX711MULTI scales(CHANNEL_COUNT, DOUTS, CLK);

long tmr=0;
long m[4];
byte buf[3];
long tares[4]={9999999,9999999,99999990,9999999};

void setup() {

 cli(); // отключить глобальные прерывания
    TCCR1A = 0; // установить TCCR1A регистр в 0
    TCCR1B = 0;

    // включить прерывание Timer1 overflow:
    TIMSK1 = (1 << TOIE1);
    // Установить CS10 бит так, чтобы таймер работал при тактовой частоте:
    TCCR1B |= (1 << CS10);

    sei();  // включить глобальные прерывания
  
  Serial.begin(115200); 
  //Serial.begin(57600);
  Serial.flush();
  delay(3000);
  pinMode(11,OUTPUT);
  

}

void sendRawData() {
  scales.read(results);
if (results[4]==0) {
  for (int i=0; i<4; ++i) {; //scales.get_count()
  
    long datas=round(long(-results[i]*0.1133)); 
    if ((datas == 0)||(datas < -300000)) datas = m[i];
    if (datas<tares[i]) tares[i]=datas;
 
    m[i]=datas;    
    datas=datas-tares[i]+1;
    
    
    buf[0] = (byte) (datas & 63);
    buf[1] = (byte) ((datas >> 6) & 63);
    buf[2] = (byte) ((datas >> 12) & 63);
    // buf[3] = (byte) ((datas >> 18 & 63);
          for(int j = 0; j < 3; j++){
            if (buf[j] > 58) Serial.write(buf[j]+1+35);
            else Serial.write(buf[j]+35);
            }
  }
  
    long datas=tmr;
      
    buf[0] = (byte) (datas & 63);
    buf[1] = (byte) ((datas >> 6) & 63);
    buf[2] = (byte) ((datas >> 12) & 63);
    // buf[3] = (byte) ((datas >> 18 & 63);
    for(int j = 0; j < 3; j++){
     if (buf[j] > 58) Serial.write(buf[j]+1+35);
     else Serial.write(buf[j]+35);
    } 
  
  if (tmr>200001) tmr=2;
  delay(10);
}
}

void loop() {
  
  sendRawData(); 

 
}

ISR(TIMER1_OVF_vect)
{
  tmr=tmr+1;
}
