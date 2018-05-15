/* 
 * File:   PRNGenerator.c
 * Author: noah
 *
 * Created on May 15, 2018, 9:53 AM
 */

#include <dsp.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "../src/PRNGenerator.h"


/*
 * Generates a pseudo random bit sequence with 4 bit latch
 * and polynome 
 */
bool PRNGenerator() {
    static uint8_t reg = 0x01;
    
    uint8_t in = ((reg&0x01)>>0) ^ ((reg&0x02)>>1);
    reg = ((reg>>1) & 0x07) | (in<<3);
    
    return reg & 0x01;
}