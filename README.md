# teensy-bird
1.	Initialize sensors
a.	SD card
b.	Accelerometer
c.	Barometer
d.	RF
e.	Blink LEDs 
f.	Buzz when initialized
2.	Ask for sensor data with time
a.	Process and check for magnitudes
b.	If statement for value
c.	Thread 1: Check for conditions of deployment and execute
d.	Thread 2: Log data to SD card and record milestones
3.	When landed buzzer to create noise

Stages:
1.	Loitering1 (for launchpad): Collecting data and waiting for values of apogee
a.	Thread 1: send RF for increments of altitude w/ current velocity
b.	Thread 2: Collect data to SD card
2.	Apogee:
a.	Thread 1: deploy drogue on NPN1
i.	Set NPN1 to high
ii.	Send RF for drogue deploy with current altitude
b.	Thread 2: Log entry for apogee event to SD card
3.	Loitering2 (waiting for altitude for main): Collecting data and waiting for values of altitude.
4.	Altitude_main:
a.	Thread 1: deploy main on NPN2
i.	Set NPN2 to high
ii.	Send RF for main deploy with current altitude and velocity
b.	Thread 2: Log entry for altitude event to SD card
5.	Loitering3 (waiting for landing event): Collecting data
6.	Landing: detects velocity is 0
a.	Thread 1: Send RF for landing
b.	Thread 2: Log to SD card landing event, Save SD card Data, and close.
c.	Buzz function for location
Steps for writing code
1.	Write the initialization of each sensor
a.	Set green and red led to high
b.	SD card initializing
i.	Buzz 2 times and blink red 2 times when initialized correctly
ii.	Delay (5000)
iii.	Buzz 4 times and blink red 4 times when not initialized correctly
c.	Accelerometer initialization
i.	Buzz 2 times and blink red 2 times when initialized correctly
1.	Stores initial conditions to SD card
ii.	Delay (5000)
iii.	Buzz 4 times and blink red 4 times when not initialized correctly
d.	Barometer initialization
i.	Buzz 2 times and blink red 2 times when initialized correctly
1.	Stores initial conditions to SD card
ii.	Delay (5000)
iii.	Buzz 4 times and blink red 4 times when not initialized correctly
e.	RF
i.	Buzz 2 times and blink red 2 times when initialized correctly
ii.	Delay (5000)
iii.	Buzz 4 times and blink red 4 times when not initialized correctly
f.	If all conditions are met Buzz 8 seconds

2.	Write the sensor pulling functions and store to pointers
a.	Create pointer objects
b.	Use I2C
3.	Write functions for calculating the magnitudes of events
a.	Calculate magnitude of acceleration
b.	Calculate velocity
c.	Store raw data and calculated magnitude with respect to time
4.	Write conditional statements for determining Stage 1-6
a.	Loitering1:
i.	Standard function
ii.	Looks for conditions met for apogee
b.	Apogee
i.	Performs Thread functions 1 and 2
ii.	Moves on to Loitering 2
c.	Loitering2: 
i.	Standard function
ii.	Looks for conditions met for Altitude_main
d.	Altitude_main
i.	Performs Thread functions 1 and 2
ii.	Moves on to Loitering 3
e.	Loitering 3
i.	Standard function
ii.	Looks for conditions met for Landing
f.	Landing 
i.	Performs Thread functions 1 and 2
ii.	Sets Buzzer locating function
g.	Post processing
i.	Perform math calculations for flight to be processed later while rocket waits in the field for recovery
ii.	Continues Buzzer locating function
![image](https://github.com/bshipdog/teensy-bird/assets/81502227/62d51332-6178-4db3-8acb-788048a7eb51)
