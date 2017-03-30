/*******************************************************************************
* File Name: main.c
*
* Version: 2.20
*
* Description:
*   This is a source code for example project of ADC single ended mode.
*
*   Variable resistor(pot) is connected to +ve input of ADC using the I/O pin.
*   P0.0. When voltage to positive terminal of ADC is 0, the output displayed
*   on the LCD pannel is 0x0000. As voltage on positive terminal goes on
*   increasing, the  converted value goes on increasing from 0x0000 and reaches
*   0xFFFF when voltage becomes 1.024V. Futher increase in voltage value,
*   doesn't cause any changes to values displayed in the LCD.
*
* Hardware Connections:
*  Connect analog input from Variable resistor to port P0[0] of DVK1 board.
*
********************************************************************************
* Copyright 2012-2015, Cypress Semiconductor Corporation. All rights reserved.
* This software is owned by Cypress Semiconductor Corporation and is protected
* by and subject to worldwide patent and copyright laws and treaties.
* Therefore, you may use this software only as provided in the license agreement
* accompanying the software package from which you obtained this software.
* CYPRESS AND ITS SUPPLIERS MAKE NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* WITH REGARD TO THIS SOFTWARE, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT,
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
*******************************************************************************/

#include <project.h>
#include "stdio.h"

/*******************************************************************************
* Function Name: main
********************************************************************************
*
* Summary:
*  main() performs following functions:
*  1: Initializes the LCD
*  2: Starts ADC
*  3: Starts ADC converstion.
*  4: Gets the converted result and displays it in LCD.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/

#define USBFS_DEVICE    (0u)

/* The buffer size is equal to the maximum packet size of the IN and OUT bulk
* endpoints.
*/
#define USBUART_BUFFER_SIZE (64u)
#define LINE_STR_LENGTH     (20u)

union
{
    int32* integer;
    uint8* byte;
} conv;

static uint16 X_Axis=0, Y_Axis=0;
static int16 X_Data, Y_Data;
static int32 Joystick_Data[16] = {0};
static unsigned char Buttons;


int main()
{
	int32 output;
		uint16 count;
	char buffer[USBUART_BUFFER_SIZE];
	
		CyGlobalIntEnable;

	/* Start USBFS operation with 5-V operation. */
	USBUART_Start(USBFS_DEVICE, USBUART_5V_OPERATION);
	while(!USBUART_bGetConfiguration());
	//USBUART_CDC_Init();

	/* Start the components */

	ADC_DelSig_1_Start();
    AMuxSeq_1_Start();

	/* Start the ADC conversion */
	ADC_DelSig_1_StartConvert();
    
    Joystick_Data[0] = 0x01;
    Joystick_Data[1] = 0x02;
    Joystick_Data[2] = 0x03;

	//USBUART_LoadInEP(3, (uint8*)Joystick_Data, 12);
    conv.integer = Joystick_Data;
	
    uint8 channel = 0;

	for(;;)
	{
		/* Wait for ACK before loading data. */
        //while (0u == USBUART_GetEPAckState(3))
        
        if (USBUART_GetEPState(3) == USBUART_IN_BUFFER_EMPTY){
        
        Joystick_Data[0] += 0x1000;
        //USBUART_LoadInEP(3, (uint8*){0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08}, 8);
        USBUART_LoadInEP(3, conv.byte, 64);
        }
        
        if(ADC_DelSig_1_IsEndConversion(ADC_DelSig_1_RETURN_STATUS))
		{
            
            output = ADC_DelSig_1_GetResult32();           
            output = (ADC_DelSig_1_CountsTo_uVolts(output)) / 8;
            Joystick_Data[AMuxSeq_1_GetChannel()] = output;
            ADC_DelSig_1_StopConvert();
            AMuxSeq_1_Next();
            ADC_DelSig_1_StartConvert();
            
			//USBUART_LoadInEP(3, (uint8 *)Joystick_Data, 3);
		}
	}
}


/* [] END OF FILE */
