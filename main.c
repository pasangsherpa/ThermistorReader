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

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <mcp3004.h>
#include <queue.h>
#include "main.h"

unsigned int time_stamp;
static volatile int value;	//analog value read from thermistor
static volatile int comp_value;	//value used in computation thread
static volatile int counter = 0;
QueueRecord queue;


#define KEY_NUM 	0
#define ALPHA		0.3

pthread_mutex_t queue_mutex;
pthread_cond_t queue_not_empty;


/*
 * Thermistor reading task
 */
PI_THREAD( readTemperature) {
	//Set up the task to sample the A-to-D converter input every 10th second.
	for (;;) {
		// Notify the computation task of data available (use a message queue, for example)
		// Do nothing until the next reading
		//piLock(KEY_NUM);
		value = analogRead(BASE);
		pthread_mutex_lock(&queue_mutex);
		Enqueue(value, queue); //put value into queue
		pthread_cond_signal(&queue_not_empty);
		pthread_mutex_unlock(&queue_mutex);
		//piUnlock(KEY_NUM);
		printf("Millis: %d\n", millis() - time_stamp);		
		time_stamp = millis();
		delay(100);
	}
}
/*
 *Reporting to GUI
 */
PI_THREAD( reportTemperature) {
	
	pthread_cond_wait(&queue_not_empty, &queue_mutex);
	counter = (counter + 1) % 10;
	comp_value =  Dequeue(queue);  //dequeue the real value
	pthread_mutex_unlock(&queue_mutex);
	//add value to average
	Tavg_new = (ALPHA * comp_value) + (1 - ALPHA) * Tavg_new;
	if (counter == 0) {
		int temp_c = convert(Tavg_new);
		int temp_f = (temp_c * 9)/5 + 32;
		printf("The current temperature is %d Farenheit which equals %d in Celcius\n", temp_f, tempc); 
	}
}
/*
 * Setup
 */
void setup(void) {
	// initialise the hardware
	// initialize variables
	// Initialize the communications from the thermistor task
	queue = CreateQueue(20);	//create a queue that can hold 20 values
	mcp3004Setup(BASE, CHANNEL);
	wiringPiSetupSys(); // Setup wiringPi
	piThreadCreate(readTemperature); // Start Thermistor reading task
	piThreadCreate(reportTemperature); //Start Reporting to console task
	time_stamp = millis();
}
/*
 *Convert Digital Value to degree Celcius
 */
int convert(int x) {
	return (0.00000000137695*x*x*x*x) - (0.00000289965*x*x*x) + (0.00219812*x*x) - (0.615599*x) + 58.395;
}
/*
 * Computation/display task
 */
int main(void) {
	int current_reading = 0;
	int previous_reading = 0;
	setup();

	for (;;) {
		//printf("getting data..");
		while (current_reading == previous_reading) {
			piLock(KEY_NUM);
			printf(" raw value: %d : \n", value);
			current_reading = convert(value);
			piUnlock(KEY_NUM);
		}
		printf(" temp in celcius: %d : \n", current_reading);
		previous_reading = current_reading;
		delay(1000);
		// Receive the data reading from the thermistor reading task when available
		// Compute a running average of the raw data
		// Convert the raw data into temperature units, centegrade and farenheit
		// print the results to the console device every second (update rate) wait for the next reading
	}
	return NO_ERROR;
}
