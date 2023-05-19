#include <HX711-multi.h>

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

long results[6]={0,0,0,0,0,0};

HX711MULTI scales(CHANNEL_COUNT, DOUTS, CLK);

long tmr=0;
long m[4];
byte buf[4];
long tares[6]={400000,300000,300000,400000,0,1};
int taresch[4]={0,0,0,0};

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
  delay(1000);
  Serial.write("1234567890123456789004");
  delay(3000);
  pinMode(11,OUTPUT);
  

}

void sendDataOut(long resultsarr[]) {

if ((resultsarr[4]==0) || (resultsarr[4]==1)) {
  Serial.write(resultsarr[4]+35);
  for (int i=0; i<4; ++i) { //scales.get_count()
  
    long datas=round(long(-resultsarr[i]*0.1133)); 
    if ((datas == 0)||(datas < -300000)) datas = m[i];
    if (datas<tares[i]) 
       if (taresch[i]>2) {tares[i]=datas; taresch[i]=0;}
       else taresch[i]=taresch[i]+1;
 
    m[i]=datas;    
    datas=datas-tares[i]+1;
    
    
    buf[0] = (byte) (datas & 63);
    buf[1] = (byte) ((datas >> 6) & 63);
    buf[2] = (byte) ((datas >> 12) & 63);
    buf[3] = (byte) ((datas >> 18) & 63);
          for(int j = 0; j < 4; j++){
            if (buf[j] > 58) Serial.write(buf[j]+1+35);
            else Serial.write(buf[j]+35);
            }
         //   Serial.write("\t");
  }
  
    long datas=tmr;
      
    buf[0] = (byte) (datas & 63);
    buf[1] = (byte) ((datas >> 6) & 63);
    buf[2] = (byte) ((datas >> 12) & 63);
    buf[3] = (byte) ((datas >> 18) & 63);
    for(int j = 0; j < 4; j++){
     if (buf[j] > 58) Serial.write(buf[j]+1+35);
     else Serial.write(buf[j]+35);
    } 
    Serial.write(38);
  //  Serial.write("\t\n");
  Serial.flush();
 
 // delay(10);
}
}

void loop() {
  scales.read(results);  
 //Serial.println(tmr);
  sendDataOut(results); 
 if (tmr>20001) tmr=2;
 //if ((tmr&16383)<3) sendDataOut(tares); 
 
}

ISR(TIMER1_OVF_vect)
{
  tmr=tmr+1;
}
