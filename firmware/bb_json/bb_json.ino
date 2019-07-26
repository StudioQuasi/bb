// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License
//
// This example shows how to deserialize a JSON document with ArduinoJson.
//
// https://arduinojson.org/v6/example/parser/

#include <ArduinoJson.h>

struct cmd {
  byte _cmd;
  long _time;
  byte _set; 
};

const int CMD_OPEN = 0;
const int CMD_CLOSE = 1;
const int CMD_TAIL_ON = 2;
const int CMD_HEAD_ON = 3;
const int CMD_BODY_OFF = 4;

long  _runningTime;
int   nextCmdIndex;
int   numCmd;



cmd *arrCmd;

void setup() {

  // Initialize serial port
  Serial.begin(57600);
  while (!Serial) continue;

  StaticJsonDocument<500> objCmd;
  
  // Allocate the JSON document
  //
  // Inside the brackets, 200 is the capacity of the memory pool in bytes.
  // Don't forget to change this value to match your JSON document.
  // Use arduinojson.org/v6/assistant to compute the capacity.
  

  // StaticJsonDocument<N> allocates memory on the stack, it can be
  // replaced by DynamicJsonDocument which allocates in the heap.
  //
  // DynamicJsonDocument doc(200);

  // JSON input string.
  //
  // Using a char[], as shown here, enables the "zero-copy" mode. This mode uses
  // the minimal amount of memory because the JsonDocument stores pointers to
  // the input buffer.
  // If you use another type of input, ArduinoJson must copy the strings from
  // the input to the JsonDocument, so you need to increase the capacity of the
  // JsonDocument.

  char json[] = "["\
    "{\"cmd\":0,\"time\":0,\"set\":[0,1]},"\
    "{\"cmd\":1,\"time\":1000,\"set\":[0,1]},"\
    "{\"cmd\":2,\"time\":2000,\"set\":[0]},"\
    "{\"cmd\":3,\"time\":3000,\"set\":[1]},"\
    "{\"cmd\":4,\"time\":4000,\"set\":[1]},"\
    "{\"cmd\":5,\"time\":5000,\"set\":[1]},"\
    "{\"cmd\":6,\"time\":6000,\"set\":[1]},"\
    "{\"cmd\":7,\"time\":7000,\"set\":[1]}"\
  "]";
 
  // Deserialize the JSON document
  DeserializationError error = deserializeJson(objCmd, json);

  // Test if parsing succeeds.
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }

  //Get the size of the JSON
  numCmd = objCmd.size();

  //Create array of commands
  arrCmd = (cmd*)malloc(sizeof(cmd) * numCmd);

  //Create array of cmd objects
  for (int i=0; i<numCmd; i++) {
    
    arrCmd[i] = cmd {
      (byte)objCmd[i]["cmd"], 
      (long)objCmd[i]["time"], 
      (byte)objCmd[i]["set"][0]
    };

    Serial.println((byte)objCmd[i]["cmd"]);
  }
  
  // Fetch values.
  //
  // Most of the time, you can rely on the implicit casts.
  // In other case, you can do doc["time"].as<long>();
  /*
  const char* sensor = doc["sensor"];
  long time = doc[1]["time"];
  double latitude = doc[1]["data"][0];
  double longitude = doc[1]["data"][1];

  // Print values.
  Serial.println(sensor);
  Serial.println(time);
  Serial.println(latitude, 6);
  Serial.println(longitude, 6);
  */


  _runningTime = millis();
  nextCmdIndex = 0;

}

void loop() {

  // not used in this example
  if ((millis() > _runningTime + arrCmd[nextCmdIndex]._time) ) {

    Serial.println((_runningTime + arrCmd[nextCmdIndex]._time));
    Serial.println(millis());
    Serial.println(nextCmdIndex);
    Serial.println();
 
    //Serial.println(arrCmd[nextCmdIndex]._cmd);
    //Serial.println(arrCmd[nextCmdIndex]._set);

    //_runningTime = millis();
    nextCmdIndex++;

    if (nextCmdIndex >= numCmd) {
      Serial.println("DONE");
      nextCmdIndex = 0;
      _runningTime = millis();
    }

  }


}

// See also
// --------
//
// https://arduinojson.org/ contains the documentation for all the functions
// used above. It also includes an FAQ that will help you solve any
// deserialization problem.
//
// The book "Mastering ArduinoJson" contains a tutorial on deserialization.
// It begins with a simple example, like the one above, and then adds more
// features like deserializing directly from a file or an HTTP request.
// Learn more at https://arduinojson.org/book/
// Use the coupon code TWENTY for a 20% discount ❤❤❤❤❤
