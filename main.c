/*
 * main.c:
 *	Measuring temperature with a thermistor.
 *
 *	@author Pasang Sherpa
 *	@author Aaron Nelson
 *	@author Jonathan Forbes
 *	@author Takatoshi Tomoyose
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <mcp3004.h>
#include "main.h"

//Conversion factor for analog to digital


/*
 * Thermistor reading task
 */
PI_THREAD( readTemperature) {
	//Set up the task to sample the A-to-D converter input every 10th second.
	for (;;) {
		// Notify the computation task of data available (use a message queue, for example)
		// Do nothing until the next reading
		int value = analogRead(BASE);
		printf("val: %d", value);
		int temp = convert(value);
		printf(" temp in celcius: %d\n", temp);
		delay(100);
	}
}
/*
 *Reporting to GUI
 */
PI_THREAD( reportTemperature) {

}
/*
 * Setup
 */
void setup(void) {
	// initialise the hardware
	// initialize variables
	// Initialize the communications from the thermistor task
	mcp3004Setup(BASE, CHANNEL);
	wiringPiSetupSys(); // Setup wiringPi
	piThreadCreate(readTemperature); // Start Thermistor reading task
}
/*
 *Convert Analog to Digital Value
 */
int convert(int x) {
	return (0.00000000137695*x*x*x*x) - (0.00000289965*x*x*x) + (0.00219812*x*x) - (0.615599*x) + 58.395;
}
/*
 * Computation/display task
 */
int main(void) {

	setup();

	for (;;) {
		//printf("getting data..");
		delay(1000);
		// Receive the data reading from the thermistor reading task when available
		// Compute a running average of the raw data
		// Convert the raw data into temperature units, centegrade and farenheit
		// print the results to the console device every second (update rate) wait for the next reading
	}
	return NO_ERROR;
}
