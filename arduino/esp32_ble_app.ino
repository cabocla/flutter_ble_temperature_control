//BLE
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;

#define SERVICE_UUID        "5288f3b2-c465-4273-b93b-fa6de364fe83"
#define CHARACTERISTIC_UUID "910e51f5-ab6f-41cf-a946-305feb203ca4"

//parameter settings

float setTempList[5];
bool trigList[5];
float setTemp1;
float setTemp2;
float setTemp3;
float setTemp4;
float setTemp5;

float setTemperature1;
float setTemperature2;
float setTemperature3;
float setTemperature4;
float setTemperature5;

float temperature1;
float temperature2;
float temperature3;
float temperature4;
float temperature5;

bool trigOn1 = false;
bool trigOn2 = false;
bool trigOn3 = false;
bool trigOn4 = false;
bool trigOn5 = false;

bool triggerOn1 = false;
bool triggerOn2 = false;
bool triggerOn3 = false;
bool triggerOn4 = false;
bool triggerOn5 = false;

char valueChar[64];
char *temp1;
char *temp2;
char *temp3;
char *temp4;
char *temp5;

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      BLEDevice::startAdvertising();
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

class MyCallbacks: public BLECharacteristicCallbacks{

  void onWrite(BLECharacteristic *pCharacteristic){
    std::string value = pCharacteristic->getValue();

    if(value.length()>0){
      memset(valueChar, 0, sizeof(valueChar));
      
      Serial.println("*****************");
      Serial.println("New Value:");

      for(int i=0; i<value.length(); i++){

        valueChar[i] = value[i];
    
      }

int     index = atoi(strtok(valueChar," "));
String     value = strtok(NULL, ",");

if(index == 1){
  if(value == "true"){
    trigOn1 = true;
  }else if (value == "false"){
    trigOn1 = false;
  }else{
    setTemp1 = value.toFloat();
  }
}else if(index == 2){
  if(value == "true"){
    trigOn2 = true;
  }else if (value == "false"){
    trigOn2 = false;
  }else{
    setTemp2 = value.toFloat();
  }
}else if(index == 3){
  if(value == "true"){
    trigOn3 = true;
  }else if (value == "false"){
    trigOn3 = false;
  }else{
    setTemp3 = value.toFloat();
  }
}else if(index == 4){
  if(value == "true"){
    trigOn4 = true;
  }else if (value == "false"){
    trigOn4 = false;
  }else{
    setTemp4 = value.toFloat();
  }
}else if(index == 5){
  if(value == "true"){
    trigOn5 = true;
  }else if (value == "false"){
    trigOn5 = false;
  }else{
    setTemp5 = value.toFloat();
  }
}

     
//      setTemp1 = atof(strtok(valueChar, " "));
//      setTemp2 = atof(strtok(NULL, " "));
//      setTemp3 = atof(strtok(NULL, " "));
//      setTemp4 = atof(strtok(NULL, " "));
//      setTemp5 = atof(strtok(NULL, " "));
//
//      char *trig1 = strtok(NULL,",");
//      char *trig2 = strtok(NULL," ");
//      char *trig3 = strtok(NULL," ");
//      char *trig4 = strtok(NULL," ");
//      char *trig5 = strtok(NULL," ");
//      
//      if(trig1=="true"){
//        trigOn1 = true;
//      }else{
//        trigOn1 = false;
//      }
//      if(trig2=="true"){
//        trigOn2 = true;
//      }else{
//        trigOn2 = false;
//      }
//      if(trig3=="true"){
//        trigOn3 = true;
//      }else{
//        trigOn3 = false;
//      }
//      if(trig4=="true"){
//        trigOn4 = true;
//      }else{
//        trigOn4 = false;
//      }
//      if(trig5=="true"){
//        trigOn5 = true;
//      }else{
//        trigOn5 = false;
//      }
//
//      Serial.println(setTemp1);
//      Serial.println(setTemp2);
//      Serial.println(setTemp3);
//      Serial.println(setTemp4);
//      Serial.println(setTemp5);
//      Serial.println();
//      Serial.println(trig1);
//      Serial.println(trig2);
//      Serial.println(trig3);
//      Serial.println(trig4);
//      Serial.println(trig5);
Serial.println(index);
Serial.println(value);
      Serial.println();
      Serial.println("*****************");
      
    }

    
  }
};

