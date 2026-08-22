; Assembly code for temperatur sensor HDC1080
; For use with PIC 12F675 : take care to preserve band gap of config word and osscal calibration bits
; Use following compiler global options : 
; -msummary=+psect -Wl,-pisrVec=04h -Wl,-presetVec=00h
    
    
PROCESSOR 12F675
#include <xc.inc>

; PIC12F675 Configuration Bit Settings
CONFIG "FOSC = INTRCIO"  // Internal clock, all GPIO available
; CONFIG "FOSC = HS"     // External clock, High speed, high gain
CONFIG "WDTE = OFF"      // Watchdog Timer Enable bit (Disabled)
CONFIG "CP = OFF"        // Code Protection bit (Code protection off)
CONFIG "CPD = OFF"       // Flash Data Memory (Code protection off)
CONFIG "BOREN = OFF"     // BOR Disabled
CONFIG "PWRTE = ON"      // Power-up Timer Enable
CONFIG "MCLRE = OFF"     // MCLR Pin Function off
    
GLOBAL resetVec

#define TX GP0	         // Serial data output
#define SCL TRISIO1
#define SDA TRISIO2
#define GP_SCL GP1
#define GP_SDA GP2
#define I2C_MSK 11111001B // This mask makes sure GPIO states do not mess with I2C TRISIO I/O
    
// HDC1080 register
#define TEMP    0x00                         // 2 bytes T°C
#define HUMY    0x01                         // 2 bytes %
#define CONFIG  0x02                         // 2 bytes config and status
#define SERID1  0xfb                         // 2 bytes upper serial ID
#define SERID2  0xfc                         // 2 bytes mid   serial ID
#define SERID3  0xfd                         // 2 bytes last  serial ID
#define MANUFID	0xfe                         // 2 bytes Manufacturer ID (0x5449  TI)
#define DEVICID 0xff                         // 2 bytes device ID (0x1050))

SYS_CLOCK   equ 4000000
INST_CLOCK  equ SYS_CLOCK/4
  
; string address in Eeprom
I2CER equ 0x00
MANID equ 0x10
DEVID equ 0x19
UNKNO equ 0x22
TEXIN equ 0x2b
HDC10 equ 0x37
OSCAV equ 0x3F

; HDC1080 I2C address = 1000000 (0x40 in 7bits)
WADDR equ 0x80
RADDR equ 0x81
  
PSECT udata_bank0
byte:
    DS 1
st1:
    DS 1
st2:
    DS 1
st3:
    DS 1
st4:
    DS 1
tmp0:
    DS 1
tmp1:
    DS 1
tmp2:
    DS 1
cval1:
    DS 1
cval2:
    DS 1
txData:
    DS 1
icount:
    DS 1
ocount:
    DS 1
count:
    DS 1
pos:
    DS 1
idx:
    DS 1
sign:
    DS 1
mask:
    DS 1
asciiN:
    DS 1
    
; align tables to a 32-word boundary for safety
; PCLATH initialization mandatory with computed goto
PSECT hexTbl,class=CODE,space=SPACE_CODE,delta=2,reloc=0x20
hexDisp:
    addwf PCL
    IRP hexd,'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'
    RETLW hexd
    ENDM

psect edata,class=EEDATA,space=SPACE_EEPROM,delta=2,noexec
    DB 'I','2','C',0x20,'n','o','t',0x20,'r','e','a','d','y',0x0d,0x0a,0
    DB 'M','a','n','.','I','D',':',0x20,0
    DB 'D','e','v','.','I','D',':',0x20,0
    DB 'U','n','k','n','o','w','n',0x21,0
    DB 'T','e','x','a','s',0x20,'I','n','s','t','.',0
    DB 'H','D','C','1','0','8','0',0
    DB 'O','s','c','a','l',':',0x20,0


PSECT resetVec,class=CODE,delta=2 	; resetVec set @ 0x0000 in linker command line
resetVec:
    ljmp setup				; A ljmp, like fcall, performs the pagsel automatically

PSECT isrVec,class=CODE,delta=2         ; Interrupt set @ 0x0004 in linker command line
Int:
exitISR:
    retfie

