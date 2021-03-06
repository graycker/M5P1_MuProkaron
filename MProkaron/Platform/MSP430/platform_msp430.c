/******************************************************************************
Filename    : platform_msp430.c
Author      : pry
Date        : 25/02/2018
Licence     : LGPL v3+; see COPYING for details.
Description : The platform specific file for MSP430.
******************************************************************************/

/* Includes ******************************************************************/
#define __HDR_DEFS__
#include "Platform/MSP430/platform_msp430.h"
#include "Kernel/kernel.h"
#undef __HDR_DEFS__

#define __HDR_STRUCTS__
#include "Platform/MSP430/platform_msp430.h"
#include "Kernel/kernel.h"
#undef __HDR_STRUCTS__

/* Private include */
#include "Platform/MSP430/platform_msp430.h"

#define __HDR_PUBLIC_MEMBERS__
#include "Kernel/kernel.h"
#undef __HDR_PUBLIC_MEMBERS__
/* End Includes **************************************************************/

/* Begin Function:RMP_MSB_Get *************************************************
Description : Get the MSB of the word.
Input       : ptr_t Val - The value.
Output      : None.
Return      : ptr_t - The MSB position.
******************************************************************************/
/* 2*i and 2*i+1 will correspond to the same slot in this table */
const u8 RMP_MSB_Tbl[128]=
{
    /* 0-1 */
    0x00,
    /* 2-3 */
    0x01,
    /* 4-7 */
    0x02,0x02,
    /* 8-15 */
    0x03,0x03,0x03,0x03,
    /* 16-31 */
    0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,
    /* 32-63 */
    0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,
    /* 64-127 */
    0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,
    0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,
    /* 128-255 */
    0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,
    0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,
    0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,
    0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07
};

ptr_t RMP_MSB_Get(ptr_t Val)
{
    /* Something at high bits */
    if((Val&0xFF00)!=0)
        return RMP_MSB_Tbl[Val>>9]+8;
    /* Something at low bits */
    else if((Val&0xFF)!=0)
        return RMP_MSB_Tbl[Val>>1];
    /* Nothing anywhere */
    return 0xFFFF;
}
/* End Function:RMP_MSB_Get **************************************************/

/* Begin Function:_RMP_Clear_Soft_Flag ****************************************
Description : Clear the software interrupt flag in the interrupt controller.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void _RMP_Clear_Soft_Flag(void)
{
    RMP_MSP430_CLEAR_SOFT_FLAG();
}
/* End Function:_RMP_Clear_Soft_Flag *****************************************/

/* Begin Function:_RMP_Clear_Timer_Flag ***************************************
Description : Clear the timer interrupt flag in the interrupt controller.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void _RMP_Clear_Timer_Flag(void)
{
    RMP_MSP430_CLEAR_TIMER_FLAG();
}
/* End Function:_RMP_Clear_Timer_Flag ****************************************/

/* Begin Function:_RMP_Yield **************************************************
Description : Trigger a yield to another thread. This will always trigger the
              timer 0 compare vector on MSP430.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void _RMP_Yield(void)
{
    /* Manully pend the interrupt */
    RMP_MSP430_PEND_SOFT_FLAG();
}
/* End Function:_RMP_Yield ***************************************************/

/* Begin Function:_RMP_Stack_Init *********************************************
Description : Initiate the process stack when trying to start a process. Never
              call this function in user application.
Input       : ptr_t Entry - The entry of the thread.
              ptr_t Stack - The stack address of the thread.
              ptr_t Arg - The argument to pass to the thread.
Output      : None.
Return      : None.
******************************************************************************/
void _RMP_Stack_Init(ptr_t Entry, ptr_t Stack, ptr_t Arg)
{
    ptr_t* Stack_Ptr=(ptr_t*)Stack;

    /* General purpose registers */
#if(RMP_MSP430_INIT_EXTRA==RMP_TRUE)
    Stack_Ptr[0]=0x0404;                                    /* R4 */
    Stack_Ptr[1]=0x0505;                                    /* R5 */
    Stack_Ptr[2]=0x0606;                                    /* R6 */
    Stack_Ptr[3]=0x0707;                                    /* R7 */
    Stack_Ptr[4]=0x0808;                                    /* R8 */
    Stack_Ptr[5]=0x0909;                                    /* R9 */
    Stack_Ptr[6]=0x1010;                                    /* R10 */
    Stack_Ptr[7]=0x1111;                                    /* R11 */
#endif
    Stack_Ptr[8]=Arg;                                       /* R12 */
#if(RMP_MSP430_INIT_EXTRA==RMP_TRUE)
    Stack_Ptr[9]=0x1313;                                    /* R13 */
    Stack_Ptr[10]=0x1414;                                   /* R14 */
    Stack_Ptr[11]=0x1515;                                   /* R15 */
#endif

#if(RMP_MSP430_X==RMP_TRUE)
    ((u16*)Stack_Ptr)[24]=0;
    ((u16*)Stack_Ptr)[25]=((Entry>>4)&0xF000)|RMP_MSP430_SR_GIE;
    ((u16*)Stack_Ptr)[26]=Entry&0xFFFF;
    //RMP_MSP430X_PCSR(Entry,RMP_MSP430_SR_GIE);/* Status & PC */
#else
    Stack_Ptr[12]=RMP_MSP430_SR_GIE;                        /* Status */
    Stack_Ptr[13]=Entry;                                    /* PC */
#endif
}
/* End Function:_RMP_Stack_Init **********************************************/

/* Begin Function:_RMP_Low_Level_Init *****************************************
Description : Initialize the low level hardware of the system.
Input       : None
Output      : None.
Return      : None.
******************************************************************************/
void _RMP_Low_Level_Init(void)
{
    RMP_MSP430_LOW_LEVEL_INIT();

    RMP_Disable_Int();
}
/* End Function:_RMP_Low_Level_Init ******************************************/

/* Begin Function:_RMP_Plat_Hook **********************************************
Description : Platform-specific hook for system initialization.
Input       : None
Output      : None.
Return      : None.
******************************************************************************/
void _RMP_Plat_Hook(void)
{
    RMP_Enable_Int();
}
/* End Function:_RMP_Plat_Hook ***********************************************/

/* Begin Function:RMP_Putchar *************************************************
Description : Print a character to the debug console.
Input       : char Char - The character to print.
Output      : None.
Return      : None.
******************************************************************************/
void RMP_Putchar(char Char)
{
    RMP_MSP430_PUTCHAR(Char);
}
/* End Function:RMP_Putchar **************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
