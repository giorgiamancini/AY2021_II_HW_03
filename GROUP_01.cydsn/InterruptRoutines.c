/* ========================================
 
Electronic Technologies and Biosensors Laboratory
Academic Year 2020/2021 - II Semester
Assignment 03
GROUP_01 

interrupt routines source file
 
 * ========================================
*/

#include "InterruptRoutines.h"
#include "project.h"

#define LDR_CHANNEL 0
#define TMP_CHANNEL 1

extern volatile uint8_t ReadValue;


CY_ISR_PROTO(Custom_ISR_ADC){
    
    Timer_ReadStatusRegister();
    
    ReadValue = 1;  //Set the flag active to confirm the reading from the ADC
    
    AMux_FastSelect(LDR_CHANNEL); // Select the ldr sensor channel
    value_digit_LDR = ADC_DelSig_Read32(); // Read the value of the ldr sensor
    
    if (value_digit_LDR < 0)     value_digit_LDR = 0;
    if (value_digit_LDR > 65535) value_digit_LDR = 65535;
    
    //value_mv_LDR = ADC_DelSig_CountsTo_mVolts(value_digit_LDR); //conversion from digit to millivolt
   

    AMux_FastSelect(TMP_CHANNEL); // Select the temperature sensor channel
    value_digit_TMP = ADC_DelSig_Read32(); // Read the value of the temperature sensor
    
    if (value_digit_TMP < 0)     value_digit_TMP = 0;
    if (value_digit_TMP > 65535) value_digit_TMP = 65535;
    
    //value_mv_TMP = ADC_DelSig_CountsTo_mVolts(value_digit_TMP); //conversion from digit to millivolt
    
    
 
}




/* [] END OF FILE */
