/*
 * Copyright 2010-2015 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *  http://aws.amazon.com/apache2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

#include <aws_iot_mqtt.h>
#include <aws_iot_version.h>
#include "aws_iot_config.h"

aws_iot_mqtt_client myClient; // init iot_mqtt_client
char msg[200]; // read-write buffer
int cnt = 0; // loop counts
int rc = -100; // return value placeholder
int initialWeight = 200;
int weight = initialWeight;
int weightChange = 5;
int thresholdWeight = 20;
int initialQuantity = 1000;
int quantityChange = 10;
int thresholdQuantity = 50;
int quantity = initialQuantity;


/*const int buttonPin = 2; // number of the pushbutton pin
const int ledPin = 13; // number of led pin
int buttonState = 0;
*/

// Basic callback function that prints out the message
void msg_callback(char* src, int len) {
  Serial.println("CALLBACK:");
  int i;
  for(i = 0; i < len; i++) {
    Serial.print(src[i]);
  }
  Serial.println("");
}

void setup() {
  // Start Serial for print-out and wait until it's ready
  Serial.begin(115200);
  while(!Serial);
  //
  char curr_version[80];
  sprintf(curr_version, "AWS IoT SDK Version(dev) %d.%d.%d-%s\n", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, VERSION_TAG);
  Serial.println(curr_version);
  // Set up the client
  if((rc = myClient.setup(AWS_IOT_CLIENT_ID)) != 0) {
    Serial.println("Setup failed!");
    Serial.println(rc);
  }
  // Load user configuration
  if((rc = myClient.config(AWS_IOT_MQTT_HOST, AWS_IOT_MQTT_PORT, AWS_IOT_ROOT_CA_PATH, AWS_IOT_PRIVATE_KEY_PATH, AWS_IOT_CERTIFICATE_PATH)) != 0) {
    Serial.println("Config failed!");
    Serial.println(rc);
  }
  // Use default connect: 60 sec for keepalive
  if((rc = myClient.connect()) != 0) {
    Serial.println("Connect failed!");
    Serial.println(rc);
  }
  // Subscribe to "topic/buttonClickRequests"
  if((rc = myClient.subscribe(AWS_IOT_MQTT_TOPIC, 1, msg_callback)) != 0) {
    Serial.println("Subscribe failed!");
    Serial.println(rc);
  }
  // Delay to make sure SUBACK is received, delay time could vary according to the server
  delay(2000);

  // set pin mode for push button pin and led pin
 /* pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
  */
}

void loop() {

         while((weight> 0 && weight >= thresholdWeight) && (quantity > 0 && quantity >= thresholdQuantity)){
              sprintf(msg, "{\"pickLocation\" : \"A0001\", \"quantity\" : \"%d\", \"weight\" : \"%d\", \"warehouse\" : \"DY00\", \"zone\" : \"A\", \"pickType\": \"A\", \"sensorId\" : \"1\", \"partNumber\" : \"HX0001\"}", quantity, weight);
              if((rc = myClient.publish(AWS_IOT_MQTT_TOPIC, msg, strlen(msg), 1, false)) != 0) {
                Serial.println("Publish failed!");
                Serial.println(rc);
              }
            
              // Get a chance to run a callback
              if((rc = myClient.yield()) != 0) {
                Serial.println("Yield failed!");
                Serial.println(rc);
              }
              weight = weight - weightChange;
              quantity = quantity - quantityChange;
              delay(1000);      
         }
         weight = initialWeight;
         quantity = initialQuantity;
         cnt++;

/*  // read the state of push button
   buttonState = digitalRead(buttonPin);

  // button state is high, turn the ON the led and send message to topic/buttonClickRequests on AWS
  if(buttonState == HIGH){
      digitalWrite(ledPin, HIGH);

      // Generate a new message in each loop and publish to "topic/buttonClickRequests"
      sprintf(msg, "{\"REQUEST_TXT\" : \"Count %d\"}", cnt++);
      if((rc = myClient.publish(AWS_IOT_MQTT_TOPIC, msg, strlen(msg), 1, false)) != 0) {
        Serial.println("Publish failed!");
        Serial.println(rc);
      }
    
      // Get a chance to run a callback
      if((rc = myClient.yield()) != 0) {
        Serial.println("Yield failed!");
        Serial.println(rc);
      }
  }else{
    digitalWrite(ledPin, LOW);
  }
 */ 
  delay(1000);
}
