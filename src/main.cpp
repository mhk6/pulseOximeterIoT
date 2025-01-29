#include "Particle.h" // for Photon
#include <Adafruit_GFX.h> // OLED libraries
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include "MAX30105.h" // MAX30102 HR Sensor library
#include "heartRate.h" // Heart rate calculating algorithm

SYSTEM_MODE(AUTOMATIC);
SYSTEM_THREAD(ENABLED);
SerialLogHandler logHandler(LOG_LEVEL_INFO);

MAX30105 particleSensor;

const byte RATE_SIZE = 4;
byte rates[RATE_SIZE];
byte rateSpot = 0;
long lastBeat = 0;
float beatsPerMinute;
int beatAvg;

#define SCREEN_WIDTH 128 // OLED display width
#define SCREEN_HEIGHT 64 // OLED display height
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int buzzer = A2; // Buzzer pin
int led = D4; // LED pin

// BPM variable for cloud
int bpmCloud = 0;

void setup() {
    Serial.begin(9600); // Start serial monitor
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Initialize OLED
    display.display();

    particleSensor.begin(Wire, I2C_SPEED_FAST); // Initialize MAX30102 sensor
    particleSensor.setup(); // Use default configurations in the library
    particleSensor.setPulseAmplitudeRed(0x1F); // Higher power for Red LED
    particleSensor.setPulseAmplitudeIR(0x1F);  // Higher power for IR LED

    //buzzer pin output
    pinMode(buzzer, OUTPUT);
    pinMode(led, OUTPUT); // Configure LED as output

    // Create BPM cloud variable
    Particle.variable("BPM", bpmCloud);
}

void loop() {
    long irValue = particleSensor.getIR(); // Read IR value

    if (irValue > 7000) { // Finger detected
        display.clearDisplay(); // Clear display each time before drawing new data
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(0, 0);
        display.println("Heart Rate Monitor");

        // If a beat is detected
        if (checkForBeat(irValue)) {
            long delta = millis() - lastBeat; // Time between beats
            lastBeat = millis();

            float bpm = 60 / (delta / 1000.0); // Calculate BPM

            if (bpm > 20 && bpm < 255) { // Valid BPM range
                rates[rateSpot++] = (byte)bpm; // Store the value
                rateSpot %= RATE_SIZE;

                if (rateSpot == 0) { // Only calculate average when buffer is full, can remove if statement for quicker, less accurate results
                    beatAvg = 0;
                    //calculate average
                    for (byte x = 0; x < RATE_SIZE; x++) {
                        beatAvg += rates[x];
                    }
                    beatAvg /= RATE_SIZE;
                }
            }

            if (abs(beatAvg - bpm) > 30 && beatAvg != 0) {
                bpm = beatAvg; // Use average instead of the outlier
            }

            // Play a tone on buzzer
            tone(buzzer, 1000, 100); // 1000Hz for 100ms

            // Flash the LED for a short duration
            digitalWrite(led, HIGH);
            delay(100); // Keep it on for 100 ms, same as buzzer
            digitalWrite(led, LOW);

            // Clear the area where BPM is displayed (clear the old BPM)
            display.fillRect(60, 20, 68, 32, BLACK); // Adjust width and height based on the text size

            // Update display with BPM value
            display.setTextSize(3);
            display.setCursor(0, 20);
            display.println("BPM:");
            display.setCursor(60, 20);
            display.println(int(beatAvg));
            display.display(); // Refresh the display

            // Print to serial monitor for debugging
            Serial.print("Current BPM: ");
            Serial.println(beatAvg);

            // Update the cloud variable and publish an event
            bpmCloud = beatAvg;

        }
    } else { // If no finger detected
        beatAvg = 0; // Reset average BPM
        bpmCloud = 0; // Update cloud variable to indicate no BPM
        display.clearDisplay(); // Clear the display when no finger is detected
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(0, 20);
        display.println("Place your finger");
        display.setCursor(0, 35);
        display.println("on the sensor.");
        display.display(); // Refresh the display

        Serial.println("No finger detected. Waiting...");
    }
}
