cd "C:\Program Files\Atmel\AVR Tools\AvrAssembler\atmega128_work\LEDS\"
C:
del "C:\Program Files\Atmel\AVR Tools\AvrAssembler\atmega128_work\LEDS\atmega128_leds.map"
del "C:\Program Files\Atmel\AVR Tools\AvrAssembler\atmega128_work\LEDS\atmega128_leds.lst"
"C:\Program Files\Atmel\AVR Tools\AvrAssembler\avrasm32.exe" -fI "C:\Program Files\Atmel\AVR Tools\AvrAssembler\atmega128_work\LEDS\atmega128_LEDS.asm" -o "C:\Program Files\Atmel\AVR Tools\AvrAssembler\atmega128_work\LEDS\atmega128_leds.hex" -d "C:\Program Files\Atmel\AVR Tools\AvrAssembler\atmega128_work\LEDS\atmega128_leds.obj" -e "C:\Program Files\Atmel\AVR Tools\AvrAssembler\atmega128_work\LEDS\atmega128_leds.eep" -I "C:\Program Files\Atmel\AVR Tools\AvrAssembler\atmega128_work\LEDS" -I "C:\Program Files\Atmel\AVR Tools\AvrAssembler\AppNotes" -w  -m "C:\Program Files\Atmel\AVR Tools\AvrAssembler\atmega128_work\LEDS\atmega128_leds.map"
