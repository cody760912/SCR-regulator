/******************************************************************************/
/* SETUP.C: Setup Functions                                                   */
/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2005-2007 Keil Software. All rights reserved.                */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/

#include "stm32f0xx.h"             /* STM32F10x Library Definitions      */
#include "work.h"


extern BIT16 ROArray[];

#define ADC1_DR_Address                0x40012440

__IO uint16_t RegularConvData_Tab;

void SetupADC (void) 
{
  	ADC_InitTypeDef     ADC_InitStruct;
  	DMA_InitTypeDef     DMA_InitStruct;
	GPIO_InitTypeDef    GPIO_InitStruct;
	  /* ADC1 DeInit */  
  	ADC_DeInit(ADC1);

	/* Enable  GPIOA clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  	/* ADC1 Periph clock enable */
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  	/* DMA1 clock enable */
  	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 , ENABLE);

  	/* Configure PA.01  as analog input */
  	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
  	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  	GPIO_Init(GPIOA, &GPIO_InitStruct);				// PC1,输入时不用设置速率

 
	/* DMA1 Channel1 Config */
	DMA_DeInit(DMA1_Channel1);
	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)ADC1_DR_Address;
	DMA_InitStruct.DMA_MemoryBaseAddr = (u32)&ROArray[20].word;//(uint32_t)&RegularConvData_Tab;
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStruct.DMA_BufferSize =4;
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStruct.DMA_Priority = DMA_Priority_High;
	DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &DMA_InitStruct);
	  
	/* DMA1 Channel1 enable */
	DMA_Cmd(DMA1_Channel1, ENABLE);
	
  // /* ADC DMA request in circular mode */
	ADC_DMARequestModeConfig(ADC1, ADC_DMAMode_Circular);
	
	/* Enable ADC_DMA */
	ADC_DMACmd(ADC1, ENABLE);  
	
	/* Initialize ADC structure */
	ADC_StructInit(&ADC_InitStruct);
	
	/* Configure the ADC1 in continous mode withe a resolutuion equal to 12 bits  */
	ADC_InitStruct.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStruct.ADC_ContinuousConvMode = ENABLE; 
	ADC_InitStruct.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStruct.ADC_ScanDirection = ADC_ScanDirection_Backward;
	ADC_Init(ADC1, &ADC_InitStruct); 
 
//   /* Convert the ADC1 temperature sensor  with 55.5 Cycles as sampling time */ 
//   ADC_ChannelConfig(ADC1, ADC_Channel_TempSensor , ADC_SampleTime_55_5Cycles);  
//   ADC_TempSensorCmd(ENABLE);
  
  	/* Convert the ADC1 Vref  with 55.5 Cycles as sampling time */ 
  	ADC_ChannelConfig(ADC1, ADC_Channel_0  , ADC_SampleTime_55_5Cycles); 
//   ADC_VrefintCmd(ENABLE);
  
  	/* ADC Calibration */
  	ADC_GetCalibrationFactor(ADC1);
	ADC_DMACmd(ADC1, ENABLE);
  	/* Enable ADC1 */
  	ADC_Cmd(ADC1, ENABLE);     
  
  	/* Wait the ADCEN falg */
  	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADEN)); 
  
  	/* ADC1 regular Software Start Conv */ 
  	ADC_StartOfConversion(ADC1);
}



