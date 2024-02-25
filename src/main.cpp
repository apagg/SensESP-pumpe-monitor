// Signal K application sensESP-smart-lampe.
//
// Lampe som gir hvit sterkt lys på dagen og rødt lys på natten 
// bruker environment.mode for å finne ut om det er natt eller dag
// Kontrolerer WS2812B LED stripe

#include "sensesp/sensors/sensor.h"
#include "sensesp/signalk/signalk_output.h"
#include "sensesp_app_builder.h"
#include <Adafruit_ADS1X15.h>
#include "sensesp/transforms/linear.h"
#include "sensesp/sensors/digital_input.h"
#include "sensesp/transforms/press_repeater.h"
#include "sensesp/transforms/repeat_report.h"


Adafruit_ADS1115 ads;

using namespace sensesp;

String lampe_navn = "sensESP-pumpe-monitor";

reactesp::ReactESP app;
  
float read_ADS_A0() { return (ads.readADC_SingleEnded(0)); }
float read_ADS_A1() { return (ads.readADC_SingleEnded(1)); }
float read_ADS_A2() { return (ads.readADC_SingleEnded(2)); }
float read_ADS_A3() { return (ads.readADC_SingleEnded(3)); }

void setup() {

#ifndef SERIAL_DEBUG_DISABLED
  SetupSerialDebug(115200);
#endif

  // Construct the global SensESPApp() object
  SensESPAppBuilder builder;
  sensesp_app = (&builder)
                    // Set a custom hostname for the app.
                    ->set_hostname(lampe_navn)
                    // Optionally, hard-code the WiFi and Signal K server
                    // settings. This is normally not needed.
                    //->set_wifi("My WiFi SSID", "my_wifi_password")
                    //->set_sk_server("192.168.10.3", 80)
                    ->get_app();
   
  // Start ADS1X15

  ads.begin();

  // I2C pins 
  // SDA Pin = 21;
  // SCL Pin = 22;
    

  unsigned int read_ADS_interval = 1000;
  
  auto pumpe_ADS_A0 = new RepeatSensor<float>(read_ADS_interval, read_ADS_A0);
  auto pumpe_ADS_A1 = new RepeatSensor<float>(read_ADS_interval, read_ADS_A1);
  auto pumpe_ADS_A2 = new RepeatSensor<float>(read_ADS_interval, read_ADS_A2);
  auto pumpe_ADS_A3 = new RepeatSensor<float>(read_ADS_interval, read_ADS_A3);

  
  pumpe_ADS_A0->connect_to(new Linear(0.0021,0,"/pump/A0/Linear"))->connect_to(
    new SKOutputFloat("electrical.pump.A0","/pump/A0/path"));

  pumpe_ADS_A1->connect_to(new Linear(0.0021,0,"/pump/A1/Linear"))->connect_to(
    new SKOutputFloat("electrical.pump.A1","/pump/A1/path"));

  pumpe_ADS_A2->connect_to(new Linear(0.0021,0,"/pump/A2/Linear"))->connect_to(
    new SKOutputFloat("electrical.pump.A2","/pump/A2/path"));

  pumpe_ADS_A3->connect_to(new Linear(0.0021,0,"/pump/A3/Linear"))->connect_to(
    new SKOutputFloat("electrical.pump.A3","/pump/A3/path"));

  const uint8_t kDigitalInput1Pin = 13;
  const unsigned int kDigitalInput1Interval = 1000;
  
  pinMode(kDigitalInput1Pin, INPUT_PULLDOWN);
  
  auto* digital_input1 = new RepeatSensor<bool>(
      kDigitalInput1Interval,
      [kDigitalInput1Pin]() { return digitalRead(kDigitalInput1Pin); });

  digital_input1->connect_to(new SKOutputBool(
      "sensors.digital_input1.value",          // Signal K path
      "/sensors/digital_input1/value",         // configuration path
      new SKMetadata("",                       // No units for boolean values
                     "Digital input 1 value")  // Value description
      ));   

  

  const uint8_t kDigitalInput2Pin = 14;
  const unsigned int kDigitalInput2Interval = 1000;
  
  pinMode(kDigitalInput2Pin, INPUT_PULLDOWN);
  
  auto* digital_input2 = new RepeatSensor<bool>(
      kDigitalInput2Interval,
      [kDigitalInput2Pin]() { return digitalRead(kDigitalInput2Pin); });
  
  
  digital_input2->connect_to(new SKOutputBool(
      "sensors.digital_input2.value",          // Signal K path
      "/sensors/digital_input2/value",         // configuration path
      new SKMetadata("",                       // No units for boolean values
                     "Digital input 2 value")  // Value description
      ));    
  
  
  
  sensesp_app->start();
}

void loop() { app.tick(); }
