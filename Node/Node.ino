//Library for Ticker
#include <Ticker.h>
#include <SX1278.h>

//Lora SX1278:
#define LORA_MODE             10
#define LORA_CHANNEL          CH_6_BW_125
#define LORA_ADDRESS          3
uint8_t ControllerAddress = 5;

//Message var:
char my_packet [50];
char testData[50];

//Pin Def:
int ButtonPIN = 2;
int LED = 4;
int GreenLED = 5;
// int NewButton = 4;

//Flag:
boolean T_ISR_F = false;
int T_packet_state;
int R_packet_state;
boolean FunctionBlockingFlag = true;

//Debouncing timer
long debouncing_time = 3000;
volatile unsigned long last_micros;

void setup() {
  Serial.begin(9600);

  //Lora init:
  loraSetup();

  //Pin config:
  pinMode(LED, OUTPUT);
  pinMode(0, OUTPUT);
  pinMode(GreenLED, OUTPUT);
  digitalWrite(GreenLED, LOW);
  // pinMode(GreenLED, mode);
  digitalWrite(0, LOW);
  pinMode(ButtonPIN, INPUT);
  //Interrupt:
  attachInterrupt(digitalPinToInterrupt(ButtonPIN), Trigger_ISR, FALLING);
}

void loop() {
  recieveData();
  delay(1000);
  // Serial.println(digitalRead(ButtonPIN));

  if (T_ISR_F && !FunctionBlockingFlag) {
    String("K").toCharArray(testData, 50);
    sendData(testData);
    delay(1000);
    //FailSafe:
    sendDataFailSafe();
  }
}

void Trigger_ISR() {
  if ((long)(micros() - last_micros) >= debouncing_time * 1000) {
    T_ISR_F = true;
    last_micros = micros();
  } else {
    Serial.println("Bounce");
  }
}

int sendData(char message[]) {
  T_packet_state = sx1278.sendPacketTimeoutACK(ControllerAddress, message);

  if (T_packet_state == 0)
  {
    //Serial.println(F("State = 0 --> Command Executed w no errors!"));
    Serial.println(F("Confirmation Packet sent....."));
    return FunctionBlockingFlag = true, T_ISR_F = false;;
  }

  else if (T_packet_state != 0) {

    if(T_packet_state == 9) {
      Serial.print(F("Packet not sent....."));
      Serial.println(T_packet_state);
      return T_packet_state;
    } 
    else if ( T_packet_state == 5 || T_packet_state == 4) {
      Serial.println("Controller is trying to send data!");
      Serial.println("Shifting to Receving mode");
      return FunctionBlockingFlag = true;
    }
    
  }
}

int sendDataFailSafe() {
  if ( T_packet_state == 9) {
    sendData(testData);
    delay(1000);
    if ( T_packet_state == 9) {
      loraSetupFT();
      delay(10);
      sendData(testData);
      delay(1000);
    }
  } else if (T_packet_state != 9) {
      recieveData();
      delay(500);
  }
}

int recieveData() {
  R_packet_state = sx1278.receivePacketTimeoutACK();

  if (R_packet_state == 0) {
    delay(10);
    Serial.println(F("Package received!"));

    for (unsigned int i = 0; i < sx1278.packet_received.length; i++) {
      my_packet[i] = (char)sx1278.packet_received.data[i];
      yield();
    }
    Serial.print(F("Message:  "));
    Serial.println(my_packet);

    Process();

    return FunctionBlockingFlag = false;
  }
}


void Process() {
  if (my_packet[0] == 'G') {
    digitalWrite(LED, HIGH);
    digitalWrite(GreenLED, LOW);
  } else if (my_packet[0] == 'R') {
    digitalWrite(LED, LOW);
    digitalWrite(GreenLED, HIGH);
  } else if (my_packet[0] == 'S') {
    ESP.restart();
  }
}

void loraSetup() {
  Serial.println("");
  // Power ON the module:
  if (sx1278.ON() == 0) {
    Serial.println(F("Setting power ON: SUCCESS "));
  } else {
    Serial.println(F("Setting power ON: ERROR "));
  }

  // Set transmission mode and print the result:
  if (sx1278.setMode(LORA_MODE) == 0) {
    Serial.println(F("Setting Mode: SUCCESS "));
  } else {
    Serial.println(F("Setting Mode: ERROR "));
  }

  // Set header:
  if (sx1278.setHeaderON() == 0) {
    Serial.println(F("Setting Header ON: SUCCESS "));
  } else {
    Serial.println(F("Setting Header ON: ERROR "));
  }

  // Select frequency channel:
  if (sx1278.setChannel(LORA_CHANNEL) == 0) {
    Serial.println(F("Setting Channel: SUCCESS "));
  } else {
    Serial.println(F("Setting Channel: ERROR "));
  }

  // Set CRC:
  if (sx1278.setCRC_ON() == 0) {
    Serial.println(F("Setting CRC ON: SUCCESS "));
  } else {
    Serial.println(F("Setting CRC ON: ERROR "));
  }

  // Select output power (Max, High, Intermediate or Low)
  if (sx1278.setPower('M') == 0) {
    Serial.println(F("Setting Power: SUCCESS "));
  } else {
    Serial.println(F("Setting Power: ERROR "));
  }

  // Set the node address and print the result
  if (sx1278.setNodeAddress(LORA_ADDRESS) == 0) {
    Serial.println(F("Setting node address: SUCCESS "));
  } else {
    Serial.println(F("Setting node address: ERROR "));
  }

  // Print a success
  Serial.println(F("SX1278 configured finished"));
  Serial.println();
}

void loraSetupFT () {
  // Power ON the module:
  if (sx1278.ON() == 0) {
  } else {
  }

  // Set transmission mode and print the result:
  if (sx1278.setMode(LORA_MODE) == 0) {
  } else {
  }

  // Set header:
  if (sx1278.setHeaderON() == 0) {
  } else {
  }

  // Select frequency channel:
  if (sx1278.setChannel(LORA_CHANNEL) == 0) {
  } else {
  }

  // Set CRC:
  if (sx1278.setCRC_ON() == 0) {
  } else {
  }

  // Select output power (Max, High, Intermediate or Low)
  if (sx1278.setPower('M') == 0) {
  } else {
  }

  // Set the node address and print the result
  if (sx1278.setNodeAddress(LORA_ADDRESS) == 0) {
  } else {
  }

  // Print a success
  Serial.println(F("SX1278 RE-CONFIGURED FINISHED"));
  Serial.println();
}

