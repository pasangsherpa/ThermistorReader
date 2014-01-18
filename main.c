/*
 * main.c:
 *	Measuring temperature with a thermistor.
 *
 *	@author Pasang Sherpa
 *  @author Aaron Nelson
 *	@author Jonathan Forbes
 *	@author Takatoshi Tomoyose
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include "main.h"

/*
 * Thermistor reading task
 */
PI_THREAD( readTemperature) {
	//Set up the task to sample the A-to-D converter input every 10th second.
	for (;;) {
        // Notify the computation task of data available (use a message queue, for example)
	    // Do nothing until the next reading
	}
}

/*
 * Setup
 */
void setup(void) {
	// initialise the hardware
	// initialize variables
	// Initialize the communications from the thermistor task

	wiringPiSetupSys(); // Setup wiringPi
	piThreadCreate(readTemperature); // Start Thermistor reading task
}

/*
 * Computation/display task
 */
int main(void) {

	setup();

	for (;;) {
		// Receive the data reading from the thermistor reading task when available
		// Compute a running average of the raw data
		// Convert the raw data into temperature units, centegrade and farenheit
		// print the results to the console device every second (update rate) wait for the next reading
	}
	return NO_ERROR;
}
