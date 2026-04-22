class Interface
{
private:
    float menubar_height = 18;
    float sidebar_window_width;

    Vector2f settings_window_size;
    Vector2f settings_window_position;

public:
    bool hide_interface = false;
    bool show_popup = false;

    bool show_sidebar_window = true;
    bool show_settings_window = false;

    bool load_grid_popup = false;
    bool save_grid_popup = false;
    bool resize_grid_popup = false;
    bool themes_grid_popup = false;

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