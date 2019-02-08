#include "error_handler.hpp"

void _Error_Handler ( const char* file, int line ){
  (void) file;
  (void) line;
  
  //TODO toogle LEDS to right position
  
  while(1);
}
