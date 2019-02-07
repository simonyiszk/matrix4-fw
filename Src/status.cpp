#include <status.hpp>
#include <window.hpp>

namespace{
    bool windows_swapped = true;
}

using namespace status;

windows::window& status::getWindow(window_from_outside w){
    const bool target = w xor windows_swapped;
    
    if(target == RIGHT)
        return windows::right_window;
    else
        return windows::left_window;
}


void status::swap_windows(){
    windows_swapped = not windows_swapped;
}


bool status::if_internal_animation_is_on = false;

