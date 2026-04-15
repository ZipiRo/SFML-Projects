struct Point
{
    Vector2i position;
    bool valid = false;
};

bool CheckPointOverlapBox(const Vector2f &point, const Vector2f &bmin, const Vector2f &bmax)
{
    if(point.x >= bmin.x && point.x <= bmax.x && point.y >= bmin.y && point.y <= bmax.y)
        return true;

    return false;
}

ImVec4 SFMLToImColor(const Color &color)
{
    return ImVec4(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f);
}

Color ImColorToSFML(const ImVec4 &color)
{
    return Color(color.x * 255, color.y * 255, color.z * 255, color.w * 255);
}

ImVec2 SFMLVector2iToImVec2(const Vector2i &vector)
{
    return ImVec2(vector.x, vector.y);
}

Vector2i ImVec2ToSFMLVector2i(const ImVec2 &vector)
{
    return Vector2i(vector.x, vector.y);
}

Color LerpColor(const Color& colora, const Color& colorb, float t)
{
    return Color(
        colora.r + (colorb.r - colora.r) * t,
        colora.g + (colorb.g - colora.g) * t,
        colora.b + (colorb.b - colora.b) * t,
        colora.a + (colorb.a - colora.a) * t
    );
}

float Vector2fDistance(const Vector2f &from, const Vector2f &to)
{
    return sqrt((to.x - from.x) * (to.x - from.x) + (to.y - from.y) * (to.y - from.y));
}

float Vector2iDistance(const Vector2i &from, const Vector2i &to)
{
    return sqrt((to.x - from.x) * (to.x - from.x) + (to.y - from.y) * (to.y - from.y));
}

static const Vector2i UP = Vector2i(0, -1);
static const Vector2i DOWN = Vector2i(0, 1);
static const Vector2i LEFT = Vector2i(-1, 0);
static const Vector2i RIGHT = Vector2i(1, 0);

const std::vector<Vector2i> directions = { LEFT, DOWN, RIGHT, UP };
