//Including Library:
#include <Arduino.h>
#include <SX1278.h>

// Including Library for 1.8 TFT DispLay
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

//Library for Ticker
#include <Ticker.h>

//// TFT Display Pin For Arduino
#define TFT_CS                            10
#define TFT_RST                           16
#define TFT_DC                            0
#define TFT_SCLK                          14
#define TFT_MOSI                          13

// Color Code For Tft Display
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

// Variable For TFT Display
int rect1x = 4;
int rect1y = 25;
int rect2y = 47;
int rect3y = 69;
int rect4y = 91;
int recwidth = 122;
int recheight = 20;

// Creating Object of TFT Display
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

//Lora SX1278:
#define LORA_MODE             10
#define LORA_CHANNEL          CH_6_BW_125
#define LORA_ADDRESS          5
volatile uint8_t NodeAddress; //Child Address 

char my_packet [50];
char testData[50];
String receivedMsg;

int T_packet_state;
int R_packet_state;

//Pin def:
#define digitalButton_1 2
#define digitalButton_2 4
#define digitalButton_3 5
#define analogButton A0
//Analog button value storing variable:
int AB_value;
//Locations:
String Location;
String Location_1 = "GEC";
String Location_2 = "BAIZID";
String Location_3 = "MURADPUR";
String Location_4 = "PROBARTAK";
boolean blockStateColor;
// boolean blockStateColor2;
Ticker Location1, Location2, Location3, Location4;

boolean locationBlock_1 = true;
boolean locationBlock_2 = true;
boolean locationBlock_3 = true;
boolean locationBlock_4 = true;
// Button State
boolean button1State = true;
boolean button2State = true;
boolean button3State = true;
boolean button4State = true;
//Sync:
boolean resetCondition = true;
boolean resetStop;
//Timer:
long interval = 3000;
volatile unsigned long DB_priv_time_1;
volatile unsigned long DB_priv_time_2;
volatile unsigned long DB_priv_time_3;
volatile unsigned long AB_priv_time;
//Interrupt Flag:
volatile boolean DB_ISR_F_1 = false;
volatile boolean DB_ISR_F_2 = false;
volatile boolean DB_ISR_F_3 = false;
//Lane Wand/Child ChipID:
// long Child_1chipID = 13272349;
// long Child_2chipID = 14660951; 
// long Child_3chipID = ;
// long Child_4chipID = ;

void setup() {
  Serial.begin(9600);
  //Display Setup:
  displaySetup();
  // Lora Initialization
  loraSetup();
  //To make button 2 work
  pinMode(0, OUTPUT);
  digitalWrite(0, LOW);
  //PinMode:
  pinMode(digitalButton_1, INPUT);
  pinMode(digitalButton_2, INPUT);
  pinMode(digitalButton_3, INPUT);
  pinMode(analogButton, INPUT);

  //Interrupt:
  attachInterrupt(digitalPinToInterrupt(digitalButton_1), ISR_DB_1, FALLING);
  attachInterrupt(digitalPinToInterrupt(digitalButton_2), ISR_DB_2, FALLING);
  attachInterrupt(digitalPinToInterrupt(digitalButton_3), ISR_DB_3, FALLING);
}

void loop() {
  //After the device is booted it automatically re-boots other device:
  sync();
  //This function checks for data to receive
  recieveData();
  //This functions is for InterruptAction:
  InterruptAction();
  //Analog functions:
  AnalogAction();
  //To make Swtich in pin 2 function:
  digitalWrite(digitalButton_1, HIGH);
  digitalWrite(0, LOW);

  // ----------------------------------
  // ----------------------------------
}

void sync() {
  if (resetCondition == true) {
    String("S").toCharArray(testData, 50);
    sendData(0 ,testData);
    delay(1000);

    sendDataFailSafe();
    if ( T_packet_state == 0 ) {
      resetStop = true;
    }
  }
  if (resetStop == true) {
    resetCondition = false;
    resetStop = false;
  }
}