//sensor & acuator
#include "DHT.h"
#define DHTTYPE DHT22 

#define DHTPin1  23
#define DHTPin2  22
#define DHTPin3  21
#define DHTPin4  19
#define DHTPin5  18

#define relayPin1 32
#define relayPin2 33
#define relayPin3 25
#define relayPin4 17
#define relayPin5 16

#define motorPin1 27
#define motorPin2 14
#define motorPin3 15
#define motorPin4 13
#define motorPin5 12

DHT dht1(DHTPin1, DHTTYPE);
DHT dht2(DHTPin2, DHTTYPE);
DHT dht3(DHTPin3, DHTTYPE);
DHT dht4(DHTPin4, DHTTYPE);
DHT dht5(DHTPin5, DHTTYPE);

//Thread
#include <Thread.h>
#include <ThreadController.h>

ThreadController control = ThreadController();

Thread* btThread = new Thread();
Thread* drawingThread = new Thread();

void setup() {

 
  
 Serial.begin(115200);
  dht1.begin();
    dht2.begin();
    dht3.begin();
    dht4.begin();
    dht5.begin();

  initDrawing();
  initBT();

setTemp1 = 30;
setTemp2 = 30;
setTemp3 = 30;
setTemp4 = 30;
setTemp5 = 30;

trigOn1 = false;
trigOn2 = false;
trigOn3 = false;
trigOn4 = false;
trigOn5 = false;
 
pinMode(motorPin1, OUTPUT); // Set pin for output to control TIP120 Base pin
pinMode(motorPin2, OUTPUT);
pinMode(motorPin3, OUTPUT);
pinMode(motorPin4, OUTPUT);
pinMode(motorPin5, OUTPUT);

pinMode(relayPin1, OUTPUT);
pinMode(relayPin2, OUTPUT);
pinMode(relayPin3, OUTPUT);
pinMode(relayPin4, OUTPUT);
pinMode(relayPin5, OUTPUT);
digitalWrite(relayPin1, HIGH);
  digitalWrite(relayPin2, HIGH);
  digitalWrite(relayPin3, HIGH);
  digitalWrite(relayPin4, HIGH);
  digitalWrite(relayPin5, HIGH);
  
 btThread->onRun(btCallback);
 btThread->setInterval(1000);

 drawingThread->onRun(drawingCallback);
 drawingThread->setInterval(5000);

 control.add(btThread);
 control.add(drawingThread);

 

}

void loop() {
  // put your main code here, to run repeatedly:

  
  
  control.run();
  if(triggerOn1){
    controlTemperature(motorPin1, relayPin1, setTemperature1, temperature1, triggerOn1);
  }
  if(triggerOn2){
  controlTemperature(motorPin2, relayPin2, setTemperature2, temperature2, triggerOn2);  
  }
  if(triggerOn3){
  controlTemperature(motorPin3, relayPin3, setTemperature3, temperature3, triggerOn3);  
  }
  if(triggerOn4){
  controlTemperature(motorPin4, relayPin4, setTemperature4, temperature4, triggerOn4);  
  }
  if(triggerOn5){
  controlTemperature(motorPin5, relayPin5, setTemperature5, temperature5, triggerOn5);  
  }

}

