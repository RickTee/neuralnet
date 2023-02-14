/*
 * File: ann.c
 * Program: 
 * Author: Richard G Townsend
 * Email: rtown@blueyonder.co.uk
 * Created on: March 16, 2015, 8:08 AM
 * Copyright (C) 2015  Richard G Townsend
 */
#include "ann.h"

/* Prototypes */
static void ann_destruct(Ann *this);
static void ann_get(Ann *this, int attr, Msg *msg);
static void ann_set(Ann *this, int attr, Msg *msg);
static void feed_forward(Ann *this, Msg *msg);
static void back_prop(Ann *this, Msg *msg);
static void calc_output_layer_errors(Ann *this, Msg *msg);
static void calc_hidden_layer_errors(Ann *this);
static void calc_input_layer_errors(Ann *this);
static void train_output_layer(Ann *this);
static void train_hidden_layer(Ann *this);
static void train_input_layer(Ann *this);
static void ann_print(Ann *this);
static float sqr(float x);

/* Globals */
static int inst_cnt;

/* Instantiate a new object */
Ann * ann_construct(int input_count, int hidden_count, int output_count, float LR, float ACL, float SL)
{
    int i;
    Ann *this = NULL;
    /* Allocate memory */
    this = (Ann *) malloc(sizeof (Ann));
    if (this == NULL)
    {
        printf("Ann: ::ann_construct Memory allocation failed");
        exit(1);
    }
    /* increment the instance count */
    inst_cnt++;
    /* Set initial values for data members */
    this->input_count = input_count;
    this->hidden_count = hidden_count;
    this->output_count = output_count;
    this->learning_rate = LR ? LR : 0.02;
    this->accel_rate = ACL ? ACL : 0.02;
    this->slope = SL ? SL : -3;
    this->net_error = 0;
    this->rms_error = 0;
    this->dispatcher = (void *) ann_dispatcher; /* Add the dispatcher pointer */

    /* Init network neurons */
    for (i = 0; i < this->input_count; i++)
    {
        this->input_layer[i] = NeuronNew(this->input_count, this->slope);
    }
    for (i = 0; i < this->hidden_count; i++)
    {
        this->hidden_layer[i] = NeuronNew(this->hidden_count, this->slope);
    }
    for (i = 0; i < this->output_count; i++)
    {
        this->output_layer[i] = NeuronNew(this->output_count, this->slope);
    }
    /* Return a pointer to the new object */
    return (this);
}

/* Do any clean up needed and free the memory */
static void ann_destruct(Ann *this)
{
    int i;
    /* Destruct super class/s first */
    //BaseClassDestroy(this->supercls);
    //OtherClassDestroy(this->supercls_1);
    /* Destroy network neurons */
    for (i = 0; i < this->input_count; i++)
    {
        NeuronDestroy(this->input_layer[i]);
    }
    for (i = 0; i < this->hidden_count; i++)
    {
        NeuronDestroy(this->hidden_layer[i]);
    }
    for (i = 0; i < this->output_count; i++)
    {
        NeuronDestroy(this->output_layer[i]);
    }
    free(this);
    inst_cnt--;
}

static void run(Ann *this, Msg *msg)
{
    int i;

    feed_forward(this, msg);

    if (msg->desired)
    {
        back_prop(this, msg);
    }
    for (i = 0; i < this->output_count; i++)
    {
        msg->output[i] = this->output_layer[i]->active;
    }
}

static void feed_forward(Ann *this, Msg *msg)
{
    int i;

    /* Process Input Layer */
    for (i = 0; i < this->input_count; i++)
    {
        NeuronSet(this->input_layer[i], NEURON_INPUT, msg);
        NeuronCalcOutput(this->input_layer[i]);
    }
    /* Process Hidden Layer */
    for (i = 0; i < this->hidden_count; i++)
    {
        NeuronSet(this->hidden_layer[i], NEURON_INPUT, msg);
        NeuronCalcOutput(this->hidden_layer[i]);
    }
    /* Process Output Layer */
    for (i = 0; i < this->output_count; i++)
    {
        NeuronSet(this->output_layer[i], NEURON_INPUT, msg);
        NeuronCalcOutput(this->output_layer[i]);
    }
}

static void back_prop(Ann *this, Msg *msg)
{
    /* Calc errors */
    calc_output_layer_errors(this, msg);
    calc_hidden_layer_errors(this);
    calc_input_layer_errors(this);
    /* Adjust weights */
    train_output_layer(this);
    train_hidden_layer(this);
    train_input_layer(this);
}

static void calc_output_layer_errors(Ann *this, Msg *msg)
{
    int i;

    this->net_error = 0;
    for (i = 0; i < this->output_count; i++)
    {
        NeuronCalcError(this->output_layer[i], msg);
        this->net_error += sqr(msg->error);
    }
}

