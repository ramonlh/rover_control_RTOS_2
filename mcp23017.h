
//#include "MCP23017.h"

// Pins definition
const uint8_t BUTTON_PIN = 0;
const uint8_t LED_PIN    = 8;

// Instantiation
using namespace MCP23017;
MCP23017_IO <> io_mux(I2CAddress::A0_0_A1_0_A2_0, Wire);

int init_MCP23017()
{
  // Check if the MCP23017 is connected
  if (!io_mux.isConnected()) {
    return 1;
    }
  else
    {
    // Initialize MCP23017
    Status status = io_mux.init();

    // Set pin modes
    status = io_mux.pinMode(BUTTON_PIN, Mode::Input, LED_PIN, Mode::Output);
    if (status != Status::Ok) 
      { 
      return 2;
      }
    status = io_mux.pinPullUp(BUTTON_PIN, PullUp::Enable);
    if (status != Status::Ok)
      { 
      return 3;
      }
    }
  return 0;
}
