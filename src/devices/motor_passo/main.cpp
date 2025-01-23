// Includes the Arduino Stepper Library
#include <Arduino.h>
#include <Stepper.h>

// Defines the number of steps per rotation
const int stepsPerRevolution = 2038;

// Creates an instance of stepper class
// Pins entered in sequence IN1-IN3-IN2-IN4 for proper step sequence
Stepper myStepper = Stepper(stepsPerRevolution, 8, 10, 9, 11);

// Position counter (tracks the current position of the stepper in steps)
long stepperPosition = 0;

void setup() {
    Serial.begin(9600); // Initialize Serial Monitor for debugging
}

void loop() {
    // Rotate CW slowly at 5 RPM
    myStepper.setSpeed(5);
    myStepper.step(stepsPerRevolution); // One full revolution
    stepperPosition += stepsPerRevolution; // Update position counter

    // Calculate angle (mapping steps to degrees)
    float angle = (float(stepperPosition % stepsPerRevolution) / stepsPerRevolution) * 360;
    Serial.print("Current Position (CW): ");
    Serial.print(stepperPosition);
    Serial.print(" steps, Angle: ");
    Serial.print(angle);
    Serial.println("°");
    delay(1000);

    // Rotate CCW quickly at 10 RPM
    myStepper.setSpeed(10);
    myStepper.step(-stepsPerRevolution); // One full revolution CCW
    stepperPosition -= stepsPerRevolution; // Update position counter

    // Calculate angle (mapping steps to degrees)
    angle = (float(stepperPosition % stepsPerRevolution) / stepsPerRevolution) * 360;

    // Handle negative positions to keep the angle between 0 and 360 degrees
    if (angle < 0) {
        angle += 360;
    }

    Serial.print("Current Position (CCW): ");
    Serial.print(stepperPosition);
    Serial.print(" steps, Angle: ");
    Serial.print(angle);
    Serial.println("°");
    delay(1000);
}