PSECT initChip,class=CODE,delta=2
init:
    bsf RP0             ; get oscillator calibration data 
    call 0x03ff
    movwf OSCCAL
    
    bcf STATUS, 5       ; manual BANKSEL method 1 : set RP0 = 0 = Bank 0
    clrf GPIO
    movlw 07h           ; 00000111B    
    movwf CMCON 	; comparator off, CIN & COUT pins as I/O
    bsf RP0
    clrf ANSEL          ; disable analog ports
    bcf RP0
    return
    
PSECT code
setup:
    call init
    PAGESEL $          ; not needed on mid-range 12F675 - reminder for other pics
    BANKSEL TRISIO
    bcf TRISIO0
    bcf TRISIO4
    bsf SCL            ; GP1 & GP2 as input, highZ
    bsf SDA
    BANKSEL GPIO
    bsf TX
    movlw 15           ; HDC1080 stabilization delay cf p. 10
    call delay_ms      ; 
    
main:
    bcf GP4    
    
    ; print 12F675 Osc calibration value - debug
    ;--------------------------------
    ;movlw OSCAV
    ;call readProm
    ;movlw '0'
    ;movwf txData
    ;call uart_send
    ;movlw 'x'
    ;movwf txData
    ;call uart_send
    ;bsf RP0
    ;movf OSCCAL, w
    ;bcf RP0
    ;call hexConv 
    ;call crlf
    ;------------------------------   
    
    ; get Manuf. ID register
    call i2c_start
    movlw WADDR
    call i2c_send
    movlw MANUFID
    call i2c_send
    call i2c_start
    movlw RADDR
    call i2c_send
    call i2c_recvack
    movf byte, w
    movwf st1
    call i2c_recvnack
    movf byte, w
    movwf st2
    call i2c_stop
    
    ; print Manuf. ID
    movlw MANID
    call readProm
    movlw 0x54
    subwf st1
    btfss ZERO
    goto unknown1
    movlw 0x49
    subwf st2
    btfss ZERO
    goto unknown1
    movlw TEXIN
    call readProm
    goto $ + 3
unknown1:
    movlw UNKNO
    call readProm
    call crlf
    
    ; get Dev. ID register
    call i2c_start
    movlw WADDR
    call i2c_send
    movlw DEVICID
    call i2c_send
    call i2c_start
    movlw RADDR
    call i2c_send
    call i2c_recvack
    movf byte, w
    movwf st1
    call i2c_recvnack
    movf byte, w
    movwf st2
    call i2c_stop
    
    ; print Dev. ID
    movlw DEVID
    call readProm
    movlw 0x10
    subwf st1
    btfss ZERO
    goto unknown2
    movlw 0x50
    subwf st2
    btfss ZERO
    goto unknown2
    movlw HDC10
    call readProm
    goto $ + 3
unknown2:
    movlw UNKNO
    call readProm
    call crlf
    
    ; get raw temperature and humidity data
