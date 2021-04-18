/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

#include "InterruptRoutines.h"
#include "project.h"

extern volatile uint8_t ReadValue;

CY_ISR_PROTO(Custom_ISR_ADC){
    
    Timer_ReadStatusRegister();
    
    ReadValue = 1;  //Set the flag active to read the value
 
}




/* [] END OF FILE */
