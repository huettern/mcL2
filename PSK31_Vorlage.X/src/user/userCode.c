/*******************************************************************************
 * Project:
 *  "Neuentwicklung DSP-Board fuer das Labor MikroComm"
 *
 * Version / Date:
 *  0.9 / August 2016
 *
 * Authors:
 *  Simon Gerber, Belinda Kneubuehler
 *
 * File Name:
 *  userCode.c
 *
 * Description:
 *  This file contains the user specific instructions that will be called at
 *  the correct location in the main loop.
 *******************************************************************************/

#include <dsp.h>
#include <p33EP256MU806.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
#include "userConfig.h"
#include "../globals.h"
#include "../tlv320aic.h"
#include "../IOconfig.h"
#include "../pwm.h"
#include "userCode.h"
#include "../SineTable.h"
#include "../PRNGenerator.h"
#include "../ShapingTable.h"
#include "FIR.h"


/**************************************************************************
 * User Global Definitions
 ***************************************************************************/

volatile int8_t mode = 3;


bool updateMode(void);
void psk31Mod(bool inBit, fractional* txLeft, fractional* txRight);
void psk31demod(fractional *inData, fractional *txLeft, fractional *txRight);
void firFilter(fractional *inData, fractional *outData);
size_t findMax(size_t size, double* data);

/**************************************************************************
 * User Public Functions
 ***************************************************************************/

/**
 * This user function is called once in the begining. It can be used to
 * initialise user buffers ect.
 */
void user_init()
{
    codec_setInput(LINE);
}

/**
 * This user function is called once in the main loop.
 */
void user_mainLoop()
{
    uint8_t r,g,b;
    
    if (updateMode()) {
//        led_setColour(LED2, 0, 0, mode & 0x01 ? 10 : 0);
//        led_setColour(LED1, 0, 0, (mode >> 1) & 0x01 ? 10 : 0);
        
        r = mode & 0x01 ? 100 : 0;
        g = mode & 0x01 ? 10 : 0;
        b = mode & 0x01 ? 12 : 0;
        led_setColour(LED2, r, g, b);
        r = (mode >> 1) & 0x01 ? 100 : 0;
        g = (mode >> 1) & 0x01 ? 10 : 0;
        b = (mode >> 1) & 0x01 ? 12 : 0;
        led_setColour(LED1, r, g, b);
    }

}

/**
 * This user function is called when new data is ready from the DMA.
 * @param sourceBuffer  is the buffer that contains the available input data
 * @param targetBuffer  is the buffer that the processed data has to be written into
 */
void user_processData(fractional *sourceBuffer, fractional *targetBuffer)
{
    uint16_t i, index1, index2;

    static fractional rxLeft[BUFFERLENGTH_DIV_2], rxRight[BUFFERLENGTH_DIV_2];
    static fractional txLeft[BUFFERLENGTH_DIV_2], txRight[BUFFERLENGTH_DIV_2];

    static unsigned int deltaPsiSin = 0;
    static unsigned int deltaPsiCos = 16384;
    static unsigned int f = 1500;
    bool inBit;
    
    const char* text = "The quick brown fox jumps over the lazy dog 123456789";
    static int chCtr, bitCtr;
    
    const int sendArr[4] = {0,0,0,1};
    static int arrCtr;
    
    // copy sourceBuffer to leftSignalBuffer and rightSignalBuffer
    for (index1 = 0, index2 = 0; index1 < BUFFERLENGTH_DIV_2; index1++) {
        rxLeft[index1] = sourceBuffer[index2++];
        rxRight[index1] = sourceBuffer[index2++];
    }

    switch (mode) {
        case 0:
            //talk through: just copy input (rx) into output (tx) 
            for (i = 0; i < BUFFERLENGTH_DIV_2; i++) {
                txLeft[i] = rxLeft[i];
            }
            for (i = 0; i < BUFFERLENGTH_DIV_2; i++) {
                txRight[i] = rxRight[i];
            }
            break;
        case 1:
            /* Generate 1500Hz sine */
            for (i = 0; i < BUFFERLENGTH_DIV_2; i++) {
                                  
                deltaPsiSin += 65536/CODEC_SAMPLE_RATE*f;
                deltaPsiCos += 65536/CODEC_SAMPLE_RATE*f;
                
                txLeft[i] = SINE_TABLE[(deltaPsiSin>>4)];
                txRight[i] = SINE_TABLE[(deltaPsiSin>>4)];
            }
            
            break;
        case 2:
            /* differential BPSK */
            
            // get new bit
//          inBit = PRNGenerator();
            inBit = sendArr[arrCtr++];
            arrCtr %= 4;
            PORTGbits.RG8 = inBit;
            
            
            psk31Mod(inBit, txLeft, txRight);
            break;
        case 3:
            /* FIR filter */
//            firFilter(rxLeft, txLeft);
            
            /* modulate */
            inBit = sendArr[arrCtr++];
            arrCtr %= 4;
            psk31Mod(inBit, txLeft, txRight);
//            psk31Mod(PRNGenerator(), txLeft, txRight);
            
            
            /* PSK31 demod */
//            psk31demod(rxLeft, txLeft);
            psk31demod(rxLeft, txLeft, txRight);
            break;
    }

    //copy left and right txBuffer into targetBuffer
    for (index1 = 0, index2 = 0; index1 < BUFFERLENGTH_DIV_2; index1++) {
        targetBuffer[index2++] = txLeft[index1];
        targetBuffer[index2++] = txRight[index1]; //txRight[index1];
    }
}

