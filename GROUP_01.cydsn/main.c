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
#include "stdio.h"

volatile uint8 ReadValue = 0; // set to zeto the flag to read the value ;

#define SLAVE_BUFFER_SIZE 7
#define CTRL_REG1 0
#define CTRL_REG2 1
#define WHO_AM_I 2
#define MSB1 3
#define LSB1 4
#define MSB2 5
#define LSB2 6

uint8 slaveBuffer[SLAVE_BUFFER_SIZE];

int32 sum_digit_LDR;
int32 sum_digit_TMP;
int32 average_digit_LDR;
int32 average_digit_TMP;
uint8_t num_samples = 0;

uint8_t average_samples; 
uint8_t status_bits;

char message[20] = {'\0'};

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    //Starting functions
    Timer_Start(); 
    ADC_DelSig_Start();
    isr_ADC_StartEx(Custom_ISR_ADC);
    AMux_Start();
    UART_Start();
    EZI2C_Start();
    
    ADC_DelSig_StartConvert(); // start ADC conversion

    slaveBuffer[WHO_AM_I] = SLAVE_WHOAMI_VALUE; // Set who am i register value
    slaveBuffer[CTRL_REG1] = SLAVE_MODE_OFF_CTRL_REG1; //set control reg 1 with status bits = 0 and number of samples = 5
    slaveBuffer[CTRL_REG2] = SLAVE_FREQUENCY_CTRL_REG2; //set control reg 2 with frequency at 50 Hz
    
    //set the databuffer header and tail
    DataBuffer[0] = 0xA0;
    DataBuffer[TRANSMIT_BUFFER_SIZE-1] = 0xC0;
    
    //set EZI2C buffer
    //SLAVE_BUFFER_SIZE - 4 is the third position, the boundary of the r/w cells 
    EZI2C_SetBuffer1(SLAVE_BUFFER_SIZE, SLAVE_BUFFER_SIZE - 4, slaveBuffer);

    
    for (;;)

    {
        
        //set the number of samples according to the bits 2-5 of the control register 1
        //in our case the number of samples to compute the average is 5 (101 in hexadecimal)
        average_samples = (slaveBuffer[CTRL_REG1] & 0x3C) >> 2; 

        //check only the status bits of the control register 1
        status_bits = slaveBuffer[CTRL_REG1] & 0x03;

        switch(status_bits){
            
            case SLAVE_BOTH_ON_CTRL_REG1:
                
                Pin_LED_Write(1); // Switch the led on 
                
                if (ReadValue == 1) {
                    
                    sum_digit_LDR = sum_digit_LDR + value_digit_LDR;
                    sum_digit_TMP = sum_digit_TMP + value_digit_TMP;
                    
                    num_samples++;
                    
                    if (num_samples == average_samples){

                        average_digit_LDR = sum_digit_LDR / average_samples;
                        average_digit_TMP = sum_digit_TMP / average_samples;

                        slaveBuffer[MSB1]= average_digit_LDR >> 8; // save in the 4th register the MSB of the average
                        slaveBuffer[LSB1]= average_digit_LDR & 0xFF; // save in the 5th register the LSB of the average
                        slaveBuffer[MSB2]= average_digit_TMP >> 8; // save in the 6th register the MSB of the average
                        slaveBuffer[LSB2]= average_digit_TMP & 0xFF; // save in the 7th register the LSB of the average
                        
                        DataBuffer[1]= slaveBuffer[MSB1]; 
                        DataBuffer[2]= slaveBuffer[LSB1]; 
                        DataBuffer[3]= slaveBuffer[MSB2];
                        DataBuffer[4]= slaveBuffer[LSB2];
                        
                        UART_PutArray(DataBuffer, TRANSMIT_BUFFER_SIZE);

                        sum_digit_LDR = 0;
                        sum_digit_TMP = 0;
                        num_samples = 0;
                    }
                    
                        
                    ReadValue = 0; // Reset the flag for reading value
                    
                }
                    
            
                break;
                
            case SLAVE_TMP_ON_CTRL_REG1:
                
                Pin_LED_Write(0); //switch the led off
                
                if (ReadValue == 1) {
                
                    sum_digit_TMP = sum_digit_TMP + value_digit_TMP;
                    num_samples++;
                    
                    if (num_samples == average_samples){

                        average_digit_TMP = sum_digit_TMP / average_samples;
                        
                        //sprintf(message, "Tmp: %ld\r\n", average_mv_TMP); //check on the uart if the sampling is ok
                        //UART_PutString(message);
                        
                        //slaveBuffer[MSB1]= 0x00; // save in the 4th register the MSB of the average
                        //slaveBuffer[LSB1]= 0x00; // save in the 5th register the LSB of the average
                        slaveBuffer[MSB2]= average_digit_TMP >> 8; // save in the 6th register the MSB of the average
                        slaveBuffer[LSB2]= average_digit_TMP & 0xFF; // save in the 7th register the LSB of the average
                        
                        DataBuffer[1]= 0x00; 
                        DataBuffer[2]= 0x00; 
                        DataBuffer[3]= slaveBuffer[MSB2]; 
                        DataBuffer[4]= slaveBuffer[LSB2];
                        
                        UART_PutArray(DataBuffer, TRANSMIT_BUFFER_SIZE);

                        sum_digit_TMP = 0;
                        num_samples = 0;
                    }
                    
                        
                    ReadValue = 0; // Reset the flag for reading value
                    
                }
                
                break;
                
            case SLAVE_LDR_ON_CTRL_REG1:
                
                Pin_LED_Write(0); //switch off the led 
                
                if (ReadValue == 1) {
                    
                    sum_digit_LDR = sum_digit_LDR + value_digit_LDR;
                    num_samples++;
                    
                    if (num_samples == average_samples){
                        
                        average_digit_LDR = sum_digit_LDR / average_samples;
                        
                        //sprintf(message, "Tmp: %ld\r\n", average_mv_TMP); //check on the uart if the sampling is ok
                        //UART_PutString(message);
                        
                        slaveBuffer[MSB1]= average_digit_LDR >> 8; // save in the 4th register the MSB of the average
                        slaveBuffer[LSB1]= average_digit_LDR & 0xFF; // save in the 5th register the LSB of the average
                        //slaveBuffer[MSB2]= 0x00; // save in the 6th register the MSB of the average
                        //slaveBuffer[LSB2]= 0x00; // save in the 7th register the LSB of the average
                        
                        DataBuffer[1]= slaveBuffer[MSB1]; 
                        DataBuffer[2]= slaveBuffer[LSB1]; 
                        DataBuffer[3]= 0x00; 
                        DataBuffer[4]= 0x00;
                        
                        UART_PutArray(DataBuffer, TRANSMIT_BUFFER_SIZE);

                        sum_digit_TMP = 0;
                        num_samples = 0;
                    }
                    
                        
                    ReadValue = 0; // Reset the flag for reading value
                    
                }
                
                break;
                
            case SLAVE_MODE_OFF_CTRL_REG1:
                
                Pin_LED_Write(0);
      
                DataBuffer[1]= 0x00; 
                DataBuffer[2]= 0x00; 
                DataBuffer[3]= 0x00; 
                DataBuffer[4]= 0x00;
                
                UART_PutArray(DataBuffer, TRANSMIT_BUFFER_SIZE);

                break;
        
            
        }
        
        
    }
}
    
/* [] END OF FILE */
