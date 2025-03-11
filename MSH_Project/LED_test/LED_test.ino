#include "SparkFun_AS7265X.h"
AS7265X sensor;

#include <Wire.h>

// Define test parameters
const unsigned long testDuration = 5 * 60 * 1000;  // 5 minutes in milliseconds
const unsigned long flashInterval = 10000;          // 10 seconds in milliseconds
bool LED_IR = true;

void setup() {
    Serial.begin(115200);
    Serial.println("AS7265x Spectral Triad Example - Consistency Test");

    if (sensor.begin() == false) {
        Serial.println("Sensor does not appear to be connected. Check wiring!");
        while (1);
    }

    sensor.setGain(16);
    sensor.setIntegrationCycles(112);  // This value may need tuning (711 for max reading length); 112 and below and then c channel no longer overflows

    sensor.disableIndicator();  // Disable status LED to reduce interference
    if (LED_IR == true){
        sensor.setBulbCurrent(100, AS7265x_LED_WHITE);
        sensor.setBulbCurrent(100, AS72653_UV);
        sensor.setBulbCurrent(100, AS7265x_LED_IR);
    }
    else{
        sensor.setBulbCurrent(100, AS7265x_LED_WHITE);
        sensor.setBulbCurrent(100, AS72653_UV); // Leave infrared LED out because it interferes with UV
        // IR interferes with UV & Visible light
    }
    
    Serial.println("Time(ms),A,B,C,D,E,F,G,H,R,I,S,J,T,U,V,W,K,L");
}

void loop() {
    static unsigned long startTime = millis();
    unsigned long currentTime = millis();

    if (currentTime - startTime <= testDuration) {
        // Check if it's time to flash the LEDs and take a measurement
        if ((currentTime - startTime) % flashInterval == 0) {
            // Enable LEDs
            if (LED_IR == true){
                sensor.enableBulb(AS7265x_LED_WHITE);
                sensor.enableBulb(AS72653_UV);
                sensor.enableBulb(AS7265x_LED_IR);
            }
            else{
                sensor.enableBulb(AS7265x_LED_WHITE);
                sensor.enableBulb(AS72653_UV);
            }

            delay(100);  // Short delay to ensure LEDs are on (adjust as needed)

            // Take measurement with LEDs on
            sensor.takeMeasurementsWithBulb();

            // Print the time (ms)
            Serial.print(currentTime - startTime);  // Print timestamp
            Serial.print(",");

            // Print spectral data
            Serial.print(sensor.getCalibratedA());  // 410nm
            Serial.print(",");
            Serial.print(sensor.getCalibratedB());  // 435nm
            Serial.print(",");
            Serial.print(sensor.getCalibratedC());  // 460nm
            Serial.print(",");
            Serial.print(sensor.getCalibratedD());  // 485nm
            Serial.print(",");
            Serial.print(sensor.getCalibratedE());  // 510nm
            Serial.print(",");
            Serial.print(sensor.getCalibratedF());  // 535nm
            Serial.print(",");
            Serial.print(sensor.getCalibratedG());  // 560nm
            Serial.print(",");
            Serial.print(sensor.getCalibratedH());  // 585nm
            Serial.print(",");
            Serial.print(sensor.getCalibratedR());  // 610nm
            Serial.print(",");
            Serial.print(sensor.getCalibratedI());  // 645nm
            Serial.print(",");
            Serial.print(sensor.getCalibratedS());  // 680nm
            Serial.print(",");
            Serial.print(sensor.getCalibratedJ());  // 705nm
            Serial.print(",");
            Serial.print(sensor.getCalibratedT());  // 730nm
            Serial.print(",");
            Serial.print(sensor.getCalibratedU());  // 760nm
            Serial.print(",");
            Serial.print(sensor.getCalibratedV());  // 810nm
            Serial.print(",");
            Serial.print(sensor.getCalibratedW());  // 860nm
            Serial.print(",");
            Serial.print(sensor.getCalibratedK());  // 900nm
            Serial.print(",");
            Serial.print(sensor.getCalibratedL());  // 940nm
            Serial.print(",");
            Serial.println();

            // Disable LEDs
            if (LED_IR == true){
                sensor.disableBulb(AS7265x_LED_WHITE);
                sensor.disableBulb(AS72653_UV);
                sensor.disableBulb(AS7265x_LED_IR);
            }
            else{
                sensor.disableBulb(AS7265x_LED_WHITE);
                sensor.disableBulb(AS7265x_LED_UV);
            }
        }
    } else {
        Serial.println("Test complete.");
        while (true);  // Stop the program
    }
}
