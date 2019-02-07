#pragma once

#include <window.hpp>

namespace status{
    enum window_from_outside : bool{
        LEFT   = true,
        RIGHT  = false
    };
    
    extern bool if_internal_animation_is_on;
    
    inline void turn_internal_anim_on (){ if_internal_animation_is_on = true;  }
    inline void turn_internal_anim_off(){ if_internal_animation_is_on = false; }
    
    void swap_windows();
    windows::window& getWindow(window_from_outside);
}

using status::LEFT;
using status::RIGHT;
