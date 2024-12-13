#ifndef _MODES_SMASH64_HPP
#define _MODES_SMASH64_HPP

#include "core/ControllerMode.hpp"
#include "core/socd.hpp"
#include "core/state.hpp"


//adding this code for testing
typedef struct {
    int c_stick_time = 5; 
} Smash64Options;



class Smash64 : public ControllerMode {
  public:
    Smash64(socd::SocdType socd_type, Smash64Options options = {});

  private:
    void UpdateDigitalOutputs(InputState &inputs, OutputState &outputs);
    void UpdateAnalogOutputs(InputState &inputs, OutputState &outputs);
    Smash64Options _options;
};

#endif
