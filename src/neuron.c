/*
 * File: neuron.
 * Program:
 * Author: Richard G Townsend
 * Email: ricktee@virginmedia.com
 * Created on: September 4, 2014, 9:30 AM
 */
#include "neuron.h"

/* Prototypes */
static void neuron_destruct(Neuron *this);
static void neuron_calc_output(Neuron *this);
static void neuron_calc_error(Neuron *this, Msg *msg);
static void neuron_adjust_weights(Neuron *this, Msg *msg);
static void neuron_adjust_threshold(Neuron *this, Msg *msg);
static void neuron_get(Neuron *this, int attr, Msg *msg);
static void neuron_set(Neuron *this, int attr, Msg *msg);
static void neuron_print(Neuron *this);
static float sigmoid(Neuron *this, float x);
static float rnd(float max);

/* Globals */
static int inst_cnt;

/* Instantiate a new object */
Neuron * neuron_construct(int inputCnt, float slope)
{
    int i;
    Neuron *this = NULL;
    /* Allocate memory */
    this = (Neuron *) malloc(sizeof (Neuron));
    if (this == NULL)
    {
        printf("rclass: ::neuron_construct Memory allocation failed");
        exit(1);
    }
    /* increment the instance count */
    inst_cnt++;
    /* Set initial values for data members */
    this->cls = NEURON;
    this->inputCnt = inputCnt;
    this->slope = slope;
    this->threshold = rnd(1);
    this->tchange = 0;
    this->desired = 0;
    this->error = 0;
    this->active = 0;
    this->output = 0;
    this->stateFlag = 0;
    this->id = inst_cnt;
    /* Add the dispatcher pointer */
    this->dispatcher = (void *) neuron_dispatcher;
    for (i = 0; i < NEURON_SIZE; i++)
    {
        this->weights[i] = rnd(0.5);
        this->inputs[i] = 0;
        this->wchange[i] = 0;
    }

    /* Return a pointer to the new object */
    return (this);
}

/* Do any clean up needed and free the memory */
static void neuron_destruct(Neuron *this)
{
    /* Destruct super class/s first */
    free(this);
    inst_cnt--;
}

static void neuron_get(Neuron *this, int attr, Msg *msg)
{
    switch (attr)
    {
    case(NEURON_INPUT): msg->inputs = this->inputs;
        break;
        //case(NEURON_OUTPUT): msg->output = this->output;
        //break;
    /* Else print an error message or set an error flag */
    default: printf("rclass: Vehicle::neuron_get: Unknown attribute %d\n", attr);
    }
}

static void neuron_set(Neuron *this, int attr, Msg *msg)
{
    int i;
    
    switch (attr)
    {
    case(NEURON_INPUT):
    {
        for (i = 0; i < this->inputCnt; i++)
        {
            this->inputs[i] = msg->inputs[i] ? msg->inputs[i] : rnd(0.1);
        }
        break;
    }
    case(NEURON_DESIRED): this->desired = msg->desired;
        break;
    case(NEURON_SLOPE): this->slope = msg->slope;
        break;
    case(NEURON_THRESHOLD): this->threshold = msg->threshold;
        break;
    /* Else print an error message or set an error flag */
    default: printf("rclass: ::_set: Unknown attribute %d\n", attr);
    }
}

static void neuron_calc_output(Neuron *this)
{
    int i;
    float sum;

    /* weights * inputCnt - threshold */
    sum = 0;
    for (i = 0; i < this->inputCnt; i++)
    {
        sum += this->weights[i] * this->inputs[i];
    }
    this->output = sum - this->threshold;
    this->active = sigmoid(this, this->output);
}

static void neuron_calc_error(Neuron *this, Msg *msg)
{
    /* Output layer error */
    if (msg->desired)
    {
        this->error = (msg->desired - this->active) * this->active * (1 - this->active);
        this->desired = msg->desired;
    }
        /* Hidden and input layer errors */
    else
    {
        this->error = this->active * (1 - this->active) * msg->sum;
    }
    msg->error = this->error;
}

static void neuron_adjust_weights(Neuron *this, Msg *msg)
{
    int i;

    for (i = 0; i < this->inputCnt; i++)
    {
        /* LR * error * input + $ACL * lastChange */
        this->wchange[i] = msg->LR * this->error * this->inputs[i] + msg->ACL * this->wchange[i];
        this->weights[i] += this->wchange[i];
    }
}

static void neuron_adjust_threshold(Neuron *this, Msg *msg)
{
    /* LR * error * 1 + ACL * lastChange */
    this->tchange = msg->LR * this->error * 1 + msg->ACL * this->tchange;
    this->threshold -= this->tchange;
}

static float sigmoid(Neuron *this, float x)
{
    int y;
    /* limits output range 0 to +1 */
    if (x > 45)
    {
        return 1;
    }
        /* Avoids floating point overflow */
    else if (x < -45)
    {
        return 0;
    }
    else
    {
        /* y = 2 / (1 + log(-2 * x)) - 1; -1 to 1 */
        y = 1 / (1 + log(this->slope * x)); /* 0 to 1 */
        return y;
    }
}

static void neuron_print(Neuron *this)
{
    int i;
    /* If this is a derived class send the message to its supercls first */
    //BaseClassPrint(this, attr, msg);
    printf("id = %d Slope = %f Active = %f Desired = %f Error = %f InputCnt = %d Output = %f\n stateFlag = %d tchange = %f Threshold = %f\n",
           this->id, this->slope, this->active, this->desired, this->error, this->inputCnt,
           this->output, this->stateFlag, this->tchange, this->threshold);

    for (i = 0; i < this->inputCnt; i++)
    {
        printf("inputs %d, weights %f, wchange %f\n", this->inputs[i], this->weights[i], this->wchange[i]);
    }
}

void neuron_dispatcher(Neuron *this, int mthd, int attr, Msg *msg)
{
    switch (mthd)
    {
    case(DESTROY): neuron_destruct(this);
        break;
    case(GET): neuron_get(this, attr, msg);
        break;
    case(SET): neuron_set(this, attr, msg);
        break;
    case(PRINT): neuron_print(this);
        break;
    case(CALC_OUTPUT): neuron_calc_output(this);
        break;
    case(CALC_ERROR): neuron_calc_error(this, msg);
        break;
    case(ADJUST_WEIGHTS): neuron_adjust_weights(this, msg);
        break;
    case(ADJUST_THRESHOLD): neuron_adjust_threshold(this, msg);
        break;
        /* If this is a base class print an error message or set an error flag */
    default: printf("rclass: ::neuron_dispatcher: Unknown method %d\n", mthd);
    }
}

/* return a random number between 0 and limit inclusive. */
static float rnd(float max)
{
    float divisor = RAND_MAX / (max + 1);
    float retval;

    do
    {
        retval = rand() / divisor;
    }
    while (retval > max);

    return retval;
}
