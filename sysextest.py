'''---------------------------------------------------------------- sysextest.py
This is a quick and dirty debugging console for a midi device.
    
It recieves sysex messages sent the the dev id 0x7d 0x33 0x33 and decodes and 
prints them. The messages are encoding using the intel hex format because
I planned to use the same format to transmit data to and from a bootloader.
    
Currently the sender is not identified however the address and type fields are
avaliable for this purpose.

In addition it also sends and recieves the Univeral midi sysex id request.
    

 Copyright (c) 2011 Donald Delmar Davis, Suspect Devices

 Permission is hereby granted, free of charge, to any person obtaining a copy of this
 software and associated documentation files (the "Software"), to deal in the Software
 without restriction, including without limitation the rights to use, copy, modify,
 merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
 permit persons to whom the Software is furnished to do so, subject to the following
 conditions:

 The above copyright notice and this permission notice shall be included in all copies
 or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
    
'''
import rtmidi_python as rtmidi
import time

# this stuff should probably go in a constants file. 
SYSEX_BEGIN             =0xf0
SYSEX_NON_REAL_TIME     =0x7e
SYSEX_NON_REAL          =0x7f
SYSEX_ALL_CHANNELS      =0x7f
SYSEX_GENERAL_INFO      =0x06
SYSEX_IDENTITY_REQUEST  =0x01
SYSEX_IDENTITY_REPLY    =0x02
SYSEX_END_SYSEX         =0xF7

askfortheids=[SYSEX_BEGIN,SYSEX_NON_REAL_TIME,SYSEX_ALL_CHANNELS,
              SYSEX_GENERAL_INFO,SYSEX_IDENTITY_REQUEST,SYSEX_END_SYSEX]

'''
    this is our callback routine for midi messages. 
    It handles sysexes sent to it and the universal sysex id response.
'''
def handleOurSysexes(message, time_stamp):
    if (len(message)>4 and message[0]==SYSEX_BEGIN): #if its a sysex message
        if (message[1]==125 and message[2]==51 and message[3]==51):
            print "DBG:",decodeHexLine(message)
        elif (message[1]==SYSEX_NON_REAL_TIME and message[3]==SYSEX_GENERAL_INFO
              and message[4]==SYSEX_IDENTITY_REPLY ):
            print 'MANUFACTURER ID: 0x%02X 0x%02X 0x%02X' % (
                   message[5],message[6],message[7])
            print "PRODUCT ID     : 0x%02X%02x" % (
                   message[8], message[9])
            print "VERSION        : 0x%02X%02x%02X%02x" % (
                   message[10],message[11],message[12],message[13])


#:llaaaattd1..dncs
def decodeHexLine(payload):
#    print payload    
    #length = int(chr(payload[5])+chr(payload[6]),16)
    #address = int(chr(payload[7])+chr(payload[8])+chr(payload[9])+chr(payload[10]),16)
    #type =  int(chr(payload[11])+chr(payload[12]),16)
    #checksum =  int(chr(payload[len(payload)-2])+chr(payload[len(payload)-2]),16)
    high=None
    decoded=''
    for byte in payload[13:len(payload)-3] :
        if high is None:
            high=byte
        else:
            pair=chr(high)+chr(byte)
            decoded=decoded+chr(int(pair,16))
            high=None
    return decoded

# main code begins here.

midi_in = rtmidi.MidiIn()
midi_out = rtmidi.MidiOut()
print midi_in.ports
midi_in.callback = handleOurSysexes
midi_in.ignore_types(False,False,False)
midi_in.open_port(0)
midi_out.open_port(0)
time.sleep(5)
midi_out.send_message(askfortheids)


while 1:
    time.sleep(5)
    print "requesting id"
    midi_out.send_message(askfortheids)


"""
    http://www.blitter.com/~russtopia/MIDI/~jglatt/tech/midispec.htm

    0xF0  SysEx
    0x7E  Non-Realtime
    0x7F  The SysEx channel. Could be from 0x00 to 0x7F. Here we set it to "disregard channel".
    0x06  Sub-ID -- General Information
    0x01  Sub-ID2 -- Identity Request
    0xF7  End of SysEx
    Here is the Identity Reply message:
    0xF0  SysEx
    0x7E  Non-Realtime
    0x7F  The SysEx channel. Could be from 0x00 to 0x7F.
    Here we set it to "disregard channel".
    0x06  Sub-ID -- General Information
    0x02  Sub-ID2 -- Identity Reply
    0xID  Manufacturer's ID
    0xf1  The f1 and f2 bytes make up the family code. Each
    0xf2  manufacturer assigns different family codes to his products.
    0xp1  The p1 and p2 bytes make up the model number. Each
    0xp2  manufacturer assigns different model numbers to his products.
    0xv1  The v1, v2, v3 and v4 bytes make up the version number.
    0xv2
    0xv3
    0xv4
    0xF7  End of SysEx
"""