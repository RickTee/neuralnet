/*
 * File: main.c
 * Program:
 * Author: Richard G Townsend
 * Email: ricktee@virginmedia.com
 * Created on: September 4, 2014, 9:24 AM
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "neuron.h"
#include "ann.h"
/* Prototypes */
static void randomize(void);

int main(void)
{
	Ann *myann;
        Msg msg;
        
        msg.desired = 1002;
        randomize();
        
	myann = AnnNew(5, 7, 5, 0.5, 0.3, -3);
	AnnPrint(myann);
        AnnDestroy(myann);
	
	return(EXIT_SUCCESS);
}

static void randomize(void)
{
    time_t ltime;
    unsigned int stime;

    ltime = time(NULL);
    ltime = ltime << 21;
    stime = (unsigned) ltime / 2;
    srand(stime);
}
