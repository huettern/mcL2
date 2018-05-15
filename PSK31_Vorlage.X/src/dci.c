/*******************************************************************************
 * Project:
 *  "Neuentwicklung DSP-Board fuer das Labor MikroComm"
 * 
 * Version / Date:
 *  0.9 / August 2016
 * 
 * Authors:
 *  Simon Gerber, Belinda Kneubuehler
 *  Hochschule fuer Technik, FHNW, Windisch
 * 
 * File Name:
 *  dci.c
 * 
 * Description:
 *  This file initialises the DCI interface and configures it in I2S mode.
*******************************************************************************/

#include <p33EP256MU806.h>
#include "dci.h"
#include "constants.h"

/**************************************************************************
 * Initiation
 ***************************************************************************/

void dci_init()
{
    DCICON1 = 0;
    DCICON1bits.CSCKE = 1;      // sampled on serial clock rising edge
    DCICON1bits.COFSM = 0b01;   // 01 = I2S Frame Sync mode
    DCICON1bits.DJST = 0;       /* Data transmission/reception begins one 
                                 * serial clock cycle after frame
                                 * synchronisation pulse*/
    DCICON1bits.UNFM = 0;       // underflow -> 0
    DCICON1bits.CSDOM = 1;      // tristate in idle
    DCICON1bits.COFSD = 1;      // framesync input, slave
    DCICON1bits.CSCKD = 1;      // clk input, slave  
    DCICON2 = 0;                // DSP is slave !
    DCICON2bits.COFSG = 0;      // 0000 = Data frame has 1 word
    DCICON2bits.WS = 0b1111;    // 1111 = Data word size is 16 bits
    
    TSCON = 0x0001;             // Transmit slot 0
    RSCON = 0x0001;             // Receive slot 0

    TXBUF0 = 0;                 // Clear the TX buffer

    IEC3bits.DCIIE = 0;         // Disable DCI interrupt, we use DMA
    DCICON1bits.DCIEN = 1;      // Enable DCI
} 