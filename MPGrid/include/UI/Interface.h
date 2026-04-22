class Interface
{
private:
    float menubar_height;
    float sidebar_window_width;

    Vector2f settings_window_size;
    Vector2f settings_window_position;

public:
    bool show_sidebar_window;
    bool show_settings_window;
    bool show_popup;
    bool hide_interface;

    bool load_grid_popup;
    bool save_grid_popup;
    bool resize_grid_popup;

    Interface()
    {
        hide_interface = false;
        show_sidebar_window = true;
        show_settings_window = false;
        show_popup = false;

        load_grid_popup = false;
        save_grid_popup = false;
        resize_grid_popup = false;

        menubar_height = 18;
    }

    void SetSettingsWindow(Vector2f position, Vector2f size)
    {
        settings_window_position = position;
        settings_window_size = size;
    }

    Vector2f GetSettingsWindowPosition()
    {
        return settings_window_position;
    }
    
    Vector2f GetSettingsWindowSize()
    {
        return settings_window_size;
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