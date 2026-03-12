
#ifndef MUX_MCP23017_H
#define MUX_MCP23017_H

#include <7Semi_MCP23017.h>

// Pins definition
constexpr uint8_t BUTTON_PIN = 0;
constexpr uint8_t LED_PIN    = 8;

// Instantiation
extern MCP23017_7Semi mcp;

// Inicialización del MCP23017
int init_MCP23017();

#endif