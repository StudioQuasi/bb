/* YourDuino SoftwareSerialExample1Remote
   - Used with YD_SoftwareSerialExampleRS485_1 on another Arduino
   - Remote: Receive data, loop it back...
   - Connect this unit Pins 10, 11, Gnd
   - To other unit Pins 11,10, Gnd  (Cross over)
   - Pin 3 used for RS485 direction control   
   - Pin 13 LED blinks when data is received  
   
   Questions: terry@yourduino.com 
*/

/*-----( Import needed libraries )-----*/
#include <SoftwareSerial.h>
/*-----( Declare Constants and Pin Numbers )-----*/
#define SSerialRX        A3  //Serial Receive pin
#define SSerialTX        A2  //Serial Transmit pin

#define SSerialTxControl 3   //RS485 Direction control
#define RS485Transmit    HIGH
#define RS485Receive     LOW

#define Pin13LED         13

/*-----( Declare objects )-----*/
SoftwareSerial RS485Serial(SSerialRX, SSerialTX); // RX, TX

/*-----( Declare Variables )-----*/
int byteReceived;
int byteSend;

int _readIndex = 0;
byte _input[32];

byte _addr = 4;

void setup()   /****** SETUP: RUNS ONCE ******/
{
  // Start the built-in serial port, probably to Serial Monitor
  //Serial.begin(9600);
  //Serial.println("SerialRemote");  // Can be ignored

  pinMode(Pin13LED, OUTPUT);

  //pinMode(SSerialTxControl, OUTPUT);
  //digitalWrite(SSerialTxControl, RS485Receive);  // Init Transceiver

  // Start the software serial port, to another device
  Serial.begin(9600);   // set the data rate

  digitalWrite(Pin13LED, HIGH);  // Show activity
  delay(1000);
  digitalWrite(Pin13LED, LOW);
  delay(500);
        
}
//--(end setup )---


void loop()   /****** LOOP: RUNS CONSTANTLY ******/
{
  //Copy input data to output  
  if (Serial.available())
  {

    byteReceived = Serial.read();   // Read the byte 

    //IF END OF SERIAL STRING
    if (byteReceived == '\n') {
          
      if (_input[0] == _addr) {

        for (int i=0; i<_input[1]; i++)
        {
          digitalWrite(Pin13LED, HIGH);  // Show activity
          delay(100);
          digitalWrite(Pin13LED, LOW);
          delay(100);
        }

      }
   
      _readIndex = 0;
      
    //
    } else {

      int _num = byteReceived - '0';
      _input[_readIndex++] = _num;
          
    }

    /*
    digitalWrite(SSerialTxControl, RS485Transmit);  // Enable RS485 Transmit    
    RS485Serial.write(byteSend); // Send the byte back
    delay(10);
    digitalWrite(SSerialTxControl, RS485Receive);  // Disable RS485 Transmit      
//    delay(100);
    */
  }// End If RS485SerialAvailable

}//--(end main loop )---

/*-----( Declare User-written Functions )-----*/
//NONE
