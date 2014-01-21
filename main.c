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
#include <wiringPiI2C.h>
#include "main.h"

/*
 * Thermistor reading task
 */
PI_THREAD( readTemperature) {
	int fd = wiringPiI2CSetup(0x48);
	printf("FD: %d\n", fd);
	int err0 = wiringPiI2CWrite(fd, 0x90);
	int err1 = wiringPiI2CWrite(fd, 0x01);
	int err2 = wiringPiI2CWrite(fd, 0x04);
	int err3 = wiringPiI2CWrite(fd, 0x83);
	int err4 = wiringPiI2CWrite(fd, 0x90);
	int err5 = wiringPiI2CWrite(fd, 0x00);
	printf("Error: %d\n", err0);
	//Set up the task to sample the A-to-D converter input every 10th second.
	for (;;) {
		printf("before delay\n");
		int  first_byte =  wiringPiI2CReadReg8(fd, 0x91);
		int  temp_first =  first_byte & 0xF0;
		printf("temp_first: \n", temp_first);
		int  second_byte =  wiringPiI2CReadReg8(fd, (first_byte & 0xF0) | temp_first);
		int  temp_second =  first_byte & 0x0F;

		int  third_byte =  wiringPiI2CReadReg8(fd, (first_byte & 0x0F) | temp_second);
		printf("temp_second:\n ", temp_second);
		
		printf("I2C Read 1: %d\n", first_byte);
		printf("I2C Read 2: %d\n", second_byte);
		printf("I2C Read 3: %d\n", third_byte);
		
		delay(500);
		printf("after delay\n");
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