void drawingCallback(){
  temperature1 = dht1.readTemperature();
  temperature2 = dht2.readTemperature();
  temperature3 = dht3.readTemperature();
  temperature4 = dht4.readTemperature();
  temperature5 = dht5.readTemperature();

  setTemperature1 = setTemp1;
  setTemperature2 = setTemp2;
  setTemperature3 = setTemp3;
  setTemperature4 = setTemp4;
  setTemperature5 = setTemp5;

  triggerOn1 = trigOn1;
  triggerOn2 = trigOn2;
  triggerOn3 = trigOn3;
  triggerOn4 = trigOn4;
  triggerOn5 = trigOn5;

  if(isnan(temperature1)){
    temperature1 = 0;
  }
  if(isnan(temperature2)){
    temperature2 = 0;
  }
  if(isnan(temperature3)){
    temperature3 = 0;
  }
  if(isnan(temperature4)){
    temperature4 = 0;
  }
  if(isnan(temperature5)){
    temperature5 = 0;
  }

  
  
}

void btCallback(){
  // notify changed value
    if (deviceConnected) {
      String temperature = "";

      temperature+=temperature1;
      temperature+=" ";
      temperature+=temperature2;
      temperature+=" ";
      temperature+=temperature3;
      temperature+=" ";
      temperature+=temperature4;
      temperature+=" ";
      temperature+=temperature5;
      temperature+=" ";
      temperature+=",";
      temperature+=setTemperature1;
      temperature+=" ";
      temperature+=setTemperature2;
      temperature+=" ";
      temperature+=setTemperature3;
      temperature+=" ";
      temperature+=setTemperature4;
      temperature+=" ";
      temperature+=setTemperature5;
      temperature+=" ";
       temperature+=",";
       
       if(triggerOn1){
        temperature+="true";
       }else {
        temperature+="false";
       }
      temperature+=" ";
      if(triggerOn2){
        temperature+="true";
       }else {
        temperature+="false";
       }
      temperature+=" ";
      if(triggerOn3){
        temperature+="true";
       }else {
        temperature+="false";
       }
      temperature+=" ";
      if(triggerOn4){
        temperature+="true";
       }else {
        temperature+="false";
       }
      temperature+=" ";
      if(triggerOn5){
        temperature+="true";
       }else {
        temperature+="false";
       }
      temperature+=" ";
    
        pCharacteristic->setValue((char*)temperature.c_str());
        pCharacteristic->notify();
     
   
    }
    // disconnecting
    if (!deviceConnected && oldDeviceConnected) {
        //delay(500); // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        Serial.println("start advertising");
        oldDeviceConnected = deviceConnected;
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected) {
        // do stuff here on connecting
        oldDeviceConnected = deviceConnected;
    }
}

void initBT(){
   // Create the BLE Device
  BLEDevice::init("ESP32 Temperature Control");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );

  pCharacteristic->setCallbacks(new MyCallbacks());

  // https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.descriptor.gatt.client_characteristic_configuration.xml
  // Create a BLE Descriptor
  pCharacteristic->addDescriptor(new BLE2902());

  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();
  Serial.println("Waiting a client connection to notify...");
}

void initDrawing(){
  
}

void updateTemperature(String value){
  
}

void controlTemperature(int motorPin, int relayPin, float setTemp, float temperature, bool triggerOn){
  bool fanOn = false;
  bool heaterOn = false;

//Serial.print(setTemp); Serial.println(triggerOn);

  if(temperature==0){
    fanOn = false;
    heaterOn = false;
    return;
  }

  if(!triggerOn){
    fanOn = false;
    heaterOn = false;
  }
  if(temperature > setTemp+0.5 && triggerOn){
    fanOn = true;
    heaterOn = false;
  }
   if (temperature < setTemp-0.5 && triggerOn){
    heaterOn = true;
    fanOn = false;
  } if (temperature <= setTemp && fanOn){
    fanOn = false;
  } if (temperature >= setTemp && heaterOn){
    heaterOn = false;
  } if(temperature == setTemp){
    fanOn = false;
    heaterOn = false;
  }

  if(fanOn){
    digitalWrite(motorPin, HIGH);
  }
  else if (heaterOn){
    digitalWrite(relayPin, LOW);
  }
}
