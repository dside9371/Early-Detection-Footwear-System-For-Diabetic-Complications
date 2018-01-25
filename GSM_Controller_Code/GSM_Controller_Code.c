/*
  
  Written By Khalid Muslih and Vadim Lazarev

  This code is written to work on esp8266. It will receive temperature data from the client primo core)
  and send them through AT commands to the GSM chip. This code allows reading feedback from the GSM chip and check for failures.
  In case of a failure, the chip will resend the AT commands sequence to the GSM chip. 

*/





int incomingByte;
char incomingChar;
char SIM800Response[200];               // Hold AT+ response from the GSM
void uploadDataToGSM(int sensorData);   // Declare funtion
int dataDigits[6] = {0}; // Holds the digits that make up only one particular sensor
//int sensorData[6] = {0}; // Holds the values for all sensors received from the Primo Core
int sensorData[6] = {1000, 2000, 3000, 4000, 5000, 6000};
//String inString = ""; 
int digitCounter = 0;
int batteryLevel = 0;
int tempread;
int rec1000;          // holds the thousands digit of an individual sensor data
int rec100;           // holds the hundredths digit of an individual sensor data
int rec10;            // holds the tens digit of an individual sensor data
int rec;              // holds the singles digit of an individual sensor data
int singleSensorData = 0000; // temp variable to hold the value of one sensor sent from the Primo Core
int atCmdReturn = 0;
int charCount9999 = 0;
int charCountData = 0;
int sensorDataReceivedCount = 0;
int digitsin = 0;

// Declare functions used for sending AT commands
int cmdCipshut(void);
int cmdCstt(void);
int cmdCiicr(void);
int cmdCifsr(void);
int cmdCipstart(void);
int cmdCipsend(int sensorData[], int batteryLevel);
void SIM800ResponseClear(char SIM800Response[]);
int tempCounter = 0;

//pinMode(2, OUTPUT);     // Initialize the LED_BUILTIN pin as an output. INTERFERES

#include <SoftwareSerial.h>
//SoftwareSerial mySerial(3,2); // make RX Arduino line is pin 2, make TX Arduino line is pin 3.
                             // This means that you need to connect the TX line from the esp to the Arduino's pin 2
                             // and the RX line from the esp to the Arduino's pin 3
SoftwareSerial mySerial(0,2); // make RX Arduino line is pin 2, make TX Arduino line is pin 0.

void setup() {
  //ESP.wdtDisable();
  Serial.begin(9600);
  while(!Serial);
  //delay(1000);

  mySerial.begin(9600);
  //Serial1.begin(9600);
  //while(!Serial1);
  //delay(1000);
  
}

// Is there a conflict due to having SoftSerial and Serial1 enabled? 
// disable Serial1

