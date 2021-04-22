/* ========================================
 
Electronic Technologies and Biosensors Laboratory
Academic Year 2020/2021 - II Semester
Assignment 03
GROUP_01 

interrupt routines header file
 
 * ========================================
*/

#ifndef __INTERRUPT_ROUTINES_H
    #define __INTERRUPT_ROUTINES_H
    
    #include "cytypes.h"
    #include "stdio.h"
    
    #define BYTE_TO_SEND 4
    #define TRANSMIT_BUFFER_SIZE 1+BYTE_TO_SEND+1 // number of bytes to transmit on bridge control panel
    
    uint8 DataBuffer[TRANSMIT_BUFFER_SIZE];
    
    int32 value_digit_TMP;
    int32 value_digit_LDR;
    //int32 value_mv_LDR;
    //int32 value_mv_TMP;

    CY_ISR_PROTO(Custom_ISR_ADC);
    
    
#endif

/* [] END OF FILE */
