/* YourDuino SoftwareSerialExample1
   - Connect to another Arduino running "YD_SoftwareSerialExampleRS485_1Remote"
   - Connect this unit Pins 10, 11, Gnd
   - Pin 3 used for RS485 direction control
   - To other unit Pins 11,10, Gnd  (Cross over)
   - Open Serial Monitor, type in top window. 
   - Should see same characters echoed back from remote Arduino

   Questions: terry@yourduino.com 
*/

/*-----( Import needed libraries )-----*/
#include <SoftwareSerial.h>
/*-----( Declare Constants and Pin Numbers )-----*/
#define SSerialRX        A1  //Serial Receive pin
#define SSerialTX        A0  //Serial Transmit pin

#define SSerialTxControl 3   //RS485 Direction control

#define RS485Transmit    HIGH
#define RS485Receive     LOW

#define Pin13LED         13

#define BUTTON_DEBOUNCE 5

#define CLEAR_TIMEOUT 30000

/*-----( Declare objects )-----*/
SoftwareSerial RS485Serial(SSerialRX, SSerialTX); // RX, TX

/*-----( Declare Variables )-----*/
int byteReceived;
int byteSend;

const int buttonPin = 3;     // the number of the pushbutton pin

int CURR_STATE = 0;

// Variables will change:
int ledState = HIGH;         // the current state of the output pin
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

unsigned long lastMsgTime = 0;
bool isCleared = false;

char cmds[] = "012434";

char bass[3][2] = {
  "a0",
  "a1",
  "aa"
};

char board[20] = {
  'a',
  'b',
  'c',
  'd',
  't',
  'l',
  'g',
  'e',
  'n',
  's',
  'j',
  'r',
  'u',
  'h',
  'i',
  'm',
  'f',
  'o',
  'q',
  'p'
};

void setup()   /****** SETUP: RUNS ONCE ******/
{

  // Start the built-in serial port, probably to Serial Monitor
  Serial.begin(57600); //19200);

  Serial.println("YourDuino.com SoftwareSerial remote loop example");
  Serial.println("Use Serial Monitor, type in upper window, ENTER");

  pinMode(buttonPin, INPUT);

  pinMode(SSerialTxControl, OUTPUT);
  digitalWrite(SSerialTxControl, RS485Receive);  // Init Transceiver   

  // Start the software serial port, to another device
  RS485Serial.begin(19200);   // set the data rate 

}//--(end setup )---

void writeCmd(char _cmd, char _board, char _index)
{
    RS485Serial.write(_cmd);
    delay(10);
    RS485Serial.write(_board);  // Disable RS485 Transm
    delay(10);
    RS485Serial.write(_index);
    delay(10);
    RS485Serial.write('\n');
    delay(10);
}

void loop()   /****** LOOP: RUNS CONSTANTLY ******/
{

  // read the state of the switch into a local variable:
  int reading = digitalRead(buttonPin);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:
    // if the button state has changed:

    if (reading != buttonState) {

      //Serial.println("CHANGE");
      buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState == HIGH) {
 
        Serial.println("1\n");
        
        CURR_STATE++;
        if (CURR_STATE > 4) {
           CURR_STATE = 0;
        }
 
      }
    }
  }

  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonState = reading;

  if (Serial.available())
  {
      lastMsgTime = millis();
      isCleared = false;

      byteReceived = Serial.read();
 
      RS485Serial.write(byteReceived);
      delay(5);  
  }

  //Make sure these guys all cleared if there's a crash or something
  if (!isCleared && millis() > lastMsgTime + CLEAR_TIMEOUT)
  {

    isCleared = true;
    lastMsgTime = millis();

    RS485Serial.write('4');
    delay(10);
    RS485Serial.write('\n');
    delay(10);
  }
  
}//--(end main loop )---


/*-----( Declare User-written Functions )-----*/

//NONE
//*********( THE END )***********
