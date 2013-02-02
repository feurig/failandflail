import rtmidi_python as rtmidi

import rtmidi_python as rtmidi

def callback(message, time_stamp):
    if (len(message)>4 and message[0]==240
        and message[1]==125 and message[2]==51 and message[3]==51
        ):
        decodeHexLine(message);
        #print message, time_stamp

#:llaaaattd1..dncs
def decodeHexLine(payload):
#    print payload    
    #    length = int(chr(payload[5])+chr(payload[6]),16)
    #address = int(chr(payload[7])+chr(payload[8])+chr(payload[9])+chr(payload[10]),16)
    #type =  int(chr(payload[11])+chr(payload[12]),16)
    #type =  int(chr(payload[len(payload)-2])+chr(payload[len(payload)-2]),16)
    high=None
    decoded=''
    for byte in payload[13:len(payload)-3] :
        #print byte,'/',
        if high is None:
            high=byte
        else:
            pair=chr(high)+chr(byte)
            #print "["+pair+"]",
            decoded=decoded+chr(int(pair,16))
            high=None
    print decoded

midi_in = rtmidi.MidiIn()
print midi_in.ports
midi_in.callback = callback
midi_in.ignore_types(False,False,False)
midi_in.open_port(0)

while 1: pass
