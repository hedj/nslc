#!/usr/bin/python3
# A Non-Standard Line Control library for python

import time
import serial
import numpy as np
import sys
import zmq

class NSLC:

    candidate_frame = ''

    T=np.array(
    [13,161,243,130,182,147,68,235,82,83,221,191,112,38,135,184,143,206
    ,240,116,253,131,106,220,231,250,120,75,223,79,196,16,69,230,140,101
    ,150,98,245,104,111,20,236,216,192,76,6,185,64,144,12,96,94,165
    ,152,190,99,203,63,66,226,232,164,224,193,145,119,160,159,51,17,108
    ,149,93,24,222,121,7,215,124,70,141,207,227,211,80,50,173,157,163
    ,167,233,14,72,11,202,225,100,1,40,142,113,168,132,86,84,62,178
    ,248,228,118,28,247,153,15,172,92,23,158,47,189,155,48,52,169,134
    ,25,219,170,49,218,59,45,67,136,156,186,244,29,35,3,199,209,176
    ,31,175,58,128,73,234,133,213,95,122,177,148,238,208,85,109,217,0
    ,36,33,174,171,241,205,146,204,229,61,10,26,129,107,183,74,195,42
    ,125,2,105,32,110,194,187,114,138,181,57,46,91,55,198,137,162,89
    ,78,41,115,88,123,44,34,201,39,43,103,53,4,212,30,237,81,180
    ,127,54,90,179,102,19,214,5,37,97,126,200,27,65,154,21,252,166
    ,71,22,151,77,9,251,117,188,56,197,8,139,210,249,239,246,254,255
    ,242,87,60,18])

    def pearson_hash(self, input_bytes):
      h = 0
      for byte in input_bytes:
        index = h ^ byte
        h = T[index]
      return bytes([h])

    def escape(self, bytestring):
      out = bytes([])
      for b in bytestring:
        if b == 10:
          out += bytes([9,10])
        elif b == 9:
          out += bytes([9,32])
        else:
          out += bytes([b])
      return out

    def unescape(self, bytestring):
      out = bytes([])
      esc_set = False
      for b in bytestring:
        if esc_set:
          esc_set = False
          if b == 10:
            out += bytes([b])
          elif b == 32:
            out += bytes([9])
        elif b == 9:
          esc_set = True;
        else:
          out += bytes([b])
      return out

    def frame(self, bytestring):
      return escape( bytestring + pearson_hash(bytestring) ) + bytes([10])

    def is_valid_frame(self, bytestring):
      if len(bytestring) < 2:
        return False
      if ( bytestring[-1] != 10): # Valid frames end with LF
        return False
      u = unescape(bytestring[:-1]) # drop the LF, then unescape
      return pearson_hash(u[:-1]) == bytes([u[-1]]) # does the checksum match?

    def unframe(self, bytestring):
      if is_valid_frame(bytestring):
        u = unescape(bytestring[:-1])
        return u[:-1]
      else:
        return bytestring

def go():
  s = serial.Serial('COM3', 250000)

  lc = NSLC()

  inb = b''
  while(True):
    if(s.in_waiting > 0):
      inb += s.read()
    if( len(inb) >= 2):
      if inb[-1] == 10 and inb[-2] != 9:
        if(lc.is_valid_frame(inb)):
          print("Found valid frame %s"%lc.unframe(inb))
        else:
          print("Found invalid frame %s"%lc.unframe(inb))
        inb = b''

if __name__ == '__main__':
  go()