//Interrupt Service Routine for Button 1,2,3:
void ISR_DB_1() { //ISR handler for button 1
  if((long(millis()) - DB_priv_time_1) >= interval) {
    DB_ISR_F_1 = true;
    DB_priv_time_1 = millis();
  }  
}
void ISR_DB_2() { //ISR handler for button 2
  if((long(millis()) - DB_priv_time_2) >= interval) {
    DB_ISR_F_2= true;
    DB_priv_time_2 = millis();
  }
}
void ISR_DB_3() { //ISR handler for button 3
  if((long(millis()) - DB_priv_time_3) >= interval) {
    DB_ISR_F_3 = true;
    DB_priv_time_3 = millis();
  }
}
//States what happens when InterruptAction Function is called:
void InterruptAction() {
  //DB 1:
  if(DB_ISR_F_1) {
    Location = Location_1;

    if (button1State) {
      Serial.println("Button 1 was pressed once!");
      // String(String(Child_1chipID) + " GL1").toCharArray(testData, 50);
      String("GL1").toCharArray(testData, 50);
      sendData(3, testData);
      delay(1000);
      //FailSafe:
      sendDataFailSafe();

      if (T_packet_state == 0) {
        blockStateColor = true;
        Location1.attach(1, Blink_Location_Rect_1);
      }
      if (T_packet_state == 0) {
        button1State = false;
        DB_ISR_F_1 = false;
      }
    } else if (!button1State) {
      Serial.println("Button 1 was pressed twice!");
      // String(String(Child_1chipID) + " RL1").toCharArray(testData, 50);
      String("RL1").toCharArray(testData, 50);
      sendData(3, testData);
      delay(1000);
      //FailSafe:
      sendDataFailSafe();

      if (T_packet_state == 0) {
        blockStateColor = false;
        Location1.attach(1, Blink_Location_Rect_1);
      }
      if (T_packet_state == 0) {
        button1State = true;
        DB_ISR_F_1 = false;
      }
    }
  } 
  //DB 2:
  else if (DB_ISR_F_2) {
    Location = Location_2;

    if (button2State) {
      Serial.println("Button 2 was pressed once!");
      // NodeAddress = 4;
      // String(String(Child_2chipID) + " GL2").toCharArray(testData, 50);
      String("GL2").toCharArray(testData, 50);
      sendData(4, testData);
      delay(1000);
      //FailSafe:
      sendDataFailSafe();

      if (T_packet_state == 0) {
        blockStateColor = true;
        Location2.attach(0.9, Blink_Location_Rect_2);
      }
      if (T_packet_state == 0) {
        button2State = false;
        DB_ISR_F_2 = false;
      }
    } else if (!button2State) {
      Serial.println("Button 2 was pressed twice!");
      // NodeAddress = 4;
      // String(String(Child_2chipID) + " RL2").toCharArray(testData, 50);
      String("RL2").toCharArray(testData, 50);
      sendData(4, testData);
      delay(1000);
      //FailSafe:
      sendDataFailSafe();

      if (T_packet_state == 0) {
        blockStateColor = false;
        Location2.attach(0.9, Blink_Location_Rect_2);
      }
      if (T_packet_state == 0) {
        button2State = true;
        DB_ISR_F_2 = false;
      }
    }
  } 
  //DB 3:
  else if (DB_ISR_F_3) {
    Location = Location_3;

    if (button3State) {
      Serial.println("Button 3 was pressed once!");
      String("GL3").toCharArray(testData, 50);
      sendData(6, testData);
      delay(1000);
      //FailSafe:
      sendDataFailSafe();

      if (T_packet_state == 0) {
        blockStateColor = true;
        Location3.attach(0.7, Blink_Location_Rect_3);
      }
      if (T_packet_state == 0) {
        button3State = false;
        DB_ISR_F_3 = false;
      }
    } else if (!button3State) {
      Serial.println("Button 3 was pressed twice!");
      String("RL3").toCharArray(testData, 50);
      sendData(6, testData);
      delay(1000);
      //FailSafe:
      sendDataFailSafe();

      if (T_packet_state == 0) {
        blockStateColor = false;
        Location3.attach(0.7, Blink_Location_Rect_3);
      }
      if (T_packet_state == 0) {
        button3State = true;
        DB_ISR_F_3 = false;
      }
    }
  }
}

