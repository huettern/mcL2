#define NTAPS 65

////////////////////////////////////////
// FIR Coeffecient Buffer
//fractional coefficients[NTAPS] __attribute__ ((space(xmemory),far)) = {
//        2,    -10,    -14,     -4,     19,     35,     18,    -32,    -73,
//      -51,     42,    132,    114,    -37,   -211,   -224,     -1,    307,
//      399,     99,   -413,   -668,   -305,    516,   1096,    726,   -603,
//    -1913,  -1767,    661,   4767,   8650,  10241,   8650,   4767,    661,
//    -1767,  -1913,   -603,    726,   1096,    516,   -305,   -668,   -413,
//       99,    399,    307,     -1,   -224,   -211,    -37,    114,    132,
//       42,    -51,    -73,    -32,     18,     35,     19,     -4,    -14,
//      -10,      2
//};

//FIR-LP fs=8000 fc=1000 NTAPS=65
fractional coefficients[NTAPS] __attribute__ ((space(xmemory),far)) = {
-1,-12,-19,-16,0,21,37,32,-1,-48,-81,-69,0,94,156,128,-1,-173,-282,-230,0,304,497,408,-1,-566,-962,-837,0,1444,3099,4412,4911,4412,3099,1444,0,-837,-962,-566,-1,408,497,304,0,-230,-282,-173,-1,128,156,94,0,-69,-81,-48,-1,32,37,21,0,-16,-19,-12,-1
};

//FIR-LP fs=8000 fc=1000 NTAPS=33
// Decimation filter
#define NTAPS_DEC 33
fractional coefDec[NTAPS_DEC] __attribute__ ((space(xmemory),far)) = {
40,87,128,131,67,-79,-286,-493,-605,-517,-152,512,1416,2421,3343,3991,4224,3991,3343,2421,1416,512,-152,-517,-605,-493,-286,-79,67,131,128,87,40
};

// FIR Delay Buffer
fractional delayBuffer[NTAPS] __attribute__ ((space(ymemory),far)); 

// FIR Delay Buffer
fractional dlyBufDec11[NTAPS_DEC] __attribute__ ((space(ymemory),far)); 
fractional dlyBufDec12[NTAPS_DEC] __attribute__ ((space(ymemory),far)); 
fractional dlyBufDec21[NTAPS_DEC] __attribute__ ((space(ymemory),far)); 
fractional dlyBufDec22[NTAPS_DEC] __attribute__ ((space(ymemory),far)); 


////////////////////////////////////////