measure:
    call i2c_start
    movlw WADDR
    call i2c_send
    movlw TEMP
    call i2c_send
    movlw 20
    call delay_ms
    call i2c_start
    movlw RADDR
    call i2c_send
    call i2c_recvack
    movf byte, w
    movwf st1
    call i2c_recvack
    movf byte, w
    movwf st2
    call i2c_recvack
    movf byte, w
    movwf st3
    call i2c_recvnack
    movf byte, w
    movwf st4
    call i2c_stop
    
    movlw 'T'
    movwf txData
    call uart_send
    movlw 'C'
    movwf txData
    call uart_send
    movlw ':'
    movwf txData
    call uart_send
    movlw 0x20
    movwf txData
    call uart_send
    movlw '0'
    movwf txData
    call uart_send
    movlw 'x'
    movwf txData
    call uart_send
    movf st1, w
    call hexConv
    movf st2, w
    call hexConv
    call crlf
    
    movlw 'R'
    movwf txData
    call uart_send
    movlw 'H'
    movwf txData
    call uart_send
    movlw ':'
    movwf txData
    call uart_send
    movlw 0x20
    movwf txData
    call uart_send
    movlw '0'
    movwf txData
    call uart_send
    movlw 'x'
    movwf txData
    call uart_send
    movf st3, w
    call hexConv
    movf st4, w
    call hexConv
    call crlf
    
    ; apply transfert function for T°C with shift >> 2 (right adjust result to 14bits)
    ; 1 lsb * 165 = 0.01007
    ; debug : 0x6c24 = 29.70
    ; movlw 0x6b
    ; movwf st1
    ; movlw 0xdc
    ; movwf st2
    bcf CARRY
    rrf st1
    rrf st2
    bcf CARRY
    rrf st1
    rrf st2
    movf st1, w
    movwf tmp1
    movwf cval1
    movf st2, w
    movwf tmp2
    movwf cval2
    movlw 7
    movwf idx
    ; divide result by 128 and add to res
    call shiftR
    call addV
    
    movf st1, w
    movwf tmp1
    movf st2, w
    movwf tmp2
    movlw 10
    movwf idx
    ; divide result by 1024 and substract to res
    call shiftR
    call subV
    
    movf st1, w
    movwf tmp1
    movf st2, w
    movwf tmp2
    movlw 12
    movwf idx
    ; divide result by 4096 and add to res
    call shiftR
    call addV
    ; substract res - 4000
    movlw 0x0f
    movwf tmp1
    movlw 0xa0
    movwf tmp2
    call subV
    ; sign handling
    btfss cval1, 7
    movlw 0x2b
    btfsc cval1, 7
    movlw 0x2d
    movwf sign
    ; absolute value
    btfss cval1, 7
    goto $ + 7
    comf cval2
    comf cval1
    movlw 1                     ; 2' complement
    addwf cval2
    btfsc CARRY                 ; propagate CARRY
    incf cval1
    nop
    call bcdConv
    
    ; display temperature in decimal
    movlw 'T'
    movwf txData
    call uart_send
    movlw 'C'
    movwf txData
    call uart_send
    movlw ':'
    movwf txData
    call uart_send
    movlw 0x20
    movwf txData
    call uart_send
    movf sign, w
    movwf txData
    call uart_send
    call dispDec
    call crlf
    
    ; apply transfert function for %RH with shift >> 3 (right adjust result to 13bits)
    ; 1 lsb * 100 = 0.012207 : correction = 1/4 - 1/32 = 0,21875 -> almost 0,22070
    ; debug : 0x8954 = 53.64
    ; movlw 0xbc
    ; movwf st3
    ; movlw 0x91
    ; movwf st4
    bcf CARRY
    rrf st3
    rrf st4
    bcf CARRY
    rrf st3
    rrf st4
    bcf CARRY
    rrf st3
    rrf st4
    movf st3, w
    movwf tmp1
    movwf cval1
    movf st4, w
    movwf tmp2
    movwf cval2    
    movlw 2
    movwf idx
    ; divide result by 4 and add to res
    call shiftR
    call addV
    
    movf st3, w
    movwf tmp1
    movf st4, w
    movwf tmp2
    movlw 5
    movwf idx
    ; divide result by 32 and substract to res
    call shiftR
    call subV
    
    movf st3, w
    movwf tmp1
    movf st4, w
    movwf tmp2
    movlw 9
    movwf idx
    ; divide result by 512 and add to res
    call shiftR
    call addV
    nop
    call bcdConv
     
    ; display humidity in decimal
    movlw 'R'
    movwf txData
    call uart_send
    movlw 'H'
    movwf txData
    call uart_send
    movlw ':'
    movwf txData
    call uart_send
    movlw 0x20
    movwf txData
    call uart_send
    movlw 0x25
    movwf txData
    call uart_send
    call dispDec
    call crlf    
 
endMain:   
    call crlf
    movlw 12
    movwf idx
delayl:
    movlw 255
    call delay_ms
    decf idx
    btfsc ZERO
    goto measure
    goto delayl

PSECT Routines,class=CODE,delta=2        
crlf:
    movlw 0x0d
    movwf txData
    call uart_send
    movlw 0x0a
    movwf txData
    call uart_send
    return
    
