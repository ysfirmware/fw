@ECHO OFF
"C:\Atmel\AVR Tools\AvrAssembler2\avrasm2.exe" -S "C:\Atmel\OK2561asm\labels.tmp" -fI -W+ie -o "C:\Atmel\OK2561asm\AVRasmfile.hex" -d "C:\Atmel\OK2561asm\AVRasmfile.obj" -e "C:\Atmel\OK2561asm\AVRasmfile.eep" -m "C:\Atmel\OK2561asm\AVRasmfile.map" -l "C:\Atmel\OK2561asm\AVRasmfile.lst" "C:\Atmel\OK2561asm\Exp20_1.asm"