//Analog button action:
void AnalogAction() {
  AB_value = analogRead(analogButton);
  if(AB_value < 100) {
    if((long(millis()) - AB_priv_time) >= interval) {
      Location = Location_4;

      if(button4State) {
        Serial.println("Button 4 was pressed once!");
        String("GL4").toCharArray(testData, 50);
        sendData(7, testData);
        delay(1000);
        //FailSafe:
        sendDataFailSafe();

        if (T_packet_state == 0) {
          blockStateColor = true;
          Location4.attach(0.8, Blink_Location_Rect_4);
        }
        if (T_packet_state == 0) {
          button4State = false;
        }
      } else if (!button4State) {
        Serial.println("Button 4 was pressed twice!");
        String("RL4").toCharArray(testData, 50);
        sendData(7, testData);
        delay(1000);
        //FailSafe:
        sendDataFailSafe();

        if (T_packet_state == 0) {
          blockStateColor = false;
          Location4.attach(0.8, Blink_Location_Rect_4);
        }
        if (T_packet_state == 0) {
          button4State = true;
        }
      }
      AB_priv_time = millis();
    }
  }
}
//Global send data function
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

int sendData(volatile uint8_t NodeAddress, char message[]) {
  T_packet_state = sx1278.sendPacketTimeoutACK(NodeAddress, message);
  if (T_packet_state == 0)
  {
    Serial.println(F("State = 0 --> Command Executed w no errors!"));
    Serial.println(F("Packet sent....."));

    return T_packet_state;
  }
  else {
    Serial.print(F("Error Code: "));
    Serial.println(T_packet_state);
    Serial.println(F("Packet not sent....."));

    return T_packet_state;
  }
}
//States what happens when SendDaata() function fails
int sendDataFailSafe() {
  Serial.println(NodeAddress);
  if ( T_packet_state != 0) {
    sendData(NodeAddress, testData);
    delay(1000);
    if ( T_packet_state != 0) {
      loraSetupFT();
      delay(10);
      sendData(NodeAddress, testData);
      delay(1000);
    }
  }
}
//Global receive data funtion
void recieveData() {
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

    receivedMsg = String(my_packet);

    Setting_Block_State_Color();
    // return R_packet_state;
  }
}
 //This is for blinking the FIRST Location