shiftR:
    bcf CARRY
    rrf tmp1
    rrf tmp2
    decf idx
    btfsc ZERO
    return
    goto shiftR
    
addV:
    movf tmp2, w
    addwf cval2
    btfsc CARRY
    incf cval1
    movf tmp1, w
    addwf cval1
    return

subV:
    movf tmp2, w
    subwf cval2
    btfss CARRY              ;f - w => C=1 if positive result
    decf cval1
    movf tmp1, w
    subwf cval1
    return

PSECT Decimal,class=CODE,delta=2 
dispDec:
    ; test & disp 1st digit (1 or 0)
    movlw 0x30
    movwf asciiN
    movlw 0x0f
    movwf mask
    btfsc tmp0, 0
    movlw 0x31
    btfss tmp0, 0
    movlw 0x20
    movwf txData
    call uart_send
    ; disp digit 2 & 3
    swapf tmp1, w
    andwf mask, w
    addwf asciiN, w
    movwf txData
    call uart_send
    movf tmp1, w
    andwf mask, w
    addwf asciiN, w
    movwf txData
    call uart_send
    movlw 0x2e
    movwf txData
    call uart_send
    ; disp digit 4 & 5
    swapf tmp2, w
    andwf mask, w
    addwf asciiN, w
    movwf txData
    call uart_send
    movf tmp2, w
    andwf mask, w
    addwf asciiN, w
    movwf txData
    call uart_send
    return

PSECT Hexconv,class=CODE,delta=2
hexConv:
    movwf byte
    swapf byte
    movlw HIGH(hexDisp)
    movwf PCLATH
    movlw 0x0f
    andwf byte, w
    call hexDisp
    movwf txData
    call uart_send
    swapf byte
    movlw HIGH(hexDisp)
    movwf PCLATH
    movlw 0x0f
    andwf byte, w
    call hexDisp
    movwf txData
    call uart_send    
    return

; from AN526 PIC16C5X Math Utility Routines p.31 
; appendix H:BINARY (16-BIT) TO BCD LISTING
PSECT BCDconv, class=CODE,delta=2
bcdConv:
    bcf CARRY
    movlw 16
    movwf idx
    clrf tmp0
    clrf tmp1
    clrf tmp2
loop16:
   rlf cval2
   rlf cval1
   rlf tmp2
   rlf tmp1
   rlf tmp0
   decfsz idx
   goto initBcd
   return

initBcd:
    movlw tmp2
    movwf FSR
    call bcdDo
    movlw tmp1
    movwf FSR
    call bcdDo
    movlw tmp0
    movwf FSR
    call bcdDo
    goto loop16

bcdDo:
    movlw 0x03
    addwf INDF, w
    movwf byte
    btfsc byte, 3 ; test if result > 7
    movwf INDF
    movlw 0x30
    addwf INDF, w
    movwf byte
    btfsc byte, 7 ; test if result > 7
    movwf INDF    ; save as MSD
    return
    
PSECT Msg,class=CODE,delta=2
errMsg:
    movlw 0x00
    call readProm
    BANKSEL GPIO
    bsf GP4
    movlw 255
    call delay_ms
    goto main
    
PSECT Eeprom,class=CODE,delta=2
readProm:
    movwf pos
loopRp:
    movf pos, w
    bsf RP0
    movwf EEADR
    bsf RD
    movf EEDATA, w
    bcf RP0
    movwf txData
    andlw 255            ; is data = 0 ?
    btfsc ZERO           ; test ZERO bit status - end-of-string
    goto exitRp   
    call uart_send
    incf pos
    goto loopRp
exitRp:
    return
 
PSECT I2Croutines,class=CODE,delta=2
i2c_start:
    BANKSEL TRISIO
    bsf SCL
    bsf SDA
    nop
    nop
    BANKSEL GPIO
    movlw I2C_MSK
    andwf GPIO, f 
    BANKSEL TRISIO
    bcf SDA
    nop
    bcf SCL
    return
    
