//Library:
#include <Ticker.h>
#include <SX1278.h>

//Child Parameter:
#define Child_1
//#define Child_2
//#define Child_3
//#define Child_4

//Lora SX1278:
#define LORA_MODE             10            //mode: mode number to set the required BW, SF and CR of LoRa modem.
#define LORA_CHANNEL          CH_6_BW_125
uint8_t ControllerAddress = 5;              //Parent Address

#ifdef Child_1
#define LORA_ADDRESS          3
#endif

#ifdef Child_2
#define LORA_ADDRESS          4
#endif

#ifdef Child_3
#define LORA_ADDRESS          6
#endif

#ifdef Child_4
#define LORA_ADDRESS          7
#endif

//Message var:
char my_packet [50];  //Used to store Incoming message in char array from Parent Node
char testData[50];    //Used to store message which will be sent to Parent Node
String receivedMsg;   //to store the converted my_packet string from char array

//Pin Def:
int ButtonPIN = 2;
int LED = 4;
int GreenLED = 5;

//Flag:
boolean T_ISR_F = false;
int T_packet_state;
int R_packet_state;
boolean FunctionBlockingFlag = true;

//Debouncing timer
long debouncing_time = 3000;
volatile unsigned long last_micros;

// timer for sending data:
long interval = 2000;
unsigned long last_interval = 0;
unsigned long current_millis;

Ticker wait1sec;
#define DEBUG

void setup() {
  //Serial communication begin:
#ifdef DEBUG
  Serial.begin(9600);
#endif
  //Lora init:
  loraSetup();
  //Pin config:
  pinConfiguration();
  //Interrupt:
  attachInterrupt(digitalPinToInterrupt(ButtonPIN), Trigger_ISR, FALLING);
#ifdef DEBUG
  Serial.print("Lora Address :");
  Serial.println(LORA_ADDRESS);
#endif
  //turnOff Led:
  ledOff();
}

void loop() {

  recieveData();

  if (!FunctionBlockingFlag) {
    unsigned long current_millis = millis();
    if ((unsigned long ) (current_millis - last_interval) >= interval) {
      childTask();
      last_interval = millis();
    }
  }
}

void childTask() {
#ifdef Child_1
  if ( (receivedMsg.equals("GL1") || receivedMsg.equals("RL1")) /*&& T_ISR_F*/ && !FunctionBlockingFlag) {
    String("KL1").toCharArray(testData, 50);
    sendData(testData);
  }
#endif

#ifdef Child_2
  if ( (receivedMsg.equals("GL2") || receivedMsg.equals("RL2")) /*&& T_ISR_F*/ && !FunctionBlockingFlag) {
    String("KL2").toCharArray(testData, 50);
    sendData(testData);
  }
#endif

#ifdef Child_3
  if ( (receivedMsg.equals("GL3") || receivedMsg.equals("RL3")) && T_ISR_F && !FunctionBlockingFlag) {
    String("KL3").toCharArray(testData, 50);
    sendData(testData);
  }
#endif

#ifdef Child_4
  if ( (receivedMsg.equals("GL4") || receivedMsg.equals("RL4")) && T_ISR_F && !FunctionBlockingFlag) {
    String("KL4").toCharArray(testData, 50);
    sendData(testData);
  }
#endif
}

void Trigger_ISR() {
  if ((long)(micros() - last_micros) >= debouncing_time * 1000) {
    T_ISR_F = true;
    last_micros = micros();
  } else {
#ifdef DEBUG
    Serial.println("Bounce");
#endif
  }
}

/*
  Function: Configures the module to transmit information and receive an ACK.
  Returns: Integer that determines if there has been any error [T_packet_state]
   state = 9  --> The ACK lost (no data available)
   state = 8  --> The ACK lost
   state = 7  --> The ACK destination incorrectly received
   state = 6  --> The ACK source incorrectly received
   state = 5  --> The ACK number incorrectly received
   state = 4  --> The ACK length incorrectly received
   state = 3  --> N-ACK received
   state = 2  --> The command has not been executed
   state = 1  --> There has been an error while executing the command
   state = 0  --> The command has been executed with no errors
*/

void sendData(char message[]) {
  delay(1000);
  T_packet_state = sx1278.sendPacketTimeoutACKRetries(ControllerAddress, message);
  if (T_packet_state == 0) {
#ifdef DEBUG
    //Serial.println(F("State = 0 --> Command Executed w no errors!"));
    Serial.println(F("Confirmation Packet sent....."));
#endif
    T_ISR_F = false;
  }
  FunctionBlockingFlag = true;
}

void recieveData() {
  R_packet_state = sx1278.receivePacketTimeoutACK();
  if (R_packet_state == 0) {
#ifdef DEBUG
    Serial.println(F("Package received!"));
#endif

    for (unsigned int i = 0; i < sx1278.packet_received.length; i++) {
      my_packet[i] = (char)sx1278.packet_received.data[i];
      yield();
    }
#ifdef DEBUG
    Serial.print(F("Message:  "));
    Serial.println(my_packet);
#endif
    receivedMsg = String(my_packet); //Converts CharArray to String
    Process();

    FunctionBlockingFlag = false;
  }
}


