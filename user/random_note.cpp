/*
Copyright 2019 Tomoaki Itoh
This software is released under the MIT License, see LICENSE.txt.
//*/

#include "userosc.h"
#include "random_note.hpp"

static RandomNote s_r_note;

void OSC_INIT(uint32_t platform, uint32_t api)
{
  s_r_note = RandomNote();
}

void OSC_CYCLE(const user_osc_param_t * const params,
               int32_t *yn,
               const uint32_t frames)
{
  RandomNote::State &s = s_r_note.state;
  const RandomNote::Params &p = s_r_note.params;

  // Handle events.
  {
    const uint32_t flags = s.flags;
    s.flags = RandomNote::k_flags_none;

    if ( flags & RandomNote::k_flag_reset ) {
      s.pitch0 = params->pitch;
      s.sampling = 1;
    }

    s.shape_lfo = params->shape_lfo;
    s_r_note.updateWaves(flags);
  }

  // Temporaries.
  float phi0 = s.phi0;
  
  q31_t * __restrict y = (q31_t *)yn;
  const q31_t * y_e = y + frames;
  
  for (; y != y_e; ) {
    float sig = 0.5f < phi0 ? 1.0f : -1.0f;   
    *(y++) = f32_to_q31(sig);
    
    phi0 += s.w00;
    phi0 -= (uint32_t)phi0;
  }
  
  s.phi0 = phi0;
}

void OSC_NOTEON(const user_osc_param_t * const params)
{
  s_r_note.state.flags |= RandomNote::k_flag_reset;
}

void OSC_NOTEOFF(const user_osc_param_t * const params)
{
  (void)params;
}

void OSC_PARAM(uint16_t index, uint16_t value)
{ 
  RandomNote::Params &p = s_r_note.params;
  
  switch (index) {
  case k_user_osc_param_shape:
    p.shape = value;
    break;
  case k_user_osc_param_shiftshape:
    p.shiftshape = value;
    break;
  default:
    break;
  }
}