void Blink_Location_Rect_1() {
  //Green Block Blink: 
  if (blockStateColor) {

    if (locationBlock_1) {

      tft.fillRect(rect1x, rect1y, recwidth, recheight, GREEN);
      tft.setCursor(40, 30);
      tft.setTextColor(BLACK);
      tft.setTextSize(1);
      tft.print("GEC");

      locationBlock_1 = false;

    } else if (!locationBlock_1) {
      tft.fillRect(rect1x, rect1y, recwidth, recheight, BLACK);
      tft.setCursor(40, 30);
      tft.setTextColor(WHITE);
      tft.setTextSize(1);
      tft.print("GEC");

      locationBlock_1 = true;

    }
  } 
  //Red Block Blink:
  else if (!blockStateColor) {

    if (locationBlock_1) {

      tft.fillRect(rect1x, rect1y, recwidth, recheight, RED);
      tft.setCursor(40, 30);
      tft.setTextColor(BLACK);
      tft.setTextSize(1);
      tft.print("GEC");

      locationBlock_1 = false;

    } else if (!locationBlock_1) {

      tft.fillRect(rect1x, rect1y, recwidth, recheight, BLACK);
      tft.setCursor(40, 30);
      tft.setTextColor(WHITE);
      tft.setTextSize(1);
      tft.print("GEC");

      locationBlock_1 = true;
    }
  }
}
//This is for blinking the SECOND Location
void Blink_Location_Rect_2() {
  //Green Block Blink
    if (blockStateColor) {

      if (locationBlock_2) {

        tft.fillRect(rect1x, rect2y, recwidth, recheight, GREEN);
        tft.setCursor(40, 55);
        tft.setTextColor(BLACK);
        tft.setTextSize(1);
        tft.print("BAIZID");

        locationBlock_2 = false;

      } else if (!locationBlock_2) {
        tft.fillRect(rect1x, rect2y, recwidth, recheight, BLACK);
        tft.setCursor(40, 55);
        tft.setTextColor(WHITE);
        tft.setTextSize(1);
        tft.print("BAIZID");

        locationBlock_2 = true;

      }
    } 
    //Red Block Blink
    else if (!blockStateColor) {

      if (locationBlock_2) {

        tft.fillRect(rect1x, rect2y, recwidth, recheight, RED);
        tft.setCursor(40, 55);
        tft.setTextColor(BLACK);
        tft.setTextSize(1);
        tft.print("BAIZID");

        locationBlock_2 = false;

      } else if (!locationBlock_2) {

        tft.fillRect(rect1x, rect2y, recwidth, recheight, BLACK);
        tft.setCursor(40, 55);
        tft.setTextColor(WHITE);
        tft.setTextSize(1);
        tft.print("BAIZID");

        locationBlock_2 = true;
      }
    }
}
//This is for blinking the THIRD Location
void Blink_Location_Rect_3() {
  //Green Block Blink
    if (blockStateColor) {

      if (locationBlock_3) {

        tft.fillRect(rect1x, rect3y, recwidth, recheight, GREEN);
        tft.setCursor(40, 75);
        tft.setTextColor(BLACK);
        tft.setTextSize(1);
        tft.print("MURADPUR");

        locationBlock_3 = false;

      } else if (!locationBlock_3) {
        tft.fillRect(rect1x, rect3y, recwidth, recheight, BLACK);
        tft.setCursor(40, 75);
        tft.setTextColor(WHITE);
        tft.setTextSize(1);
        tft.print("MURADPUR");

        locationBlock_3 = true;

      }
    } 
    //Red Block Blink
    else if (!blockStateColor) {

      if (locationBlock_3) {

        tft.fillRect(rect1x, rect3y, recwidth, recheight, RED);
        tft.setCursor(40, 75);
        tft.setTextColor(BLACK);
        tft.setTextSize(1);
        tft.print("MURADPUR");

        locationBlock_3 = false;

      } else if (!locationBlock_3) {

        tft.fillRect(rect1x, rect3y, recwidth, recheight, BLACK);
        tft.setCursor(40, 75);
        tft.setTextColor(WHITE);
        tft.setTextSize(1);
        tft.print("MURADPUR");

        locationBlock_3 = true;
      }
    }
}
//This is for blinking the FOURTH Location
void Blink_Location_Rect_4() {
  //Green Block Blink
    if (blockStateColor) {

      if (locationBlock_4) {

        tft.fillRect(rect1x, rect4y, recwidth, recheight, GREEN);
        tft.setCursor(40, 97);
        tft.setTextColor(BLACK);
        tft.setTextSize(1);
        tft.print("PROBARTAK");

        locationBlock_4 = false;

      } else if (!locationBlock_4) {
        tft.fillRect(rect1x, rect4y, recwidth, recheight, BLACK);
        tft.setCursor(40, 97);
        tft.setTextColor(WHITE);
        tft.setTextSize(1);
        tft.print("PROBARTAK");

        locationBlock_4 = true;

      }
    } 
    //Red Block Blink
    else if (!blockStateColor) {

      if (locationBlock_4) {

        tft.fillRect(rect1x, rect4y, recwidth, recheight, RED);
        tft.setCursor(40, 97);
        tft.setTextColor(BLACK);
        tft.setTextSize(1);
        tft.print("PROBARTAK");

        locationBlock_4 = false;

      } else if (!locationBlock_4) {

        tft.fillRect(rect1x, rect4y, recwidth, recheight, BLACK);
        tft.setCursor(40, 97);
        tft.setTextColor(WHITE);
        tft.setTextSize(1);
        tft.print("PROBARTAK");

        locationBlock_4 = true;
      }
    }
}

