/*----------------------------------------------------------------------sysextest.ino
 * this uses pjrc's midi library to get the sysex info.
 *
 */
#include <Wire.h> 
#include <MIDI.h>
#include "MidiSpecs.h"
#include <WiiChuck.h>

#include <stdio.h>
#include <stdarg.h>

WiiChuck wii = WiiChuck();        // sda, scl



void sendThroughSysex(char *printbuffer, int bufferlength);
uint8_t sysexbuffer[80]={0xF0,0x7D,0x33,0x33,0x00,0xf7};
char _pBr[40];
int _nChars;
#define debug(...) _nChars = sprintf(_pBr, __VA_ARGS__); sendThroughSysex(_pBr,_nChars);
void setup() {
    wii.init();

  ;
}

uint8_t t=0;

/*--------------------------------------------------------------------------check_midi()
 *
 * check midi looks for sysex messages and responds to the universal sysex general
 * info request
 *
 */
uint8_t identityResponse[]={MIDIv1_SYSEX_START,USYSEX_NON_REAL_TIME,USYSEX_ALL_CHANNELS,
                            USYSEX_GENERAL_INFO,USYSEX_GI_ID_RESPONSE,
                            0x7d,0x03,0x0d,      // vendor id and family
                            0x01,0x01,           // product id
                            0x00,0x00,0x00,0x01, // version
                            MIDIv1_SYSEX_END};

bool check_midi() {
    uint8_t *theSysex,theLength;
    if (usbMIDI.read() && usbMIDI.getType()==7) {
        debug("I Got a Sysex (length=%d) !",theLength=usbMIDI.getData1());
        theSysex=usbMIDI.getSysExArray();
        if ((theLength>=6)
            && (theSysex[1]==USYSEX_NON_REAL_TIME)
            && (theSysex[3]==USYSEX_GENERAL_INFO)
            && (theSysex[4]==USYSEX_GI_ID_REQUEST)
            ) {
            //debug("sending identity");
            usbMIDI.sendSysEx(sizeof(identityResponse),identityResponse);
            return(true);
        }
        if (theLength==2) {
            debug("Empty Sysex Recieved");
        }
    }
}


#define MIDI_TASKS {check_midi();}
#define AUDIBLE_LAG_MS 15

/*
 * NOTE doesnt check for millis rollover.
 */
void idle(int ms) {
    long int cmillis;
    long int endmillis=millis()+ms;
    long lag=millis();
    while ((cmillis=millis())<endmillis) {
        if (cmillis>=lag) {
            MIDI_TASKS;
            lag=cmillis+AUDIBLE_LAG_MS;
        }
    }
}

 

void loop() {
    int n;
    for (t=1; t<126; t++){
      usbMIDI.sendNoteOn(t,127,1);
      idle(100);
      usbMIDI.sendNoteOff(t,0,1);
      idle(100);
      debug("note#%02X",t);
    }
}



void sendThroughSysex(char *printbuffer, int bufferlength) {
  int n;
  n = iHexLine(1, 0 , (uint8_t *) printbuffer, (uint8_t) bufferlength ,sysexbuffer+4);
  sysexbuffer[n+4]=0xf7;
  sysexbuffer[n+5]=0x00;
  usbMIDI.sendSysEx(n+5,sysexbuffer);              
}

#define HIGHBYTE(x) ((uint8_t) (((x) >> 8) &  0x00ff) )
#define LOWBYTE(x) ((uint8_t) ((x) & 0x00ff) )
#define HIGHNIBBLE(x) ((((uint8_t)(x)) & 0xF0) >> 4)
#define LOWNIBBLE(x) (((uint8_t)(x)) & 0x0F)
#define HEXCHAR(c) ((c>9)?55+c:48+c)

uint8_t iHexLine(uint8_t rectype, uint16_t address, uint8_t *payload,uint8_t payloadlength, uint8_t *buffer) {

  int i=0; int j;
  uint8_t n=0;
  uint16_t checksum=0;
  //uint16_t length=0;
  
  buffer[i++]=':';
  
  checksum+=payloadlength;
  buffer[i++]=HEXCHAR(HIGHNIBBLE(payloadlength));
  buffer[i++]=HEXCHAR(LOWNIBBLE(payloadlength));

  n=HIGHBYTE(address);
  checksum+=n;
  buffer[i++]=HEXCHAR(HIGHNIBBLE(n));
  buffer[i++]=HEXCHAR(LOWNIBBLE(n));

  n=LOWBYTE(address);
  checksum+=n;
  buffer[i++]=HEXCHAR(HIGHNIBBLE(n));
  buffer[i++]=HEXCHAR(LOWNIBBLE(n));

  n=rectype;
  checksum+=n;
  buffer[i++]=HEXCHAR(HIGHNIBBLE(n));
  buffer[i++]=HEXCHAR(LOWNIBBLE(n));
  
  for (j=0; j<payloadlength ; j++) {
      n=payload[j];
      checksum+=n;
      buffer[i++]=HEXCHAR(HIGHNIBBLE(n));
      buffer[i++]=HEXCHAR(LOWNIBBLE(n));
  }
  n=~((uint8_t) checksum&0x00ff)+1;
  buffer[i++]=HEXCHAR(HIGHNIBBLE(n));
  buffer[i++]=HEXCHAR(LOWNIBBLE(n));
  return i;
}
