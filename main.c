
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
	//Send the address of the I2C ADC and set configuration bits
	int fd = wiringPiI2CSetup(0x48);
	printf("FD: %d\n", fd);
	int err0 = wiringPiI2CWrite(fd, 0x90);	//send 7bit I2C address with LSB low for write
	int err1 = wiringPiI2CWrite(fd, 0x01);	//send 0x01 to point to configuration register
	int err2 = wiringPiI2CWrite(fd, 0x04);	//write 0x04 to MSB of config reg
	int err3 = wiringPiI2CWrite(fd, 0x80);	//write 0x80 to LSB of config reg
	int err4 = wiringPiI2CWrite(fd, 0x90);	//send 7bit I2Caddress with LSB low for write
	int err5 = wiringPiI2CWrite(fd, 0x00);	//send 0x00 to point pointer to conversion register
	printf("Error: %d\n", err0);
	//Set up the task to sample the A-to-D converter input every 10th second.
	for (;;) {
		printf("before delay\n");
		int err6 = wiringPiI2CWrite(fd, 0x91);	//send 7bit I2C address with LSB high for read
		//int err6 = wiringPiI2CWrite(fd, 0x00);	//send 0x00 to point to conversion register
		int msb_read = wiringPiI2CReadReg16(fd, 0x01);	//start read of conversion register
		//int  first_byte =  wiringPiI2CReadReg8(fd, 0x91);
		//int msb_read = wiringPiI2CRead(fd);	//read the MSB from the conversion register
		//int  temp_first =  first_byte & 0xF0;
		printf("msb_read: %d\n", msb_read);
		//int  second_byte =  wiringPiI2CReadReg8(fd, (first_byte & 0xF0) | temp_first)
		//int lsb_read = wiringPiI2CReadReg8(fd, 0x00);	//read the LSB from the conversion register
		//int  temp_second =  first_byte & 0x0F;
		//printf("lsb_read: %d\n", lsb_read);
		//int temp_result = (msb_read << 4) | (lsb_read >> 4);
		int sign = msb_read & 0x8000;
		int temp_result = 0;
		if (sign) {

		} else {
			msb_read = msb_read & 0x7FFF;
			temp_result = msb_read >> 4;
		}
		printf("temp: %d\n", temp_result);
  		//int  third_byte =  wiringPiI2CReadReg8(fd, (first_byte & 0x0F) | temp_second);
		//printf("temp_second:\n ", temp_second);
		
		//printf("I2C Read 1: %d\n", first_byte);
		//printf("I2C Read 2: %d\n", second_byte);
		//printf("I2C Read 3: %d\n", third_byte);
		
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

