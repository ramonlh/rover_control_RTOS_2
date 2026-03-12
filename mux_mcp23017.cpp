
#include "mux_mcp23017.h"

MCP23017_7Semi mcp(0x20);

int init_MCP23017()
{
  // Iniciar MCP23017 en dirección I2C 0x20
  if (!mcp.begin(SDA, SCL)) {
    return 1;  // No encontrado
  }

  // Configurar pines del multiplexor
  mcp.pinMode((pins)BUTTON_PIN, INPUT);
  mcp.pinMode((pins)LED_PIN, OUTPUT);
  mcp.pinMode((pins)BUTTON_PIN, INPUT_PULLUP);

  return 0;  // OK
}