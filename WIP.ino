//Arduino Beltstrap Guitar Clipper - Pre-amp and Effects
//WIP Code: Waiting on mark 1 prototype for further testing.
//Based on code from https://www.electrosmash.com/pedalshield-uno

#include <avr/pgmspace.h>

//defining harware resources.
#define LED 13
#define FOOTSWITCH 12
#define TOGGLE 2
#define PUSHBUTTON_1 A5
#define PUSHBUTTON_2 A4

//defining the output PWM parameters
#define PWM_FREQ 0x00FF // pwm frequency - 31.3KHz
#define PWM_MODE 0 // Fast (1) or Phase Correct (0)
#define PWM_QTY 2 // 2 PWMs in parallel

//other variables
int input, vol_variable=512;
int counter=0;
unsigned int ADC_low, ADC_high;

void setup() { //setup IO - inputs/outputs pins configurations and pull-ups
pinMode(FOOTSWITCH, INPUT_PULLUP);
pinMode(TOGGLE, INPUT_PULLUP);
pinMode(PUSHBUTTON_1, INPUT_PULLUP);
pinMode(PUSHBUTTON_2, INPUT_PULLUP);
pinMode(LED, OUTPUT);

// setup ADC- configured to be reading automatically the hole time.
ADMUX = 0x60; // left adjust, adc0, internal vcc
ADCSRA = 0xe5; // turn on adc, ck/32, auto trigger
ADCSRB = 0x07; // t1 capture for trigger
DIDR0 = 0x01; // turn off digital inputs for adc0

// setup PWM - for more info about this config check the forum.
TCCR1A = (((PWM_QTY - 1) << 5) | 0x80 | (PWM_MODE << 1)); //
TCCR1B = ((PWM_MODE << 3) | 0x11); // ck/1 TIMSK1 = 0x20; // interrupt on capture interrupt ICR1H = (PWM_FREQ >> 8);
ICR1L = (PWM_FREQ & 0xff);
DDRB |= ((PWM_QTY << 1) | 0x02); // turn on outputs
sei(); // turn on interrupts - not really necessary with arduino


}

const int probePin = A0;
int probeValue;

void loop()
{
  //record an initial time
  long sampleStop = micros();

  //storage for sound samples
  unsigned char sample;
  
  //Read Guitar Input
  probeValue = analogRead(probePin);
  
  //Convert 0 to 1023 to volts: 1024/5 = 204.8:
  float volts = probeValue/204.8;
  float voltDiff = volts - 2.4;
  volts = volts + voltDiff*28;
  unsigned char soundValue = map(volts, 0, 5, 0, 255);
    
  //Assuming value is an unsigned int
  byte low = probeValue & 0x00FF; //The & 0X00FF masks out high byte (probably unnecessary)
  byte high = probeValue >> 8; //will pad high bits with 0s after shift - would pad with 1s if value were negative
  
  //analogWrite(9, high);
  //analogWrite(10, low);

  
  
  //Bit Masks
  //probeValue &= 0xFF0; // 6 bit
  //probeValue &= 0xFFFC; // 8 bit  
  
  //10 bit output, 8 bits via PWM 9, 2 bits via PWM 10
  analogWrite(9, probeValue/4);
  analogWrite(10, (probeValue%4)*64);
    
  //Increased Range required, assumed - 64x digital amplification (via post modification)
  //16 bit to 8 and 8
  //analogWrite(9, probeValue/256);
  //analogWrite(10, probeValue%256);
   
  
  //500 microsecond delay: 2khz (Retro Effect?)
  //50 microsecond delay: 20khz
  delayMicroseconds(500);
}