void loop() {
    
    while ((singleSensorData != 9999)) //&& mySerial.available())// and new number is available /// check here
    {
                        
            while((charCount9999<6) && (mySerial.available() > 0)) // collect the _ _ _ _ digits  
            {
                dataDigits[charCount9999] = mySerial.read(); //-1
                delay(10);
                if ((dataDigits[charCount9999] != 13) && (dataDigits[charCount9999] != 10))
                {
                  digitCounter++;
                }
                charCount9999++;
            }

            if(dataDigits[0] == 13){
              
              dataDigits[0] = dataDigits[2];
              dataDigits[1] = dataDigits[3];
              dataDigits[2] = dataDigits[4];
              dataDigits[3] = dataDigits[5];
            }
            
            if (digitCounter == 4)
            {
                rec1000 = (dataDigits[0]-48)*1000;
                rec100 = (dataDigits[1]-48)*100;
                rec10 = (dataDigits[2]-48)*10;
                rec = dataDigits[3]-48;
                singleSensorData = rec1000 + rec100 + rec10 + rec;
                //Serial.println("Come here often2 ? ");
            }
            else if (digitCounter == 3)  
            {
                //rec1000 = (dataDigits[0]-48)*1000;
                rec100 = (dataDigits[0]-48)*100;
                rec10 = (dataDigits[1]-48)*10;
                rec = dataDigits[2]-48;
                singleSensorData = rec100 + rec10 + rec;
                //Serial.println("Come here often3 ? ");
            }
            else if (digitCounter == 2)  
            {
                //rec1000 = (dataDigits[0]-48)*1000;
                //rec100 = (dataDigits[0]-48)*100;
                rec10 = (dataDigits[0]-48)*10;
                rec = dataDigits[1]-48;
                singleSensorData = rec10 + rec;
                //Serial.println("Come here often4 ? ");
            } 
            else if ( digitCounter == 1 )
            {
              //Serial.println("Come here often5 ? ");
                rec = dataDigits[0]-48;
                singleSensorData = rec; 
            }
       
            rec1000 = 0;
            rec100 = 0;
            rec10 = 0;
            rec = 0;
            digitCounter = 0;
            charCount9999 = 0;
            delay(10);
            //Serial.println("Come here often6 ? ");
        // the absense of this delay causes a WDT reset (watch dog timer issue with the loop)
    }
    
    while((sensorDataReceivedCount) < 6) // collect data for 6 sensors //Serial.available() > 0 && j<6 // was j = 0 and j < 6
    {     
      //Serial.println("Come here often7 ? ");
        while((charCountData < 4) && (mySerial.available() > 0)) // collect the _ _ _ _ digits //(mySerial.available() > 0) && 
        {   
            tempread = mySerial.read(); 
            delay(100);
            if ((tempread != 13) && (tempread != 10))
            {
              dataDigits[charCountData] = tempread;
              digitCounter++;
              digitsin = 1;
            }
            
            if ((tempread == 13) || (tempread == 10))
            {
               break;
            }
            
            charCountData++;
       
        }
        delay(100);
//        
//        if(dataDigits[0] == 13)
//        {
//              
//            dataDigits[0] = dataDigits[2];
//            dataDigits[1] = dataDigits[3];
//            dataDigits[2] = dataDigits[4];
//            dataDigits[3] = dataDigits[5];
//        }

        
        if (digitCounter == 4)
        {
            rec1000 = (dataDigits[0]-48)*1000;
            rec100 = (dataDigits[1]-48)*100;
            rec10 = (dataDigits[2]-48)*10;
            rec = dataDigits[3]-48;
            singleSensorData = rec1000 + rec100 + rec10 + rec;
            
        }
        else if (digitCounter == 3)  
        {
            rec100 = (dataDigits[0]-48)*100;
            rec10 = (dataDigits[1]-48)*10;
            rec = dataDigits[2]-48;
            singleSensorData = rec100 + rec10 + rec;
        }
        else if (digitCounter == 2)  
        {
            rec10 = (dataDigits[0]-48)*10;
            rec = dataDigits[1]-48;
            singleSensorData = rec10 + rec;
        }
        else if ( digitCounter == 1 )
        {
             rec = dataDigits[0]-48;
             singleSensorData = rec; 
        }
        digitCounter = 0;
        charCountData = 0;
        
        if(digitsin == 1 )
        {
          sensorData[sensorDataReceivedCount] = singleSensorData;
          //Serial.println(sensorData[sensorDataReceivedCount]);
          sensorDataReceivedCount++;
        }
        rec1000 = 0;
        rec100 = 0;
        rec10 = 0;
        rec = 0;
        digitsin = 0;
        dataDigits[0] = 0;
        dataDigits[1] = 0;
        dataDigits[2] = 0;
        dataDigits[3] = 0;
       ///Serial.println("Come here often9 ? ");


       
    }
    // Finished collecting data from all sensors. 
    sensorDataReceivedCount = 0;

    delay(5000);
    tempCounter = 300;
    //Serial.println("Come here often10 ? ");


// add timeout

    restart:
//    
    delay(500);
    atCmdReturn = cmdCipshut();
    if (atCmdReturn != 1) 
    {
        goto restart;
        //return;
    }
//    
    SIM800ResponseClear(SIM800Response);
    delay(500);

    atCmdReturn = cmdCstt();
    if (atCmdReturn != 1) 
    {
        goto restart; //return;
    }
//    
    SIM800ResponseClear(SIM800Response);
    delay(500);

    atCmdReturn = cmdCiicr();
    if (atCmdReturn != 1) 
    {
        goto restart; //return;
    }
//    
    SIM800ResponseClear(SIM800Response);
    delay(500);

    atCmdReturn = cmdCifsr();
    if (atCmdReturn != 1) 
    {
        goto restart; //return;
    }
//    
    SIM800ResponseClear(SIM800Response);
    delay(500);
    
    atCmdReturn = cmdCipstart();
    if (atCmdReturn != 1) 
    {
        goto restart; //return;
    }
//    
    SIM800ResponseClear(SIM800Response);
    delay(500);
    
    atCmdReturn = cmdCipsend(sensorData, batteryLevel);
    if (atCmdReturn != 1) 
    {
        goto restart; //return;
    }


    SIM800ResponseClear(SIM800Response);
    delay(5000);

    while (Serial.available() > 0)
    {
        Serial.read();
    }
    
//
//    for (int i = 0; i < 6; i++)
//    {
//        dataDigits[i] = '0';
//        sensorData[i] = '0';
//    }

    // clear 
    //SIM800Response[i] = '\0';
    //possily finds "ERROR" omewhere in the array and returns error where there is NONE
}

