// headerfile for
// Function generator Sin
// made by and (C) Helmut Weber
//
// mods MHY: 1/1/2021
// average error from target HZ is 2.0 Hz. Worst case is 8Hz (half of step size)
 
#include "driver/dac.h"
 
void dac_cosine_enable(dac_channel_t channel);
void dac_frequency_set(int clk_8m_div, int frequency_step);
void dac_scale_set(dac_channel_t channel, int scale);
void dac_offset_set(dac_channel_t channel, int offset);
void dac_invert_set(dac_channel_t channel, int invert);
float dac_CW_set(double f_target,int sinePrecision);
