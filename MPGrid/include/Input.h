#include <unordered_map>

class Input
{
private:
    Input() {}

    struct KeyState
    {
        bool pressed = false;
        bool was_pressed = false;
    };

    struct ButtonState
    {
        bool pressed = false;
        bool was_pressed = false;
    };

    std::unordered_map<Keyboard::Key, KeyState> Keyboard;
    std::unordered_map<Mouse::Button, ButtonState> Mouse;
    float mouse_wheel_delta;

public:
    static void BeginFrame()
    {
        auto &instance = GetInstance();

        for(auto &[key, state] : instance.Keyboard)
            state.was_pressed = state.pressed;
    
        for(auto &[button, state] : instance.Mouse)
            state.was_pressed = state.pressed;

        instance.mouse_wheel_delta = 0.0f;
    }

    static void FetchInputData(Event &event)
    {
        auto &instance = GetInstance();
    
        if(const auto &key = event.getIf<Event::KeyPressed>())
            instance.Keyboard[key->code].pressed = true;

        if(const auto& key = event.getIf<Event::KeyReleased>())
            instance.Keyboard[key->code].pressed = false;

        if(const auto &button = event.getIf<Event::MouseButtonPressed>())
            instance.Mouse[button->button].pressed = true;

        if(const auto &button = event.getIf<Event::MouseButtonReleased>())
            instance.Mouse[button->button].pressed = false;

        if(const auto &wheel = event.getIf<Event::MouseWheelScrolled>())
            instance.mouse_wheel_delta = wheel->delta; 
    }

    static bool IsKeyDown(Keyboard::Key key)
    {
        auto &instance = GetInstance();
        auto it = instance.Keyboard.find(key);

        if(it != instance.Keyboard.end())
        {
            const auto &state = it->second;
            return state.pressed && !state.was_pressed;
        }

        return false;
    }
    
    static bool IsKeyUp(Keyboard::Key key)
    {
        auto &instance = GetInstance();
        auto it = instance.Keyboard.find(key);

        if(it != instance.Keyboard.end())
        {
            const auto &state = it->second;
            return !state.pressed && state.was_pressed;
        }
        
        return false;
    }
    
    static bool IsKey(Keyboard::Key key)
    {
        auto &instance = GetInstance();
        auto it = instance.Keyboard.find(key);

        if(it != instance.Keyboard.end())
        {
            const auto &state = it->second;
            return state.pressed && state.was_pressed;
        }
        
        return false;
    }

    static bool IsMouseButtonDown(Mouse::Button button)
    {
        auto &instance = GetInstance();
        auto it = instance.Mouse.find(button);

        if(it != instance.Mouse.end())
        {
            const auto &state = it->second;
            return state.pressed && !state.was_pressed;
        }
        
        return false;
    }

    static bool IsMouseButtonUp(Mouse::Button button)
    {
        auto &instance = GetInstance();
        auto it = instance.Mouse.find(button);

        if(it != instance.Mouse.end())
        {
            const auto &state = it->second;
            return !state.pressed && state.was_pressed;
        }
        
        return false;
    }

    static bool IsMouseButton(Mouse::Button button)
    {
        auto &instance = GetInstance();
        auto it = instance.Mouse.find(button);

        if(it != instance.Mouse.end())
        {
            const auto &state = it->second;
            return state.pressed && state.was_pressed;
        }
        
        return false;
    }

    static float MouseWheelDelta()
    {
        auto &instance = GetInstance();
        return instance.mouse_wheel_delta;
    }

    static Input &GetInstance()
    {
        static Input instance;
        return instance;
    }
};