void SIM800ResponseClear(char SIM800Response[])
{
      for (int i = 0; i < 200; i++)
      {
        SIM800Response[i] = '0';
      }
  
}

int cmdCipshut(void)
{
      // Get the GSM into a known state
      Serial.println("AT+CIPSHUT");  //
      Serial.flush();  
      while(!Serial.available())
      {
        delay(100);
      }
      for(int i = 0; Serial.available() > 0 && i<200 ; i++) {
          SIM800Response[i] = Serial.read(); // i++
          delay(100);
      }
    
      if(strstr(SIM800Response, "OK"))
      {
         //AT+CIPSHUT returned OK
         //mySerial.println("AT+CIPSHUT returned OK");
         return 1;
      }
      else if(strstr(SIM800Response, "ERROR"))
      {
         //AT+CIPSHUT returned ERROR
         //mySerial.println("AT+CIPSHUT returned ERROR");
         return 2;
      }
      else 
      {  // Need time out
         //No matching response
         return 3;
      }



      
}



int cmdCstt(void)
{
    // Send at+cstt="wholesale"
    Serial.println("at+cstt=\"wholesale\"");  //
    Serial.flush();  
    while(!Serial.available())
    {
      delay(100);
      //......waiting for at+cstt="wholesale"
    }
    for(int i = 0; Serial.available() > 0 && i<200 ; i++) {
        SIM800Response[i] = Serial.read();
        delay(100);
    }
    
    if(strstr(SIM800Response, "OK"))
    {
       //at+cstt="wholesale" returned OK
       return 1;
    }
    else if(strstr(SIM800Response, "ERROR"))
    {
       //at+cstt="wholesale" returned ERROR
       return 2;
    }
    else 
    { // Need time out
       //No matching response.
       return 3;
    }
}

int cmdCiicr(void)
{
    // Send at+ciicr
    Serial.println("at+ciicr");  //
    Serial.flush();  
    while(!Serial.available())
    {
      delay(100);
      //......waiting for at+ciicr
    }
    for(int i = 0; Serial.available() > 0 && i<200 ; i++) {
        SIM800Response[i] = Serial.read();
        delay(100);
    }
    
    if(strstr(SIM800Response, "OK"))
    {
       //"at+ciicr returned OK"
       return 1;
    }
    else if(strstr(SIM800Response, "ERROR"))
    {
       //"at+ciicr returned ERROR"
       return 2;
    }
    else 
    { // Need time out
       //"No matching response."
       return 3;
    }
}


int cmdCifsr(void)
{
    // Send at+cifsr
    Serial.println("at+cifsr");  //
    Serial.flush();  
    while(!Serial.available())
    {
      delay(100);
      //"......waiting for at+cifsr
    }
    for(int i = 0; Serial.available() > 0 && i<200 ; i++) {
        SIM800Response[i] = Serial.read();
        delay(100);
    }

    if(strstr(SIM800Response, "ERROR"))
    {
       //"at+cifsr returned ERROR"
       //Serial1.println("Display SIM800Response of cmdCifsr() in strst ERROR");
       //Serial1.println("returned 2 from at+cifsr");
       //Serial1.println(SIM800Response);
       //Serial1.println("K. Moving out of cmdCifsr()");
       //Serial1.flush();
       return 2;       
       //mySerial.println("returned 2 from at+cifsr");
    }
    else 
    { // Need time out
       //"Assume IP granted"

       //Serial1.println("returned 1 from at+cifsr");       //mySerial.println("returned 1 from at+cifsr");
       //Serial1.println(SIM800Response);
       //Serial1.flush();
       return 1;
    }
}
  



