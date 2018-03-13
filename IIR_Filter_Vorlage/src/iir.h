/* 
 * File:   iir.h
 * Author: noah
 *
 * Created on March 13, 2018, 10:09 AM
 */

#ifndef IIR_H
#define	IIR_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <dsp.h>
    
// number of biquads in IIR filter
#define iir_numSections      3
#define IIR_TAPS            15

    
fractional iir_coeff[IIR_TAPS] __attribute__ ((space(xmemory),far))={
    
        2592,        2592,       11199,           0,           0,
        2693,        2169,       19112,        2693,      -10285,
        7637,          74,       15498,        7637,      -14464 
};

fractional __attribute__ ((space(ymemory),far)) iir_state1[IIR_TAPS];
fractional __attribute__ ((space(ymemory),far)) iir_state2[IIR_TAPS];



#ifdef	__cplusplus
}
#endif

#endif	/* IIR_H */

