
/* **************************
    * By Midelic on RCGroups *
    **************************
    This project is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    
    MiLo Rx code is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this code.  If not, see <http://www.gnu.org/licenses/>.
*/
#define DEBUG
#define DEBUG_EEPROM
//#define DEBUG_LOOP_TIMING  
#define DEBUG_BIND
//#define DEBUG_DATA
//#define DEBUG_SPORT
#define DEBUG_ON_GPIO3
#define DEBUG_SIM_SPORT_SENSOR



#ifdef ESP8266
	#define ESP8266_PLATFORM
#endif
//----Modules defs--------
//#define EL24P//flywoo RX
//#define DIY_RX
//#define MATEK_RX_R24D
//#define MATEK_RX_R24S
//define BETA_FPV_RX_NANO
//#define NAMIMNO_RX_NANO_FLASH
#define ESP8266_E28_2G4M20S


#define MSW_SERIAL
//#define SW_SERIAL

//#define HC_BIND
//#define USER_MAX_POWER
//#define TELEMETRY // mstrens removed to test
//#define HC_SPORT

#if defined MATEK_RX_R24D ||defined NAMIMNO_RX_NANO_FLASH || defined DIY_RX
  #define DIVERSITY
#endif

//#define SWAMPING
//#define RSSI_AVG
//#define SPORT_TELEMETRY // mstrens removed to test
#define FAILSAFE

//#define MinPower PWR_10mW
//#define MaxPower PWR_100mW
#if defined MATEK_RX_R24D ||defined NAMIMNO_RX_NANO_FLASH || defined MATEK_RX_R24S || defined BETA_FPV_RX_NANO 
    #define HAS_PA_LNA
    #define MinPower -10//10mW//  powerValues[4]= [-10,-6,-3,1];(Beta_FPV&Matek)//powerValues[4]=[-15,-10,-7,-3];(NAMIMNO_RX_NANO_FLASH)
    #define MaxPower 1//100mW
    #ifdef USER_MAX_POWER
        #define UserPower -10//10mW for example can be defined whatever you need
    #endif
#endif

#ifdef ESP8266_E28_2G4M20S
    #define HAS_PA_LNA
    #define MinPower -13//10mW
    #define MaxPower -2//100mW
    #define USER_MAX_POWER -13 //10mW for example can be defined whatever you need
#endif

#if defined EL24P || defined DIY_RX//No PA/LNA
    #define MinPower 10 // 10mW
    #define MaxPower 13 // 20mW
    #define USER_MAX_POWER 10 //10mW for example can be defined whatever you need
#endif

//#define SBUS // mstrens removed to test
//#define TX_FAILSAFE
//#define PWM_SERVO
//#define ADC_VOLT
#define STATISTIC
//#define SERVO_RATE
//#define PARALLEL_SERVO
//#define EU_LBT
//#define USE_WIFI

#ifdef USE_WIFI
    #include "devWIFI_elegantOTA.h"
#endif

#ifdef DEBUG_ON_GPIO3
 #define G3ON digitalWrite(3,HIGH)
 #define G3OFF digitalWrite(3,LOW)
 #define G3TOGGLE digitalWrite(3,!digitalRead(3))
 #define G3PULSE(usec) digitalWrite(3,HIGH);delayMicroseconds(usec); digitalWrite(3,LOW)
#else
 #define G3ON 
 #define G3OFF 
 #define G3TOGGLE 
 #define G3PULSE(usec) 