static void calc_hidden_layer_errors(Ann *this)
{
    int i, j;
    Msg msg;

    for (i = 0; i < this->hidden_count; i++)
    {
        msg.sum = 0;
        for (j = 0; j < this->output_count; j++)
        {
            NeuronGet(this->output_layer[j], NEURON_ERROR, &msg);
            NeuronGet(this->output_layer[j], NEURON_WEIGHTS, &msg);
            msg.sum += msg.error * msg.weight; /* Fix me */
        }
        NeuronCalcError(this->hidden_layer[i], &msg);
        this->net_error += sqr(msg.sum);
    }
}

static void calc_input_layer_errors(Ann *this)
{
    int i, j;
    Msg msg;

    for (i = 0; i < this->input_count; i++)
    {
        msg.sum = 0;
        for (j = 0; j < this->hidden_count; j++)
        {
            msg.sum += this->hidden_layer[i]->error * this->hidden_layer[i]->weights[i];
        }
        NeuronCalcError(this->input_layer[i], &msg);
        this->net_error += sqr(msg.error);
    }
    this->rms_error = sqrt(this->net_error);
}

static void train_output_layer(Ann *this)
{
    Msg msg;
    int i;

    for(i=0;i<this->output_count;i++)
    {
        msg.LR = this->learning_rate;
        msg.ACL = this->accel_rate;
        NeuronAdjustWeights(this->output_layer[i], &msg);
        /* Calc and assign new threshold */
        NeuronAdjustThreshold(this->output_layer[i], &msg);
    }
}

static void train_hidden_layer(Ann *this)
{
    Msg msg;
    int i;

    for(i=0;i<this->hidden_count;i++)
    {
        msg.LR = this->learning_rate;
        msg.ACL = this->accel_rate;
        NeuronAdjustWeights(this->hidden_layer[i], &msg);
        /* Calc and assign new threshold */
        NeuronAdjustThreshold(this->hidden_layer[i], &msg);
    }
}

static void train_input_layer(Ann *this)
{
    Msg msg;
    int i;

    for(i=0;i<this->input_count;i++)
    {
        msg.LR = this->learning_rate;
        msg.ACL = this->accel_rate;
        NeuronAdjustWeights(this->input_layer[i], &msg);
        /* Calc and assign new threshold */
        NeuronAdjustThreshold(this->input_layer[i], &msg);
    }
}

static void ann_get(Ann *this, int attr, Msg *msg)
{
    int i;

    switch (attr)
    {
    case(ANN_INPUT_LAYER):
    {
        for (i = 0; i < this->input_count; i++)
        {
            NeuronGet(this->input_layer[i], NEURON_ACTIVE, msg);
        }
    }
        break;
    case(ANN_HIDDEN_LAYER):
    {
        for (i = 0; i < this->hidden_count; i++)
        {
            NeuronGet(this->hidden_layer[i], NEURON_ACTIVE, msg);
        }
    }
        break;
    case(ANN_OUTPUT_LAYER):
    {
        for (i = 0; i < this->output_count; i++)
        {
            NeuronGet(this->output_layer[i], NEURON_ACTIVE, msg);
        }
    }
        break;
        /* If this is a derived class and we don't understand the message send
         * it to the super class */
        //default: BaseClassGet(this, attr, msg);
        /* Else print an error message or set an error flag */
    default: printf("Ann:neuron_get: Unknown attribute %d\n", attr);
    }
}

static void ann_set(Ann *this, int attr, Msg *msg)
{
    switch (attr)
    {
    //case(ANN_INPUT_LAYER): this->input_layer = NULL;
        //break;
    //case(ANN_HIDDEN_LAYER): this->hidden_layer = NULL;
        //break;
        /* If this is a derived class and we don't understand the message send
         * it to the super class */
        //default: BaseClassSet(this, attr, msg);
        /* Else print an error message or set an error flag */
    default: printf("rclass: ::_set: Unknown attribute %d\n", attr);
    }
}

static void ann_print(Ann *this)
{
    /* If this is a derived class send the message to its supercls first */
    //BaseClassPrint(this, attr, msg);
    printf("\nInput = %d Hidden = %d output %d\n",
           this->input_count, this->hidden_count, this->output_count);
}

void ann_dispatcher(Ann *this, int mthd, int attr, Msg *msg)
{
    switch (mthd)
    {
    case(ANN_DESTROY): ann_destruct(this);
        break;
    case(ANN_GET): ann_get(this, attr, msg);
        break;
    case(ANN_SET): ann_set(this, attr, msg);
        break;
    case(ANN_PRINT): ann_print(this);
        break;
        /* If this is a derived class and we don't understand the method send
         * it to the super class */
        // Always pass the object as its current class not its super class
    //default: ((Ann *) this)->supercls->dispatcher(((Ann *) this)->supercls, mthd, attr, msg);
        /* If this class uses multiple inheritance */
    //default: DoSuperMethod(this->cls, this, mthd, attr, msg);
        /* If this is a base class print an error message or set an error flag */
    default: printf("Ann: ::_dispatcher: Unknown method %d\n", mthd);
    }
}

static float sqr(float x)
{
    return (x * x);
}
