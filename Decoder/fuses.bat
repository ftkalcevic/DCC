\avreal\avreal32 +atmega32 -p1 -as -w -v -o8000 -%% -! -n -fCKOPT=0,CKSEL=E,JTAGEN=1,OCDEN=1,EESV=0
rem CKOPT=0
rem CKSEL=E     1110 - 8MHz external
rem JTAGEN=1 disable jtag
rem OCDEN=1 disable on chip debugging
rem EESV=0 save eeprom between programs
