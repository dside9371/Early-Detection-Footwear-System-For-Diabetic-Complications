/*
  
  Written By Khalid Muslih
  This example code is in the public domain.

  This code is written to work on Arduino Primo Core. It will send temperature data to the other shoe (other primo core).
  Through BLE
  
  Note:  The temperature data are raw voltage values, and we convert these values to actual temperature readings in the data base. 

*/
#include <BLECentralRole.h>
#include <RTCInt.h>
#include <math.h>

#define SERIESRESISTOR 10000
// create central instance
BLECentralRole                   bleCentral                  = BLECentralRole();

// create service with UUID compliant to CTS service
BLEService                       ctsService                  = BLEService("1805");

// create a generic characteristics with UUID and properties compliant to CTS service 
//                                                                              ( UUID,  Properties,          length)
BLECharacteristic                ctsCharacteristic           = BLECharacteristic("2a2b", BLERead | BLENotify, 9);

RTCInt rtc;  //create an RTCInt type object

int analogPin0 = 0;
int analogPin1 = 1;
int analogPin2 = 2;
//int analogPin3 = 3;
int analogPin4 = 4;
int analogPin5 = 5;
int analogPin6 = 6;
int analogPin7 = 7;
float voltage[9];
unsigned char temp[9];
int placeholder;
int val = 0;
void setup() {
  Serial.begin(9600);
  nrf_saadc_resolution_set(NRF_SAADC_RESOLUTION_10BIT);

  //initialize BLE led
  pinMode(BLE_LED, OUTPUT);
  pinMode(analogPin0, INPUT);
  pinMode(analogPin1, INPUT);
  pinMode(analogPin2, OUTPUT);
  //pinMode(analogPin3, INPUT);
  pinMode(analogPin4, INPUT);
  pinMode(analogPin5, INPUT);
  pinMode(analogPin6, INPUT);
  pinMode(analogPin7, INPUT);
  //time settings
  
    
  // add service and characteristics
  bleCentral.addAttribute(ctsService);
  bleCentral.addAttribute(ctsCharacteristic);
  // assign event handlers for central events
  bleCentral.setEventHandler(BLEScanReceived, receiveAdvPck);  
  bleCentral.setEventHandler(BLEConnected, bleCentralConnectHandler);
  bleCentral.setEventHandler(BLEDisconnected, bleCentralDisconnectHandler);
 
  // set scan parameters
  // interval and window in 0.625 ms increments 
  bleCentral.setScanInterval(3200); // 2 sec
  bleCentral.setScanWindow(800);  // 0.5 sec

  // begin initialization and start scanning
  bleCentral.begin(); 
  
  Serial.println("Scanning...");
}

void loop() {
  int i;
  if(bleCentral.connected()){
    
    // prepare data to send   
    digitalWrite(analogPin2, HIGH);
    delay(5000);
    voltage[0] = (analogRead(analogPin0));
    delay(100);
    
    voltage[1] = (analogRead(analogPin1));
    delay(100);
    
    voltage[2] = (analogRead(analogPin4));
    delay(100);
    
    voltage[3] = (analogRead(analogPin5));
    delay(100);
    
    voltage[4] = (analogRead(analogPin6));
    delay(100);
    
    voltage[5] = (analogRead(analogPin7));
    delay(100);


      
    
    digitalWrite(analogPin2, LOW);
    // update characteristic value
    ctsCharacteristic.setValue((unsigned char *)temp, 9);

    // update the value every second
    delay(5000);
    
  }    
  else{ // if we are not connected we are scanning hence blink BLE led
    digitalWrite(BLE_LED, LOW);
    delay(200);
    digitalWrite(BLE_LED, HIGH);
    delay(200);
  }
}

void receiveAdvPck(BLEPeripheralPeer& peer){
  char advertisedName[31];
  byte len;
  // search for a device that advertises "CTS-Client" name
  peer.getFieldInAdvPck(BLE_GAP_AD_TYPE_SHORT_LOCAL_NAME, advertisedName, len);
  if(len == 0) // field not found
      peer.getFieldInAdvPck(BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME, advertisedName, len);
  if(len != 0){ // the field was found
  Serial.println(advertisedName);
    if(!strcmp(advertisedName, "CTS-Client"))
      // Name found. Connect to the peripheral
      bleCentral.connect(peer);
   } 
}

void bleCentralConnectHandler(BLEPeripheralPeer& peer) {
  // peer connected event handler
  Serial.print("Connected event, peripheral: ");
  Serial.println(peer.address());
  // turn BLE_LED on
  digitalWrite(BLE_LED, HIGH);
}

void bleCentralDisconnectHandler(BLEPeripheralPeer& peer) {
  // peer disconnected event handler
  Serial.print("Disconnected event, peripheral: ");
  Serial.println(peer.address());
  // turn BLE_LED off
  digitalWrite(BLE_LED, LOW);
}
//void AnalogPinRead(){
//  
//  }
