class Interface
{
private:
    float menubar_height = 18;
    float sidebar_window_width;
    
    Vector2f settings_window_size;
    Vector2f settings_window_position;
    
    Vector2f modules_bar_size;
    Vector2f modules_bar_position;

public:
    bool hide_interface = false;
    bool show_popup = false;

    bool show_sidebar_window = true;
    bool show_settings_window = false;
    bool show_modules_bar = true;

    bool load_grid_popup = false;
    bool save_grid_popup = false;
    bool resize_grid_popup = false;
    bool themes_grid_popup = false;
    bool set_background_popup = false;
    bool show_keybinds_popup = false;

    void SetSettingsWindow(Vector2f position, Vector2f size)
    {
        settings_window_position = position;
        settings_window_size = size;
    }

    void SetModulesBar(Vector2f position, Vector2f size)
    {
        modules_bar_position = position;
        modules_bar_size = size;
    }

    Vector2f GetSettingsWindowPosition()
    {
        return settings_window_position;
    }
    
    Vector2f GetSettingsWindowSize()
    {
        return settings_window_size;
    }

    Vector2f GetModulesbarPosition()
    {
        return modules_bar_position;
    }
    
    Vector2f GetModulesbarSize()
    {
        return modules_bar_size;
    }

    void SetSidebarWindow(float width)
    {
        sidebar_window_width = width;
    }

    float GetSidebarWidth()
    {
        return sidebar_window_width;
    }

    float GetMenubarHeight()
    {
        return menubar_height;
    }
};