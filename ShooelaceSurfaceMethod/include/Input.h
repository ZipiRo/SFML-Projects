bool mouse_button_pressed[5] = {};
bool IsMouseButtonDown(Mouse::Button button)
{
    if(!window.hasFocus()) return false;
    
    if(Mouse::isButtonPressed(button) && !mouse_button_pressed[int(button)])
    {
        
        mouse_button_pressed[int(button)] = true;
        return true;
    }
    else if(!Mouse::isButtonPressed(button))
    {
        mouse_button_pressed[int(button)] = false;
    }

    return false;
}