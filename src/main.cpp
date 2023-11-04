#include <Arduino.h>
#include <Adafruit_ICM20X.h>
#include <Adafruit_ICM20948.h>
#include <Adafruit_BMP3XX.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <SD.h>
#include <SPI.h>
#include <TeensyThreads.h>

// define LED pins
#define LED_RED 5                           // define Red LED pin - arbitrary
#define LED_GREEN 6                         // define Green LED pin - arbitrary
#define BUZZER 7                            // define buzzer pin - arbitrary
#define NPN_1 8                             // define NPN1 - arbitrary
#define NPN_2 9                             // define NPN2 - arbitrary
#define CS_SD 10                            // define SD CS pin - arbitrary
#define SEALEVELPRESSURE_HPA (1013.25)      // defines seal level pressure
const int BUZZER_TONE = 1000;               // buzzer tone set to 1000kHz
const int BUZZER_TIME_INITIALIZATION = 500; // time that buzzer is on during initialization functions (BUZZER_PASS/FAIL)
const int BUZZER_TIME_ALL_PASS = 8000;      // time that buzzer is left on if all conditions are met
File FILE;                                  // file object
String filename = "flight_data.txt";        // setting filename

Adafruit_ICM20948 icm;
Adafruit_BMP3XX bmp;
uint16_t measurement_delay_us = 65535; // Delay between measurements for testing
sensors_event_t a;                     // acceleration
sensors_event_t g;                     // gyro
sensors_event_t m;                     // magnetic
sensors_event_t t_ICM;                 // temp Celsius
float t_BMP;                           // Celsius
float P_BMP;                           // hPa
int i1, i2, i3, i4;                    // int values for checking Pass on each component
int f = 1;                             // used for switch statement in void loop to jump between flight stages

// prototype functions for initialization processes
void blinkRed_FAIL();   // LED for failure
void BUZZER_FAIL();     // buzzer for failure
void blinkRed_PASS();   // LED for pass
void BUZZER_PASS();     // buzzer for pass
void initialize_LEDS(); // initializes LEDs
void initialize_SD();   // initializes SD
void initialize_ICM();  // initializes ICM
void initialize_BMP();  // initializes BMP

// prototype functions for flight and command
void loitering_1();
void apogee();
void loitering_2();
void altitude_main();
void loitering_3();
void landing();
void post_processing();

// prototype functions for reading sensor data
void read_bmp();
void read_imu();

// prototype functions for math
void accel_mag_imu();
void velocity_mag_imu();
void altitude_bmp();
void pressure_bmp();

void setup()
{
  Serial.begin(115200);
  delay(5000); // giving enough time to react to count and perform checklist items

  bool j = TRUE;
  while (j)
  {
    // set pins
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(BUZZER, OUTPUT);
    pinMode(NPN_1, OUTPUT);
    pinMode(NPN_2, OUTPUT);

    // initialization functions
    initialize_LEDS(); // initializes LEDs
    initialize_SD();   // initializes SD
    initialize_ICM();  // initializes ICM
    initialize_BMP();  // initializes BMP
    if (i1 == 1 && i2 == 1 && i3 == 1)
    {
      j = FALSE;
      tone(BUZZER, BUZZER_TONE);
      delay(BUZZER_TIME_ALL_PASS);
      noTone(BUZZER);
    }
  }
}

