#ifndef __AD_H
#define __AD_H

void AD_Init(void);
uint16_t AD_GetValue(uint8_t ADC_Channel);
float AD_Average(uint8_t channel,uint8_t samples);

#endif
