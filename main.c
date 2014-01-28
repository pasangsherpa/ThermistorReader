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
#include <strings.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <mcp3004.h>
#include "main.h"

unsigned int DEBUG_MODE;
unsigned int time_stamp;
static volatile int value;	//analog value read from thermistor
static volatile int comp_value;	//value used in computation thread
static volatile int counter = 0;

pthread_mutex_t value_mutex;
pthread_cond_t value_cond;


/*
 * Thermistor reading task
 */
PI_THREAD( readTemperature) {
	//Set up the task to sample the A-to-D converter input every 10th second.
	for (;;) {
		if (DEBUG_MODE) printf("Millis: %d \n", millis() - time_stamp);
		pthread_mutex_lock(&value_mutex);
		value = analogRead(BASE);
		pthread_cond_signal(&value_cond);
		pthread_mutex_unlock(&value_mutex);
		time_stamp = millis();
		delay(100); // Do nothing until the next reading
	}
}

/*
 * Setup
 */
void setup(void) {
	mcp3004Setup(BASE, CHANNEL); // Setup SPI
	wiringPiSetupSys(); // Setup wiringPi
	time_stamp = millis();
	piThreadCreate(readTemperature); // Start Thermistor reading task
}

/*
 *Convert Digital Value to degree Celcius
 */
float convert(int x) {
	return (0.00000000137695*x*x*x*x) - (0.00000289965*x*x*x) + (0.00219812*x*x) - (0.615599*x) + 58.395;
}

/*
 * Computation/display task
 */
int main(int argc, char * argv[]) {

	if(strcasecmp("-d", argv[1]) == 0){
		DEBUG_MODE  = atoi(argv[2]); // set debug mode
        }

	setup();

	for (;;) {
	        pthread_cond_wait(&value_cond, &value_mutex);
	        counter = (counter + 1) % 10;
	        comp_value =  value;
       		pthread_mutex_unlock(&value_mutex);
	        //add value to average
        	float Tavg_new = (ALPHA * comp_value) + (1 - ALPHA) * Tavg_new;
	        if (counter == 0) {
			// Convert the raw data into temperature units, centegrade and farenheit
               		float temp_c = convert(Tavg_new);
                	float temp_f = (temp_c * 9)/5 + 32;
                	printf("Current temperature is %6.3f °F | %6.3f °C\n\n", temp_f, temp_c);
        	}
	}
	return NO_ERROR;
}