void loop()
{
  switch (f)
  {
  case 1:
    // loitering_1
    break;
  case 2:
    // apogee
    break;
  case 3:
    // loitering_2
    break;
  case 4:
    // altitude_main
    break;
  case 5:
    // loitering_3
    break;
  case 6:
    // landing
    break;
  case 7:
    // post processing
    break;
  }
  //  /* Get a new normalized sensor event */
  icm.getEvent(&a, &g, &t_ICM, &m);
  t_BMP = bmp.temperature;
  P_BMP = bmp.pressure;
  float a_magnitude = sqrt(pow(a.acceleration.x, 2) + pow(a.acceleration.y, 2) + pow(a.acceleration.z, 2));
  Serial.print(t_ICM.temperature);
  Serial.print(",");
  Serial.print(a.acceleration.x);
  Serial.print(",");
  Serial.print(a.acceleration.y);
  Serial.print(",");
  Serial.print(a.acceleration.z);
  Serial.print(",");
  Serial.print(m.magnetic.x);
  Serial.print(",");
  Serial.print(m.magnetic.y);
  Serial.print(",");
  Serial.print(m.magnetic.z);
  Serial.print(",");
  Serial.print(g.gyro.x);
  Serial.print(",");
  Serial.print(g.gyro.y);
  Serial.print(",");
  Serial.print(g.gyro.z);
  Serial.print(",");
  Serial.println(a_magnitude);
  delay(500);
}

// functions
void blinkRed_FAIL() // LED for failure
{
  for (int i = 0; i < 4; i++)
  {
    digitalWrite(LED_RED, HIGH); // turns on Red LED
    threads.delay(500);          // continue for 500ms
    digitalWrite(LED_RED, LOW);  // LED off
    threads.delay(500);          // break before for loop continues
  }
}

void BUZZER_FAIL() // buzzer for failure
{
  for (int i = 0; i < 4; i++)
  {
    tone(BUZZER, BUZZER_TONE); // 1KHz sound signal
    threads.delay(500);        // continue for 500ms
    noTone(BUZZER);            // stops sound
    threads.delay(500);        // break before for loop continues
  }
}

void blinkRed_PASS() // LED for pass
{
  for (int i = 0; i < 2; i++)
  {
    digitalWrite(LED_RED, HIGH); // turns on Red LED
    threads.delay(500);          // continue for 500ms
    digitalWrite(LED_RED, LOW);  // LED off
    threads.delay(500);          // break before for loop continues
  }
}

void BUZZER_PASS() // buzzer for pass
{
  for (int i = 0; i < 2; i++)
  {
    tone(BUZZER, BUZZER_TONE); // 1KHz sound signal
    threads.delay(500);        // continue for 500ms
    noTone(BUZZER);            // stops sound
    threads.delay(500);        // break before for loop continues
  }
}

void initialize_LEDS() // initializes LEDs
{
  // initialize LEDs
  digitalWrite(LED_GREEN, HIGH);
  for (int i = 0; i < 4; i++)
  {
    digitalWrite(LED_RED, HIGH);
    delay(500);
    digitalWrite(LED_RED, LOW);
    delay(500);
  }
  delay(5000);
}

void initialize_SD() // initializes SD
{
  // initialize SD card and check
  if (!SD.begin(CS_SD)) // if it doesn't connects
  {
    threads.addThread(blinkRed_FAIL);
    threads.addThread(BUZZER_FAIL);
    i1 = 0;
  }
  else
  {
    threads.addThread(blinkRed_PASS);
    threads.addThread(BUZZER_PASS);
    i1 = 1;
  }
  delay(5000);
}

void initialize_ICM() // initializes ICM
{
  // initialize ICM20948 and check
  if (!icm.begin_I2C()) // if it doesn't connects
  {
    threads.addThread(blinkRed_FAIL);
    threads.addThread(BUZZER_FAIL);
    i2 = 0;
  }
  else
  {
    threads.addThread(blinkRed_PASS);
    threads.addThread(BUZZER_PASS);
    i2 = 1;
  }
  delay(5000);
}

void initialize_BMP() // initializes BMP
{
  // initialize BMP388 and check
  if (!bmp.begin_I2C()) // if it doesn't connects
  {
    threads.addThread(blinkRed_FAIL);
    threads.addThread(BUZZER_FAIL);
    i3 = 0;
  }
  else
  {
    threads.addThread(blinkRed_PASS);
    threads.addThread(BUZZER_PASS);
    i3 = 1;
  }
}