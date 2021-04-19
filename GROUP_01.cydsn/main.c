/* ========================================
 
Electronic Technologies and Biosensors Laboratory
Academic Year 2020/2021 - II Semester
Assignment 03
GROUP_01 

main source file
 
 * ========================================
*/
#include "project.h"
#include "InterruptRoutines.h"
#include "RegAddress.h"

volatile uint8 ReadValue;

#define SLAVE_BUFFER_SIZE 7
#define CTRL_REG1 0
#define CTRL_REG2 1
#define WHO_AM_I 2
#define LDR_CHANNEL 0
#define TMP_CHANNEL 1
#define AVERAGE_SAMPLES 5 //maybe we have to keep this value from the control reg2 


uint8 slaveBuffer[SLAVE_BUFFER_SIZE];

int32 value_digit_TMP;
int32 value_digit_LDR;
int32 value_mv_LDR;
int32 value_mv_TMP;
int32 sum_mv_LDR; //sum value in millivolt
int32 sum_mv_TMP;
int32 sum_digit_LDR;
int32 sum_digit_TMP;
int32 average_mv_LDR; //average value in millivolt
int32 average_mv_TMP;
int32 average_digit_LDR;
int32 average_digit_TMP;
uint8_t num_samples = 0;

char message[20] = {'\0'};

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    Timer_Start(); 
    ADC_DelSig_Start();
    isr_ADC_StartEx(Custom_ISR_ADC);
    AMux_Start();
    UART_Start();
    EZI2C_Start();
    
    ADC_DelSig_StartConvert();
    
    ReadValue = 0; // set to zeto the flag to read the value 
    
    slaveBuffer[WHO_AM_I] = SLAVE_WHOAMI_VALUE; // Set who am i register value
    slaveBuffer[CTRL_REG1] = SLAVE_NORMAL_MODE_OFF_CTRL_REG1; //set control reg 1 with status bits = 0
    
    //set EZI2C buffer
    //SLAVE_BUFFER_SIZE - 4 is the third position, the boundary of the r/w cells 
    EZI2C_SetBuffer1(SLAVE_BUFFER_SIZE, SLAVE_BUFFER_SIZE - 4, slaveBuffer);  
    
    for (;;)
    
    {
        
        
        
        if (ReadValue == 1)
        {
            // Read the value of the photodetector
            AMux_FastSelect(LDR_CHANNEL);
            value_digit_LDR = ADC_DelSig_Read32();
            
            if (value_digit_LDR < 0)     value_digit_LDR = 0;
            if (value_digit_LDR > 65535) value_digit_LDR = 65535;
            
            value_mv_LDR = ADC_DelSig_CountsTo_mVolts(value_digit_LDR);
            sum_mv_LDR = sum_mv_LDR + value_mv_LDR;
            sum_digit_LDR = sum_digit_LDR + value_digit_LDR;
            
            // Read the value of the temperature sensor
            AMux_FastSelect(TMP_CHANNEL);
            value_digit_TMP = ADC_DelSig_Read32();
            
            if (value_digit_TMP < 0)     value_digit_TMP = 0;
            if (value_digit_TMP > 65535) value_digit_TMP = 65535;
            
            value_mv_TMP = ADC_DelSig_CountsTo_mVolts(value_digit_TMP);
            sum_mv_TMP = sum_mv_TMP + value_mv_TMP;
            sum_digit_TMP = sum_digit_TMP + value_digit_TMP;
            
            num_samples ++;
            
            if (num_samples == AVERAGE_SAMPLES)
            {
                average_mv_LDR = sum_mv_LDR / AVERAGE_SAMPLES;
                average_digit_LDR = sum_digit_LDR / AVERAGE_SAMPLES;
                
                sprintf(message, "LDR: %ld\r\n", average_mv_LDR); //check on the uart if the sampling is ok
                UART_PutString(message);
                
                average_mv_TMP = sum_mv_TMP / AVERAGE_SAMPLES;
                average_digit_TMP = sum_digit_TMP / AVERAGE_SAMPLES;
                
                
                sprintf(message, "Tmp: %ld\r\n", average_mv_TMP); //check on the uart if the sampling is ok
                UART_PutString(message);
                
                sum_mv_LDR = 0;
                sum_mv_TMP = 0;
                num_samples = 0;
            }
            
                // Reset the flag for reading value
            ReadValue = 0;
            
        }
        
        if (slaveBuffer[CTRL_REG1] == SLAVE_BOTH_MODE_ON_CTRL_REG1){
            
            slaveBuffer[3]= average_digit_LDR >> 8; // save in the 4th register the MSB of the average
            slaveBuffer[4]= average_digit_LDR & 0xFF; // save in the 5th register the LSB of the average
            slaveBuffer[5]= average_digit_TMP >> 8; // save in the 6th register the MSB of the average
            slaveBuffer[6]= average_digit_TMP & 0xFF; // save in the 7th register the LSB of the average
            Pin_LED_Write(1); // turn the led on
        }
        
        else if (slaveBuffer[CTRL_REG1] == SLAVE_TMP_ON_CTRL_REG1){
            slaveBuffer[3] = 0x00; // save 0 because we have no sampling
            slaveBuffer[4] = 0x00; // save 0 because we have no sampling
            slaveBuffer[5]= average_digit_TMP >> 8; // save in the 6th register the MSB of the average
            slaveBuffer[6]= average_digit_TMP & 0xFF; // save in the 7th register the LSB of the average
            Pin_LED_Write(0);
        }
        
        else if (slaveBuffer[CTRL_REG1] == SLAVE_LDR_ON_CTRL_REG1) {
            slaveBuffer[3]= average_digit_LDR >> 8; // save in the 4th register the MSB of the average
            slaveBuffer[4]= average_digit_LDR & 0xFF; // save in the 5th register the LSB of the average
            slaveBuffer[5] = 0x00; // save 0 because we have no sampling
            slaveBuffer[6] = 0x00; // save 0 because we have no sampling
            Pin_LED_Write(0);
        }
        
        else if (slaveBuffer[CTRL_REG1] == SLAVE_LDR_ON_CTRL_REG1) {
            slaveBuffer[3] = 0x00; // save 0 because we have no sampling
            slaveBuffer[4] = 0x00; // save 0 because we have no sampling
            slaveBuffer[5] = 0x00; // save 0 because we have no sampling
            slaveBuffer[6] = 0x00; // save 0 because we have no sampling
            Pin_LED_Write(0);
        }
            
    }
}

    
/* [] END OF FILE */
