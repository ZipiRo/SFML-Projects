static bool mouse_button_pressed[5] = {};
static bool keyboard_button_pressed[255] = {};

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

bool IsMouseButtonPressed(Mouse::Button button)
{
    if(!window.hasFocus()) return false;
    
    if(Mouse::isButtonPressed(button))
        return true;

    return false;
}

bool IsKeyboardButtonDown(Keyboard::Key key)
{
    if(!window.hasFocus()) return false;
    
    if(Keyboard::isKeyPressed(key) && !mouse_button_pressed[int(key)])
    {
        mouse_button_pressed[int(key)] = true;
        return true;
    }
    else if(!Keyboard::isKeyPressed(key))
    {
        mouse_button_pressed[int(key)] = false;
    }

    return false;
}