/**************************************************************************
 * User Private Functions
 ***************************************************************************/

/**
 * This function changes between four different modes whenever a switch is
 * pushed. The switches are used to count the mode up or down.
 * @return a bool that is true whenever the mode has changed (a switch has been
 * pushed)
 */
bool updateMode(void)
{
    bool modeChanged = false;
    if (switchStatesChanged) // Any changes in the state of the switches?
    {
        if (switchStatesChanged & SW1_MASK) // Has state of switch 1 changed?
        {
            switchStatesChanged &= ~SW1_MASK;
            if ((switchStates & SW1_MASK) == 0) // Is new state = 0?
            {
                mode++;
                if (mode > 3) {
                    mode = 0;
                }
            }
        }
        if (switchStatesChanged & SW2_MASK) // Has state of switch 2 changed?
        {
            switchStatesChanged &= ~SW2_MASK;
            if ((switchStates & SW2_MASK) == 0) // Is new state = 0?
            {
                mode--;
                if (mode < 0) {
                    mode = 3;
                }
            }
        }
        modeChanged = true;
    }
    return modeChanged;
}

/**
 * Fills the output buffers with psk31 modulated signal of the input bit
 * @param txLeft
 * @param txRight
 */
void psk31Mod(bool inBit, fractional* txLeft, fractional* txRight)
{
    uint16_t i;
    static bool currSign = true;
    static int ai = 0;
    static int aq = 0;
    long int aiCos = 0;
    long int aqSin = 0;
    
    static unsigned int f = 1500;
    
    static unsigned int deltaPsiSin = 0;
    static unsigned int deltaPsiCos = 16384;
    
    for (i = 0; i < BUFFERLENGTH_DIV_2; i++) 
    {
        if(!inBit) 
        {
            if(currSign) 
            {
                // High to low
                ai = ShapingTable01[i];
                aq = ShapingTable01[i];
            }
            else 
            {
                // low to high
                ai = ShapingTable02[i];
                aq = ShapingTable02[i];
            }
        }
        
        deltaPsiSin += 65536*f/CODEC_SAMPLE_RATE;
        deltaPsiCos += 65536*f/CODEC_SAMPLE_RATE;

        aiCos = (long int)ai * SINE_TABLE[(deltaPsiCos>>4)];
        aqSin = (long int)aq * SINE_TABLE[(deltaPsiSin>>4)];

        txLeft[i] = (fractional)((aiCos + aqSin)>>16);
        txRight[i] = txLeft[i];
    }
    if(!inBit) currSign = !currSign;
}

void firFilter(fractional *inData, fractional *outData)
{
    static FIRStruct flt;
    static bool init = false;
    
    if(!init)
    {
        FIRStructInit(&flt,NTAPS,coefficients,COEFFS_IN_DATA,delayBuffer);
        FIRDelayInit(&flt);
        init = true;
    }
    
    FIR(BUFFERLENGTH_DIV_2, outData, inData, &flt);
}

