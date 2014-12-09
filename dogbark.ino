/*
 * There are places in the code that held passwords and vital information that is unique to each user.
 * Please read through the code and place your own passwords and tokens. All places to replace or change are denoted by XXXX's or "Place your account/key info here"
 * Depending on use, you might have to change a few other things as well. Thanks! Enjoy!
 *
 * DemoApp.ino
 *
 *      Created on: Nov 6, 2014
 *      Author: a0221660@ti.com
 *
 *      Copyright (c) 2014, Evan Wakefield
 *		All rights reserved.
 *
 *		Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 *
 *		1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 *
 *		2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the
 *                 documentation and/or other materials provided with the distribution.
 *
 *		THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
 *		EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 *		OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 *		SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *		SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 *		OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 *		HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *		(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *		EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <SPI.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <Temboo.h>
#include "TembooAccount.h" // Contains Temboo account information

#define SOUND_SENSOR 23

WiFiClient client;

int numRuns = 1;   // Execution count, so this doesn't run forever
int count = 0;      //amount my dog has barked
int ampReading = 0;  //sound reading
int threshold = 1200; 
int triggerSend = 5;       //triggers message every ten barks
int totalCount = 0;
const int var = 19;
String time;

void setup() {
  pins_init();
  Serial.begin(9600);
  initInternet();
}

void loop() {
  ampReading = analogRead(SOUND_SENSOR);
  delay(10);
  Serial.println(ampReading);
  if (ampReading > threshold) {
    totalCount++;
    count++;
    if(count == triggerSend){
      sendText();
    }

    getDate();
    appendRow();

    delay(500);      //makes sure it only records one instance every so often
  }

  Serial.println("\nWaiting...\n");
}

void pins_init()
{
  pinMode(SOUND_SENSOR, INPUT);
}

void initInternet()
{
  int wifiStatus = WL_IDLE_STATUS;

  // Determine if the WiFi Shield is present.
  Serial.print("\n\nShield:");
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("FAIL");

    // If there's no WiFi shield, stop here.
    while(true);
  }

  Serial.println("OK");

  // Try to connect to the local WiFi network.
  while(wifiStatus != WL_CONNECTED) {
    Serial.print("WiFi:");
    wifiStatus = WiFi.begin(WIFI_SSID, WPA_PASSWORD);

    if (wifiStatus == WL_CONNECTED) {
      Serial.println("OK");
    } else {
      Serial.println("FAIL");
    }
    delay(5000);
  }

  Serial.println("Setup complete.\n");
}

void sendText()
{
  Serial.println("Running SendSMS - Run");
      
  TembooChoreo SendSMSChoreo(client);
  
  // Invoke the Temboo client
  SendSMSChoreo.begin();
  
  // Set Temboo account credentials
  SendSMSChoreo.setAccountName(TEMBOO_ACCOUNT);
  SendSMSChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
  SendSMSChoreo.setAppKey(TEMBOO_APP_KEY);
  
  // Set profile to use for execution
  SendSMSChoreo.setProfile("PUT YOUR OWN PROFILE HERE!"); //******* PUT YOUR OWN PROFILE HERE

  // Set Choreo inputs
  String BodyValue = "Your dog has barked " + String(count) + " times.";
  SendSMSChoreo.addInput("Body", BodyValue);
  
  // Identify the Choreo to run
  SendSMSChoreo.setChoreo("/Library/Twilio/SMSMessages/SendSMS");
  
  // Run the Choreo; when results are available, print them to serial
  SendSMSChoreo.run();
  
  while(SendSMSChoreo.available()) {
    char c = SendSMSChoreo.read();
    Serial.print(c);
  }
  SendSMSChoreo.close();
  
  count = 0;
}

void getDate()
{
   Serial.println("Running GetDate - Run");

  TembooChoreo GetDateChoreo(client);

  // Invoke the Temboo client
  GetDateChoreo.begin();

  // Set Temboo account credentials
  GetDateChoreo.setAccountName(TEMBOO_ACCOUNT);
  GetDateChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
  GetDateChoreo.setAppKey(TEMBOO_APP_KEY);

  // Identify the Choreo to run
  GetDateChoreo.setChoreo("/Library/Utilities/Dates/GetDate");

  // Run the Choreo; when results are available, print them to serial
  GetDateChoreo.run();
  int a = 0;
  while(GetDateChoreo.available()) {
    String name = GetDateChoreo.readStringUntil('\x1F');
    name.trim();
    
    String data = GetDateChoreo.readStringUntil('\x1E');
    data.trim();
    
    if(name == "FormattedDate"){
      time = data;
    }
  }
  GetDateChoreo.close();
  Serial.println(time);
}

void appendRow()
{
  Serial.println("Running AppendRow - Run");

  TembooChoreo AppendRowChoreo(client);
  
  // Invoke the Temboo client
  AppendRowChoreo.begin();
  
  // Set Temboo account credentials
  AppendRowChoreo.setAccountName(TEMBOO_ACCOUNT);
  AppendRowChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
  AppendRowChoreo.setAppKey(TEMBOO_APP_KEY);
  
  // Set Choreo inputs
  String PasswordValue = "XXXXXXXXXXX";
  AppendRowChoreo.addInput("Password", PasswordValue);
  String UsernameValue = "XXXXXXXXXXX@gmail.com";
  AppendRowChoreo.addInput("Username", UsernameValue);
  String RowDataValue = String(time) + "," + String(ampReading) + "," + String(totalCount);
  AppendRowChoreo.addInput("RowData", RowDataValue);
  String SpreadsheetTitleValue = "PUT SPREADSHEET TITLE HERE";
  AppendRowChoreo.addInput("SpreadsheetTitle", SpreadsheetTitleValue);
  
  // Identify the Choreo to run
  AppendRowChoreo.setChoreo("/Library/Google/Spreadsheets/AppendRow");
  
  // Run the Choreo; when results are available, print them to serial
  AppendRowChoreo.run();
  
  while(AppendRowChoreo.available()) {
    char c = AppendRowChoreo.read();
    Serial.print(c);
  }
  AppendRowChoreo.close();
}