void Setting_Block_State_Color() {
  //Sets the block state for FIRST Location
  if(Location == Location_1 ) {
    if (receivedMsg.equals("KL1")) {
      if (blockStateColor) {
        Location1.detach();
        tft.fillRect(rect1x, rect1y, recwidth, recheight, GREEN);
        tft.setCursor(40, 30);
        tft.setTextColor(BLACK);
        tft.setTextSize(1);
        tft.print("GEC");
      } else if (!blockStateColor) {
        Location1.detach();
        tft.fillRect(rect1x, rect1y, recwidth, recheight, RED);
        tft.setCursor(40, 30);
        tft.setTextColor(BLACK);
        tft.setTextSize(1);
        tft.print("GEC");
      }
    }
    digitalWrite(0, LOW);
  } 
  //Sets the block state for SECOND location
  else if (Location == Location_2) {
    if (receivedMsg.equals("KL2")) {
      if (blockStateColor) {
        Location2.detach();
        tft.fillRect(rect1x, rect2y, recwidth, recheight, GREEN);
        tft.setCursor(40, 55);
        tft.setTextColor(BLACK);
        tft.setTextSize(1);
        tft.print("BAIZID");
      } else if (!blockStateColor) {
        Location2.detach();
        tft.fillRect(rect1x, rect2y, recwidth, recheight, RED);
        tft.setCursor(40, 55);
        tft.setTextColor(BLACK);
        tft.setTextSize(1);
        tft.print("BAIZID");
      }
    }
  }
  //Sets the block state for THIRD location
  else if (Location == Location_3) {
    if (receivedMsg.equals("KL3")) {
      if (blockStateColor) {
        Location3.detach();
        tft.fillRect(rect1x, rect3y, recwidth, recheight, GREEN);
        tft.setCursor(40, 75);
        tft.setTextColor(BLACK);
        tft.setTextSize(1);
        tft.print("MURADPUR");
      } else if (!blockStateColor) {
        Location3.detach();
        tft.fillRect(rect1x, rect3y, recwidth, recheight, RED);
        tft.setCursor(40, 75);
        tft.setTextColor(BLACK);
        tft.setTextSize(1);
        tft.print("MURADPUR");
      }
    }
  }
  //Sets the block state for FOURTH location
  else if (Location == Location_4) {
    if (receivedMsg.equals("KL4")) {
      if (blockStateColor) {
        Location4.detach();
        tft.fillRect(rect1x, rect4y, recwidth, recheight, GREEN);
        tft.setCursor(40, 97);
        tft.setTextColor(BLACK);
        tft.setTextSize(1);
        tft.print("PROBARTAK");
      } else if (!blockStateColor) {
        Location4.detach();
        tft.fillRect(rect1x, rect4y, recwidth, recheight, RED);
        tft.setCursor(40, 97);
        tft.setTextColor(BLACK);
        tft.setTextSize(1);
        tft.print("PROBARTAK");
      }
    }
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

void displaySetup() {

  // Diaplay Initialization and Creating BLock
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(2);

  tft.fillScreen(ST7735_CYAN);
  tft.setTextSize(0);
  tft.setTextColor(BLACK);
  tft.setCursor(23, 10);
  tft.println("TRAFFIC CONTROL");
  delay(1000);


  // Black Rect
  // For Location 1:
  tft.drawRect(rect1x - 1 , rect1y - 1, recwidth + 2, recheight, BLACK);
  tft.drawRect(rect1x, rect1y, recwidth, recheight, BLACK);
  // For Location 2:
  tft.drawRect(rect1x - 1 , rect2y - 1, recwidth + 2, recheight, BLACK);
  tft.drawRect(rect1x, rect2y, recwidth, recheight, BLACK);
  // For Location 3:
  tft.drawRect(rect1x - 1 , rect3y - 1, recwidth + 2, recheight, BLACK);
  tft.drawRect(rect1x, rect3y, recwidth, recheight, BLACK);
  // For Location 4:
  tft.drawRect(rect1x - 1 , rect4y - 1 , recwidth + 2, recheight, BLACK);
  tft.drawRect(rect1x, rect4y, recwidth, recheight, BLACK);
  // Status Block:
  tft.drawRect(rect1x - 1 , 127 - 1 , recwidth + 2, 30, BLACK);
  tft.drawRect(rect1x, 127, recwidth, 30, BLACK);
  tft.fillRect(rect1x, 127, recwidth, 30, YELLOW); //To fill the block

  // Full Fill RECT
  // Location 1
  tft.fillRect(rect1x, rect1y, recwidth, recheight, YELLOW);
  // Location 2
  tft.fillRect(rect1x, rect2y, recwidth, recheight, YELLOW);
  // Location 3
  tft.fillRect(rect1x, rect3y, recwidth, recheight, YELLOW);
  // Location 4
  tft.fillRect(rect1x, rect4y, recwidth, recheight, YELLOW);

  // Location Names in display (Max 4 location)
  // Location name can be  changed according to location preference.
  // Location -1: GEC
  tft.setCursor(40, 30);
  tft.setTextColor(BLACK);
  tft.setTextSize(1);
  tft.print("GEC");

  // Location -2 :Baizid
  tft.setCursor(40, 55);
  tft.setTextColor(BLACK);
  tft.setTextSize(1);
  tft.print("BAIZID");

  // Location -3 :Muradpur
  tft.setCursor(40, 75);
  tft.setTextColor(BLACK);
  tft.setTextSize(1);
  tft.print("MURADPUR");

  // Location -4 :PROBARTAK
  tft.setCursor(40, 97);
  tft.setTextColor(BLACK);
  tft.setTextSize(1);
  tft.print("PROBARTAK");
}