int cmdCipstart(void)
{
    // Send AT+CIPSTART="TCP","athena.ecs.csus.edu","80"
    Serial.println("AT+CIPSTART=\"TCP\",\"athena.ecs.csus.edu\",\"80\"");  //
    Serial.flush();  
    while(!Serial.available())
    {
      delay(100);
      //......waiting for AT+CIPSTART=\"TCP\",\"athena.ecs.csus.edu\",\"80\"
    }
    for(int i = 0; Serial.available() > 0 && i<200 ; i++) {
        SIM800Response[i] = Serial.read();
        delay(100);
    }
  
    if(strstr(SIM800Response, "CONNECT OK"))
    {
       //AT+CIPSTART=\"TCP\",\"athena.ecs.csus.edu\",\"80\" returned CONNECT OK
       return 1;
    }
    else if(strstr(SIM800Response, "ERROR"))
    {
       //AT+CIPSTART=\"TCP\",\"athena.ecs.csus.edu\",\"80\" returned ERROR
       return 2;
    }
    else 
    { // CONNECT OK confirmation has not been received yet. Keep waiting, keep checking, then time out.
        //"CONNECT OK not received. waiting."
        
        // Keep waiting for AT+CIPSTART response
        while(!Serial.available())
        {
            delay(100);
            //......waiting for AT+CIPSTART=\"TCP\",\"athena.ecs.csus.edu\",\"80\"
        }
        // Some data just came in. Save it into our array.
        for (int i = 0; Serial.available() > 0 && i<200 ; i++) {
            SIM800Response[i] = Serial.read(); // i++
            delay(100);
        }
  
        // Now lets find out what the response was.. OK or FAIL?
        if(strstr(SIM800Response, "CONNECT OK"))
        {
           //AT+CIPSTART=\"TCP\",\"athena.ecs.csus.edu\",\"80\" returned CONNECT OK
           return 1;
        }
        else if (strstr(SIM800Response, "CONNECT FAIL"))
        {
           //AT+CIPSTART=\"TCP\",\"athena.ecs.csus.edu\",\"80\" returned CONNECT FAIL
  
           // Here we need to restart the sending process. 
           // for that we need a state machine.
           return 3;
        }
        else
        return 3;
    }
}


int cmdCipsend(int sensorData[], int batteryLevel)
{  
    // Send at+cipsend
    Serial.println("at+cipsend");  //
    Serial.flush();  
    while(!Serial.available())
    {
      delay(100);
      //......waiting for at+cipsend
    }
    for(int i = 0; Serial.available() > 0 && i<200 ; i++) {
        SIM800Response[i] = Serial.read();
        delay(100);
    }
    
    if(strstr(SIM800Response, ">"))
    {
       //GET http://athena.ecs.csus.edu/~lazarevv/index.php?sensor1=145&sensor2=156&sensor3=143&sensor4=156&sensor5=148&sensor6=178&battery=batteryLevel HTTP/1.1 
       Serial.print("GET http://athena.ecs.csus.edu/~lazarevv/index.php?sensor1=");  //
       Serial.print(sensorData[0]);
       
       Serial.print("&sensor2=");      
       Serial.print(sensorData[1]);
       
       Serial.print("&sensor3=");      
       Serial.print(sensorData[2]);
       
       Serial.print("&sensor4=");      
       Serial.print(sensorData[3]);
       
       Serial.print("&sensor5=");      
       Serial.print(sensorData[4]);
       
       Serial.print("&sensor6=");      
       Serial.print(sensorData[5]);

       Serial.print("&battery=");      
       Serial.print(batteryLevel);
       
       Serial.print(" HTTP/1.1"); 
       Serial.print("\r\n");  
       Serial.flush();  
  
       Serial.print("Host: athena.ecs.csus.edu");
       Serial.print("\r\n");  
       Serial.print("\r\n"); 
       Serial.println((char)26);
       Serial.flush(); 
       return 1;
       
    }
    else if(strstr(SIM800Response, "ERROR"))
    {
       //at+ciicr returned ERROR"
       return 2;
    }
    else 
    { // Need time out
       //No matching response.
       return 3;
    }
}



  