void Process() {
  //For Child 1
#ifdef Child_1
  if (receivedMsg.equals("GL1")) {
    digitalWrite(LED, HIGH); //To turn off RED LED
    digitalWrite(GreenLED, LOW); //To turn on Green LED
  }
  if (receivedMsg.equals("RL1")) {
    digitalWrite(LED, LOW); //Turns on RED LED
    digitalWrite(GreenLED, HIGH); //Turns off Green LED
  }
#endif

  //For Child 2
#ifdef Child_2
  if (receivedMsg.equals("GL2")) {
    digitalWrite(LED, HIGH); //To turn off RED LED
    digitalWrite(GreenLED, LOW); //To turn on Green LED
  }
  if (receivedMsg.equals("RL2")) {
    digitalWrite(LED, LOW); //Turns on RED LED
    digitalWrite(GreenLED, HIGH); //Turns off Green LED
  }
#endif

  //For Child 3
#ifdef Child_3
  if (receivedMsg.equals("GL3")) {
    digitalWrite(LED, HIGH); //To turn off RED LED
    digitalWrite(GreenLED, LOW); //To turn on Green LED
  }
  if (receivedMsg.equals("RL3")) {
    digitalWrite(LED, LOW); //Turns on RED LED
    digitalWrite(GreenLED, HIGH); //Turns off Green LED
  }
#endif

  //For Child 4
#ifdef Child_4
  if (receivedMsg.equals("GL4")) {
    digitalWrite(LED, HIGH); //To turn off RED LED
    digitalWrite(GreenLED, LOW); //To turn on Green LED
  }
  if (receivedMsg.equals("RL4")) {
    digitalWrite(LED, LOW); //Turns on RED LED
    digitalWrite(GreenLED, HIGH); //Turns off Green LED
  }
#endif

  //This one should be broadcasted
  if (receivedMsg.equals("S")) {
    ESP.restart();
  }
}

void loraSetup() {
#ifdef DEBUG
  Serial.println("");
#endif
  // Power ON the module:
  if (sx1278.ON() == 0) {
#ifdef DEBUG
    Serial.println(F("Setting power ON: SUCCESS "));
#endif
  } else {
#ifdef DEBUG
    Serial.println(F("Setting power ON: ERROR "));
#endif
  }

  // Set transmission mode and print the result:
  if (sx1278.setMode(LORA_MODE) == 0) {
#ifdef DEBUG
    Serial.println(F("Setting Mode: SUCCESS "));
#endif
  } else {
#ifdef DEBUG
    Serial.println(F("Setting Mode: ERROR "));
#endif
  }

  // Set header:
  if (sx1278.setHeaderON() == 0) {
#ifdef DEBUG
    Serial.println(F("Setting Header ON: SUCCESS "));
#endif
  } else {
#ifdef DEBUG
    Serial.println(F("Setting Header ON: ERROR "));
#endif
  }

  // Select frequency channel:
  if (sx1278.setChannel(LORA_CHANNEL) == 0) {
#ifdef DEBUG
    Serial.println(F("Setting Channel: SUCCESS "));
#endif
  } else {
#ifdef DEBUG
    Serial.println(F("Setting Channel: ERROR "));
#endif
  }

  // Set CRC:
  if (sx1278.setCRC_ON() == 0) {
#ifdef DEBUG
    Serial.println(F("Setting CRC ON: SUCCESS "));
#endif
  } else {
#ifdef DEBUG
    Serial.println(F("Setting CRC ON: ERROR "));
#endif
  }

  // Select output power (Max, High, Intermediate or Low)
  if (sx1278.setPower('M') == 0) {
#ifdef DEBUG
    Serial.println(F("Setting Power: SUCCESS "));
#endif
  } else {
#ifdef DEBUG
    Serial.println(F("Setting Power: ERROR "));
#endif
  }

  // Set the node address and print the result
  if (sx1278.setNodeAddress(LORA_ADDRESS) == 0) {
#ifdef DEBUG
    Serial.println(F("Setting node address: SUCCESS "));
#endif
  } else {
#ifdef DEBUG
    Serial.println(F("Setting node address: ERROR "));
#endif
  }

  // Print a success
#ifdef DEBUG
  Serial.println(F("SX1278 configured finished"));
  Serial.println();
#endif
}

void pinConfiguration() {
  pinMode(LED, OUTPUT);
  pinMode(0, OUTPUT);
  pinMode(GreenLED, OUTPUT);
  digitalWrite(GreenLED, LOW);
  // pinMode(GreenLED, mode);
  digitalWrite(0, LOW);
  pinMode(ButtonPIN, INPUT);
}

void ledOff() {
  digitalWrite(LED, LOW);
  digitalWrite(GreenLED, LOW);
}



