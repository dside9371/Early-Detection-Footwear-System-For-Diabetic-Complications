 /*
  
  Written By Khalid Muslih
  This example code is in the public domain.

  This code is written to work on Arduino Primo Core. It will receive temperature data from the other shoe (other primo core)
  and compare it with the local temperature data. The last will be sending these raw values into the data base through UART.
  Note:  The temperature data are raw voltage values, and we convert these values to actual temperature readings in the data base. 

*/
#include <SoftwareSerial.h>
#include <math.h>
#include <BLEPeripheral.h>
// download ArduinoLowPower library from library manager to enter in low power mode
#include "ArduinoLowPower.h"


// create peripheral instance
BLEPeripheral                    blePeripheral                            = BLEPeripheral();

// create remote service with UUID compliant to CTS service
BLERemoteService                 remoteCtsService            = BLERemoteService("1805");

// create remote characteristics with UUID and properties compliant to CTS service 
BLERemoteCharacteristic          remoteCtsCharacteristic           = BLERemoteCharacteristic("2a2b", BLERead | BLENotify);
//int BUTTON = 1;
SoftwareSerial mySerial(8, 9); // RX, TX

//analog pin config
int analogPin0 = 0;
int analogPin1 = 1;
int analogPin2 = 2;
//int analogPin3 = 3;
int analogPin4 = 4;
int analogPin5 = 5;
int analogPin6 = 6;
int analogPin7 = 7;
int temp[6];
int BLETemp[6];
int tempDifferences[6] = {0};
/////////////////////gsm////////////////////
int incomingByte;
char incomingChar;
char SIM800Response[200];

//void uploadDataToGSM(int tempDifferences);
///////////////////////////////////////////
void setup() {
  pinMode(BLE_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(analogPin0, INPUT);
  pinMode(analogPin1, INPUT);
  pinMode(analogPin2, OUTPUT);
  //pinMode(analogPin3, INPUT);
  pinMode(analogPin4, INPUT);
  pinMode(analogPin5, INPUT);
  pinMode(analogPin6, INPUT);
  pinMode(analogPin7, INPUT);
  nrf_saadc_resolution_set(NRF_SAADC_RESOLUTION_10BIT);
  
  Serial.begin(9600);
  while (!Serial) {
                                                        // wait for serial port to connect. Needed for native USB port only
  }
                                                   // set the data rate for the SoftwareSerial port
  mySerial.begin(9600);
                                                          //  mySerial.println("Hello, world?");
  
  blePeripheral.setLocalName("CTS-Client");

  // set device name and appearance
  blePeripheral.setDeviceName("CTS client");
  blePeripheral.setAppearance(BLE_APPEARANCE_GENERIC_CLOCK);

  blePeripheral.addRemoteAttribute(remoteCtsService);
  blePeripheral.addRemoteAttribute(remoteCtsCharacteristic);

   // assign event handlers for connected, disconnected to peripheral
  blePeripheral.setEventHandler(BLEConnected, blePeripheralConnectHandler);
  blePeripheral.setEventHandler(BLEDisconnected, blePeripheralDisconnectHandler);
  blePeripheral.setEventHandler(BLERemoteServicesDiscovered, blePeripheralRemoteServicesDiscoveredHandler);

  // assign event handlers for characteristic
  remoteCtsCharacteristic.setEventHandler(BLEValueUpdated, bleRemoteCtsCharacteristicValueUpdatedHandle);
 
  // begin initialization
  blePeripheral.begin();

  //attachInterrupt(BUTTON, readTemp, LOW);
//  mySerial.println(F("BLE Peripheral - cts"));
}

void loop() {
  digitalWrite(analogPin2, HIGH);
  delay(5000);
  readTemp();
//  if (mySerial.available()) {
//    Serial.write(mySerial.read());
//  }
//  if (Serial.available()) {
//    mySerial.write(Serial.read());
//  }
  
  tempDifferences[0] = (currentTemp[0] - temp[0]);
  delay(100);
  tempDifferences[1] = (currentTemp[1] - temp[1]);
  delay(100);
  tempDifferences[2] = (currentTemp[2] - temp[2]);
  delay(100);
  tempDifferences[3] = (currentTemp[3] - temp[3]);
  delay(100);
  tempDifferences[4] = (currentTemp[4] - temp[4]);
  delay(100);
  tempDifferences[5] = (currentTemp[5] - temp[5]);
  mySerial.println("9999");
  mySerial.flush();
  delay(2000);
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(BLE_LED, LOW);
  // send to the GSM Chip //
  mySerial.println(tempDifferences[0]);
  mySerial.flush();
  delay(2000);
  mySerial.println(tempDifferences[1]);
  mySerial.flush();
  delay(2000);
  mySerial.println(tempDifferences[2]);
  mySerial.flush();
  delay(2000);
  mySerial.println(tempDifferences[3]);
  mySerial.flush();
  delay(2000);
  mySerial.println(tempDifferences[4]);
  mySerial.flush();
  delay(2000);
  mySerial.println(tempDifferences[5]);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(BLE_LED, HIGH);
  mySerial.flush();
  delay(2000);
  
  
  
}


void blePeripheralConnectHandler(BLECentral& central) {
  // central connected event handler
  //mySerial.print(F("Connected event, central: "));
 // mySerial.println(central.address());
  digitalWrite(BLE_LED, HIGH);
  digitalWrite(RED_LED, LOW);
}

void blePeripheralDisconnectHandler(BLECentral& central) {
  // central disconnected event handler
  //mySerial.print(F("Disconnected event, central: "));
 // mySerial.println(central.address());
  digitalWrite(BLE_LED, LOW);
  digitalWrite(RED_LED, HIGH);
}

void blePeripheralRemoteServicesDiscoveredHandler(BLECentral& central) {
  // central remote services discovered event handler
 // mySerial.print(F("Remote services discovered event, central: "));
 // mySerial.println(central.address());

  if (remoteCtsCharacteristic.canRead()) {
    remoteCtsCharacteristic.read();
  }
}


void bleRemoteCtsCharacteristicValueUpdatedHandle(BLECentral& central, BLERemoteCharacteristic& characteristic) {
  // copy the time value in a local variable
  unsigned char currentTemp[BLE_REMOTE_ATTRIBUTE_MAX_VALUE_LENGTH + 1];
 
  memset(currentTemp, 0, sizeof(currentTemp));
  memcpy(currentTemp, remoteCtsCharacteristic.value(), remoteCtsCharacteristic.valueLength());
  // temp recieved over ble
  
  BLETemp[0] = 10*((int)currentTemp[0]);
  BLETemp[1] = 10*((int)currentTemp[1]);
  BLETemp[2] = 10*((int)currentTemp[2]);
  BLETemp[3] = 10*((int)currentTemp[3]);
  BLETemp[4] = 10*((int)currentTemp[4]);
  BLETemp[5] = 10*((int)currentTemp[5]);
  //local temp analog read in
  temp[0] =  (int)analogRead(analogPin0);
  temp[1] =  (int)analogRead(analogPin1);  
  temp[2] = (int)analogRead(analogPin4);
  temp[3] = (int)analogRead(analogPin5);
  temp[4] = (int)analogRead(analogPin6);
  temp[5] = (int)analogRead(analogPin7);
  // difference calc
  digitalWrite(analogPin2, LOW);
  
}

void readTemp(){  
  remoteCtsCharacteristic.read();
}
