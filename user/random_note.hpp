#pragma once
/*
Copyright 2019 Tomoaki Itoh
This software is released under the MIT License, see LICENSE.txt.
//*/

#include "userosc.h"

struct RandomNote {

  enum {
    k_flags_none    = 0,
    k_flag_reset    = 1<<1
  };

  struct Params {
    uint16_t  shape;
    uint16_t  shiftshape;

    Params(void) :
      shape(0),
      shiftshape(0)
    { }
  };
  
  struct State {
    uint16_t  pitch0;
    int32_t   shape_lfo;
    int16_t   sampling;
    int32_t   timer;
    float     phi0;
    float     w00;
    uint32_t  flags:8;

    State(void) :
      w00(440.f * k_samplerate_recipf),
      flags(k_flags_none)
    {
      reset();
    }

    inline void reset(void)
    {
      phi0 = 0;
      pitch0 = 0;
      sampling = 0;
      timer = 0;
    }
  };

  RandomNote(void) {
    init();
  }

  void init(void) {
    state = State();
    params = Params();
  }

  inline void updatePitch(void) {
    int32_t offset = (int32_t)(osc_white() * (params.shiftshape >> 4));
    int32_t note = (state.pitch0 >> 8) + offset;

    // osc_w0f_for_note で想定しているnoteでクリップ
    // see also: osc_api.h
    note = clipminmaxi32( 0, note, 151 );

    state.w00 = osc_w0f_for_note((uint16_t)note, 0);
  }

  inline void updateWaves(const uint16_t flags) {
    if ( state.sampling ) {
      updatePitch();
      state.sampling = 0;

      int32_t shape =
        (int32_t)params.shape + (state.shape_lfo >> (31 - 8));
      state.timer = clipmini32( 1, shape + 1 );
    }

    state.timer--;
    if ( state.timer <= 0 ) {
      state.sampling = 1;
    }
  }

  State   state;
  Params  params;
};
