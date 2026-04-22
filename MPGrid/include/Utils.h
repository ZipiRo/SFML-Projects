const float PI = 3.1415926354f;

struct Point
{
    Vector2i position;
    bool valid;
};

template <typename T>
std::vector<T> VectorShuffle(std::vector<T> v) 
{
    for (int i = v.size() - 1; i > 0; --i)
    {
        int j = rand() % (i + 1);
        std::swap(v[i], v[j]);
    }

    return v;
}

ImVec4 SFMLToImColor(const Color &color)
{
    return ImVec4(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f);
}

Color ImColorToSFML(const ImVec4 &color)
{
    return Color(color.x * 255, color.y * 255, color.z * 255, color.w * 255);
}

static const Vector2i UP = Vector2i(0, -1);
static const Vector2i DOWN = Vector2i(0, 1);
static const Vector2i LEFT = Vector2i(-1, 0);
static const Vector2i RIGHT = Vector2i(1, 0);

const std::vector<Vector2i> directions = { LEFT, DOWN, RIGHT, UP };
