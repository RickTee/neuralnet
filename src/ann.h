/*
 * File: ann.h
 * Program: 
 * Author: Richard G Townsend
 * Email: rtown@blueyonder.co.uk
 * Created on: March 16, 2015, 7:56 AM
 * Copyright (C) 2015  Richard G Townsend
 */
#ifndef __ANN_H__
#define	__ANN_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "neuron.h"
/* Class Id */
#define ANN		(('A'+'N'+'N') << 16)
/* Methods */
#define ANN_DESTROY         ANN + 1
#define ANN_GET             ANN + 2
#define ANN_SET             ANN + 3
#define ANN_PRINT           ANN + 4
/* Attributes */
#define ANN_INPUT_LAYER     ANN + 10
#define ANN_HIDDEN_LAYER    ANN + 11
#define ANN_OUTPUT_LAYER    ANN + 12
#define ANN_MAX             512

/* Forward declarations */
typedef struct _Ann Ann;

/* If this class is using multiple inheritance; add the class ID and use a
 * class dispatcher to send the super classes methods */
struct _Ann
{
    int cls; /* Use if this class needs multiple inheritance */
    //BaseClass *supercls; /* Use if this is a derived class */
    int input_count;
    int hidden_count;
    int output_count;
    float learning_rate;
    float accel_rate;
    float slope;
    float net_error;
    float rms_error;
    Neuron *input_layer[ANN_MAX];
    Neuron *hidden_layer[ANN_MAX];
    Neuron *output_layer[ANN_MAX];
    void (*dispatcher)(void * this, int mthd, int attr, Msg * msg);
};

/* Prototypes */
Ann * ann_construct(int input_count, int hidden_count, int output_count, float LR, float ACL, float SL);
void ann_dispatcher(Ann * this, int mthd, int attr, Msg * msg);
/* Required for multiple inheritance */
//void DoSuperMethod(int cls, Ann * this, int mthd, int attr, Msg * msg);
/* Macros */
#define AnnNew(input_count, hidden_count, output_count, LR, ACL, SL)   ((ann_construct)(input_count, hidden_count, output_count, LR, ACL, SL))
#define AnnDestroy(this)            ((this->dispatcher)(this, ANN_DESTROY, 0, NULL))
#define AnnGet(this, attr, msg)     ((this->dispatcher)(this, ANN_GET, attr, msg))
#define AnnSet(this, attr, msg)     ((this->dispatcher)(this, ANN_SET, attr, msg))
#define AnnPrint(this)              ((this->dispatcher)(this, ANN_PRINT, 0, NULL))


#endif	/*  __ANN_H__ */