void psk31demod(fractional *inData, fractional *txLeft, fractional *txRight)
{
    int i;
    static FIRStruct fltDec11,fltDec12, fltDec21, fltDec22, fltMatch1, fltMatch2;
    static bool init = false;
    
    static unsigned int deltaPsiSin = 0;
    static unsigned int deltaPsiCos = 16384;
    
    // base band frequency
    static unsigned int f = 1500;
    
    // Memory
    static int rsin[BUFFERLENGTH_DIV_2], rcos[BUFFERLENGTH_DIV_2];
    static int rsinDec1[BUFFERLENGTH_DIV_2/4], rcosDec1[BUFFERLENGTH_DIV_2/4];
    static int rsinDec2[BUFFERLENGTH_DIV_2/16], rcosDec2[BUFFERLENGTH_DIV_2/16];
    static int rsinDec2flt[BUFFERLENGTH_DIV_2/16], rcosDec2flt[BUFFERLENGTH_DIV_2/16];
    
    // Avg power
    static double averagePower[16], power;
    
    // Sampling
    size_t sampleTime;
    
    // decision
    static double decX, decXold, decY, decYold, decReW;
    
    if(!init)
    {
        // Init 4 decimation filters
        FIRStructInit(&fltDec11,NTAPS_DEC,coefDec,COEFFS_IN_DATA,dlyBufDec11);
        FIRDelayInit(&fltDec11);
        FIRStructInit(&fltDec12,NTAPS_DEC,coefDec,COEFFS_IN_DATA,dlyBufDec12);
        FIRDelayInit(&fltDec12);
        FIRStructInit(&fltDec21,NTAPS_DEC,coefDec,COEFFS_IN_DATA,dlyBufDec21);
        FIRDelayInit(&fltDec21);
        FIRStructInit(&fltDec22,NTAPS_DEC,coefDec,COEFFS_IN_DATA,dlyBufDec22);
        FIRDelayInit(&fltDec22);
        // Init 2 matched filter
        FIRStructInit(&fltMatch1,NTAPS_MATCHED_FLT,coefMatchedFlt,COEFFS_IN_DATA,dlyBufMatchedFlt1);
        FIRDelayInit(&fltMatch1);
        FIRStructInit(&fltMatch2,NTAPS_MATCHED_FLT,coefMatchedFlt,COEFFS_IN_DATA,dlyBufMatchedFlt2);
        FIRDelayInit(&fltMatch2);
        init = true;
    }
    
    // Multiply input with sine and cosine
    for (i = 0; i < BUFFERLENGTH_DIV_2; i++) 
    {   
        // base band signals
        deltaPsiSin += 65536*f/CODEC_SAMPLE_RATE;
        deltaPsiCos += 65536*f/CODEC_SAMPLE_RATE;
        
        // Multiply input with base band signals
        rsin[i] = (fractional)(((long)inData[i] * (long)SINE_TABLE[(deltaPsiSin>>4)])>>15);
//        rsin[i] = inData[i];
        rcos[i] = (fractional)(((long)inData[i] * (long)SINE_TABLE[(deltaPsiCos>>4)])>>15);
//        rcos[i] = inData[i] * SINE_TABLE[(deltaPsiCos>>4)];
    }
        
    // Apply first decimation including filtering
    FIRDecimate(BUFFERLENGTH_DIV_2/4, rsinDec1, rsin, &fltDec11, 4);
    FIRDecimate(BUFFERLENGTH_DIV_2/4, rcosDec1, rcos, &fltDec21, 4);
    
//    // Apply second decimation including filtering
    FIRDecimate(BUFFERLENGTH_DIV_2/16, rsinDec2, rsinDec1, &fltDec12, 4);
    FIRDecimate(BUFFERLENGTH_DIV_2/16, rcosDec2, rcosDec1, &fltDec22, 4);

    /* Apply matched filter */
    FIR(BUFFERLENGTH_DIV_2/16, rsinDec2flt, rsinDec2, &fltMatch1);
    FIR(BUFFERLENGTH_DIV_2/16, rcosDec2flt, rcosDec2, &fltMatch2);
    
    /* Calculate average power and determine sampling time */
    for (i = 0; i < BUFFERLENGTH_DIV_2/16; i++) 
    {   
        power = (double)Fract2Float(rsinDec2flt[i])*(double)Fract2Float(rsinDec2flt[i]) + 
                (double)Fract2Float(rcosDec2flt[i])*(double)Fract2Float(rcosDec2flt[i]) ;
        // Simple IIR mean filter
        averagePower[i] = ((double)0.99)*averagePower[i] + ((double)0.01)*power;
    }
    sampleTime = findMax(BUFFERLENGTH_DIV_2/16, averagePower);
    
    /* decider */
    decXold = decX; decYold = decY;
    decX = rsinDec2flt[sampleTime]; decY = rcosDec2flt[sampleTime];
    decReW = decXold*decX + decYold*decY;
    if(decReW > 0)
    {
        // bd = 1
        PORTGbits.RG8 = 1;
    }
    else
    {
        // bd = 0
        PORTGbits.RG8 = 0;
        
    }
    
    // DEBUG: output decimated signal
    for (i = 0; i < BUFFERLENGTH_DIV_2; i++) 
    {   
        if(i < sampleTime*16)
            txLeft[i] = 0;
        else
            txLeft[i] = 30000;
        
//        txLeft[i] = Float2Fract(((((double)rcosDec2flt[i/16]*(double)rcosDec2flt[i/16])) + (((double)rsinDec2flt[i/16]*(double)rsinDec2flt[i/16])))/65535.0);
//        txLeft[i] = inData[i];
//        txLeft[i] =  rsinDec2[i/16];
//        txLeft[i] = rsinDec2flt[i/16];
        txRight[i] = rcosDec2flt[i/16];
//        txLeft[i] = rsinDec2flt[i/16];
        
    }
//    outData[sampleTime*16] = 100;
    
}

/**
 * Returns the array index with the largest value
 * @param size
 * @param data
 * @return 
 */
size_t findMax(size_t size, double* data)
{
    size_t idx = 0;
    double max = -DBL_MAX;
    int i;
    
    for(i = 0; i < size; i++)
    {
        if(data[i] > max)
        {
            max = data[i];
            idx = i;
        }
    }
    
    return idx;
}