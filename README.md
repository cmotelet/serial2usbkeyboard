# Minimal keyboard emulator
Read input from serial console VT100 with speed port at 19200 bauds and forward the input to an emulated keybord.  

Emphasized characters are not managed

To enable/disable debug, in serial console, hit 3 times `[ESC]` key then hit `[SPACE]` key  

```
Use ATTiny85 (digispark) with bootloader micronucleus V2.6 (6650 bytes free)  
                   +-\/-+  
 RESET  (D5) PB5  1|    |8  VCC  
  USB-  (D3) PB3  2|    |7  PB2 (D2) <= RX serial 19200 bauds  
  USB+  (D4) PB4  3|    |6  PB1 (D1) BUILTIN LED  
             GND  4|    |5  PB0 (D0)  
                   +----+  
```
This is just a proof of concept, be careful if you use it. ;-)  

# Credits:  
http://digistump.com/products/1  
https://github.com/micronucleus/micronucleus  
https://github.com/J-Rios/Digispark_SoftSerial-INT0  