i2c_stop:
    BANKSEL TRISIO
    bsf SCL
    nop
    bsf SDA
    call delay_i2c
    return
    
i2c_send:
    movwf byte
    BANKSEL GPIO
    movlw I2C_MSK      ; Reset SCL & SDA GPIO while keeping other GP's status
    andwf GPIO, f
    BANKSEL TRISIO
    bcf SCL
    bcf SDA    
    movlw 8
    movwf idx
    BANKSEL GPIO
bit_send:
    rlf byte            ; IMPORTANT ! Transmit msb first
    btfsc CARRY
    bsf GP_SDA          ; High
    btfss CARRY
    bcf GP_SDA          ; Low
    bsf GP_SCL           ; clocking
    nop
    nop
    nop
    bcf GP_SCL
    bcf GP_SDA
    decfsz idx
    goto bit_send
ack:
    BANKSEL TRISIO
    bsf SDA            ; release data line
    call delay_i2c
    bsf SCL
    BANKSEL GPIO
    btfsc GP_SDA
    goto i2c_error     ; goto i2c error routine
    BANKSEL GPIO
    movlw I2C_MSK
    andwf GPIO, f
    BANKSEL TRISIO
    bcf SCL
    bcf SDA
    return
    
i2c_recvack:
    BANKSEL GPIO
    movlw I2C_MSK
    andwf GPIO, f
    BANKSEL TRISIO
    bcf SCL
    bsf SDA              ; release the data bus
    clrf byte
    movlw 8
    movwf idx
looprcva:
    BANKSEL GPIO
    bsf GP_SCL    
    btfss GP_SDA           ; test SDA GPIO status line
    bcf CARRY
    btfsc GP_SDA
    bsf CARRY
    bcf GP_SCL
    rlf byte
    decfsz idx
    goto looprcva
 ackd:
    movlw I2C_MSK
    andwf GPIO, f
    BANKSEL TRISIO
    bcf SDA
    nop
    bsf SCL
    call delay_i2c
    bcf SCL
    nop
    return
    
i2c_recvnack:
    BANKSEL GPIO
    movlw I2C_MSK
    andwf GPIO, f
    BANKSEL TRISIO
    bcf SCL
    bsf SDA              ; release the data bus
    clrf byte
    movlw 8
    movwf idx    
looprcvna: 
    BANKSEL GPIO
    bsf GP_SCL
    btfss GP_SDA
    bcf CARRY
    btfsc GP_SDA
    bsf CARRY
    bcf GP_SCL
    rlf byte
    decfsz idx
    goto looprcvna
nackd:
    BANKSEL GPIO
    movlw I2C_MSK
    andwf GPIO, f
    BANKSEL TRISIO
    bsf SDA
    nop
    bsf SCL
    call delay_i2c
    bcf SCL
    nop
    bcf SDA
    return

i2c_error:
    BANKSEL GPIO
    bsf GP4
    movlw I2CER
    call readProm
    call crlf
    movlw 250
    call delay_ms
    movlw 250
    call delay_ms
    goto setup
    

PSECT serialTX,class=CODE,delta=2
uart_send:
    movlw 8
    movwf idx
    BANKSEL GPIO
    bcf TX            ; start bit
    call delay_uart
loop_bit:
    rrf txData
    btfss STATUS, 0
    bcf TX
    btfsc STATUS, 0
    bsf TX
    call delay_uart
    decfsz idx
    goto loop_bit
    bsf TX           ; stop bit
    call delay_uart
    return
    
PSECT delays,class=CODE,delta=2
delay_ms:             ; 1-255 ms (approx)
    bcf RP0
    movwf ocount
    movlw 255
    movwf icount
loop_i:
    nop
    decfsz icount
    goto loop_i
    movlw 255
    movwf icount
loop_o:
    decfsz ocount
    goto loop_i
    return
    
delay_i2c:                ; 5us between call and return
    nop
    return
    
delay_uart:                ; UART 9600 bauds delay = 104us
    movlw 31
    movwf icount
loop_uart:
    decfsz icount
    goto loop_uart
    return


END resetVec
    