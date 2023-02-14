/*
 * File: neuron.h
 * Program:
 * Author: Richard G Townsend
 * Email: ricktee@virginmedia.com
 * Created on: September 4, 2014, 9:38 AM
 */
#ifndef NEURON_H
#define	NEURON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
/* Class Id */
#define NEURON              (('N'+'E'+'U'+'R'+'O'+'N') << 16)
/* Methods */
#define DESTROY             NEURON + 1
#define GET                 NEURON + 2
#define SET                 NEURON + 3
#define CALC_OUTPUT         NEURON + 4
#define CALC_ERROR          NEURON + 5
#define ADJUST_WEIGHTS      NEURON + 6
#define ADJUST_THRESHOLD    NEURON + 7
#define PRINT               NEURON + 8
/* Attributes */
#define NEURON_INPUT        NEURON + 10
#define NEURON_SLOPE        NEURON + 11
#define NEURON_THRESHOLD    NEURON + 12
#define NEURON_ACTIVE       NEURON + 13
#define NEURON_OUTPUT       NEURON + 14
#define NEURON_DESIRED      NEURON + 15
#define NEURON_ERROR        NEURON + 16
#define NEURON_WEIGHTS      NEURON + 17
/* Other */
#define NEURON_SIZE         10
/* Forward declarations */
typedef struct _Msg Msg;
typedef struct _Neuron Neuron;

struct _Msg
{
    int inputCnt;
    int *inputs;
    float slope;
    float threshold;
    float output[NEURON_SIZE];
    //float desired_out;
    float desired;
    float test_pattern;
    float sum;
    float error;
    float weight;
    float LR;
    float ACL;
};

/* If this class is using multiple inheritance; add the class ID and use a
 * class dispatcher to send the super classes methods */
struct _Neuron
{
    int cls;
    int inputCnt;
    int inputs[NEURON_SIZE];
    float slope;
    float threshold;
    float weights[NEURON_SIZE];
    float wchange[NEURON_SIZE];
    float tchange;
    float desired;
    float error;
    float active;
    float output;
    /* extension */
    int stateFlag;
    int id;
    void (*dispatcher)(Neuron * this, int mthd, int attr, Msg * msg);
};

/* Prototypes */
Neuron * neuron_construct(int inputCnt, float slope);
void neuron_dispatcher(Neuron * this, int mthd, int attr, Msg * msg);
/* Required for multiple inheritance */
//void DoSuperMethod(int cls, Neuron * this, int mthd, int attr, Msg * msg);
/* Macros */
#define NeuronNew(inputCnt, slope)          ((neuron_construct)(inputCnt, slope))
#define NeuronDestroy(this)                 ((this->dispatcher)(this, DESTROY,          0,      NULL))
#define NeuronGet(this, attr, msg)          ((this->dispatcher)(this, GET,              attr,   msg))
#define NeuronSet(this, attr, msg)          ((this->dispatcher)(this, SET,              attr,   msg))
#define NeuronCalcOutput(this)              ((this->dispatcher)(this, CALC_OUTPUT,      0,      NULL))
#define NeuronCalcError(this, msg)          ((this->dispatcher)(this, CALC_ERROR,       0,      msg))
#define NeuronAdjustWeights(this, msg)      ((this->dispatcher)(this, ADJUST_WEIGHTS,   0,      msg))
#define NeuronAdjustThreshold(this, msg)    ((this->dispatcher)(this, ADJUST_THRESHOLD, 0,      msg))
#define NeuronPrint(this)                   ((this->dispatcher)(this, PRINT,            0,      NULL))


#endif	/* NEURON_H */