#endif
/*
    Protocol description:
    2.4Ghz LORA modulation
    - 142 Hz frame rate(7ms)
    - Data Rate ~76kb/s(-108dBm)
    - Bw-812; SF6 ; CR -LI -4/7 .
    - Preamble length 12 symbols
    - Fixed length packet format(implicit) -15 bytes
    - Downlink telemetry rate(1:3)
    - Uplink telemetry rate(1:6)
    - Hardware CRC is ON.
    
    # Normal frame channels 1-8; frame rate 7ms.
    
    0. next expected telemetry down link frame counter(sequence) (bits 7..4 (4 bits=16 val)) | synchro channel (bit 3) | Frame type(bits 2..0 (3 lsb bits))
    1. txid1 TXID on 16 bits
    2. txid2
    3. flag next frame must be dwn tlm frame (bit 7) | flag requesing starting WIFI (bit 6) | Model ID /Rx_Num(bits 5....0 = 6 bits) 
    4. channels 8 channels/frame ; 11bits/channel
    5. channels total 11 bytes of channels data in the packet frame
    6. channels
    7. channels
    8. channels
    9. channels
    10. channels
    11. channels
    12. channels
    13. channels
    14. channels ;15 bytes payload frame
    
    # Normal frame channels 9-16 separate; frame rate 7ms.
    0. next expected telemetry down link frame counter(sequence) (bits 7..4 (4 bits=16 val)) | synchro channel (bit 3) | Frame type(bits 2..0 (3 lsb bits))
    1. txid1 TXID on 16 bits
    2. txid2
    3. flag next frame must be dwn tlm frame (bit 7) | flag requesing starting WIFI (bit 6) | Model ID /Rx_Num(bits 5....0 = 6 bits) 
    4. channels 8 channels/frame ; 11bits/channel
    5. channels total 11 bytes of channels data in the packet frame
    6. channels
    7. channels
    8. channels
    9. channels
    10. channels
    11. channels
    12. channels
    13. channels
    14. channels ;15 bytes payload frame
    
    # TX uplink telemetry frame can be sent separate ;frame rate 7ms;1:6 telemetry data rate.
    0. next expected telemetry down link frame counter(sequence) (bits 7..4 (4 bits=16 val)) | synchro channel (bit 3) | Frame type(bits 2..0 (3 lsb bits))
    1. txid1 TXID on 16 bits
    2. txid2
    3. no. of bytes in sport frame(on max 4bits) | telemetry uplink counter sequence(4 bits)
    4.Sport data byte1
    5.Sport data byte 2
    6.Sport data byte 3
    7.Sport data byte 4
    8.SPort data byte 5
    9.SPort data byte 6
    10.SPort data byte 7
    11.SPort data byte 8
    12.SPort data byte 9
    13.SPort data byte 10
    14.SPort data byte 11 ;15bytes payload/11 bytes sport telemetry
    
    # RX downlink telemetry frame sent separate at a fixed rate of 1:3;frame rate 7ms.	
    0. - bits 7...4 : No. of bytes in sport frame(4 bits)
       - bits 3...2 : unused (2 bits) 
       - bitss 1...0 : type of link data packet(RSSI/SNR /LQI) (2 bits= 3 values currently) 
    1.txid1
    2.txid2
    3.  - bits 7...4 : current downlink tlm counter (4 bits); when received TX should send this counter + 1
        - bits 3...0 : last upllink tlm counter received(4 bits)
    4.RSSI/LQI/SNR alternate every ~80 ms update for each data
    5.Sport data byte1
    6.Sport data byte2
    7.Sport data byte3
    8.Sport data byte4
    9.Sport data byte5
    10.Sport data byte6
    11.Sport data byte7;
    12.Sport data byte8;
    13.Sport data byte9
    14.Sport data byte10; 15 bytes payload;10 bytes sport telemetry
    
    # bind packet
    0. Frame type = BIND_PACKET = 0
    1. rx_tx_addr[3];
    2. rx_tx_addr[2];
    3. rx_tx_addr[1];
    4. rx_tx_addr[0];
    5. RX_num;
    6. chanskip;
    7. up to 14.  0xA7


    # Frame Sequence
    0- downlink telemetry
    1- RC channels 1_8_1 
    2- RC channels 9_16
    3- downlink telemetry
    4- RC channels 1_8_2
    5 -uplink telemetry                 
    6- downlink telemetry
    7- RC channels 9_16
    8- RC channels 1_8_1
    9- downlink telemetry
    10- RC channels 9_16
    11- uplink telemetry               
    12- downlink telemetry
    13- RC channels 1_8_2
    14- RC channels 9_16
    15- downlink telemetry
    
    
    0 - downlink telemetry
    1- RC channels 1_8_1         
    2- RC channels 1_8_2      
    3- downlink telemetry      
    4- RC channels 1_8_1       
    5 -uplink telemetry              
    6- downlink telemetry      
    7- RC channels 1_8_2         
    8- RC channels 1_8_1          
    9- downlink telemetry            
    10- RC channels 1_8_2       
    11- uplink telemetry           
    12- downlink telemetry
    13- RC channels 1_8_1            
    14- RC channels 1_8_2              
    15- downlink telemetry
    16- RC channels 1_8_1            
    17  uplink telemetry              
    15- downlink telemetry
    
    
*/
