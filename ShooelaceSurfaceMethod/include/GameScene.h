#include <iostream>
#include <vector>
#include <cmath>

#include "Input.h"

void DrawLine(Vector2f start, Vector2f end, Color color, int thickness = 1)
{
    sf::Vector2f direction = end - start;
    float length = direction.length();

    sf::Vector2f unit = direction / length;

    sf::Vector2f normal(-unit.y, unit.x);

    normal *= thickness / 2.f;

    sf::ConvexShape line;
    line.setPointCount(4);
    line.setPoint(0, start + normal);
    line.setPoint(1, end + normal);
    line.setPoint(2, end - normal);
    line.setPoint(3, start - normal);

    line.setFillColor(color);

    window.draw(line);
}

Vector2f TextMidPoint(const Text &text)
{
    Vector2f size = text.getGlobalBounds().size;
    return Vector2f(size.x * 0.5, size.y * 0.5);
}

namespace Point
{
    const int RADIUS = 2;
    const int OUTLINE_THICKNESS = 2;
    const Color FILL_COLOR = Color::Red;
    const Color OUTLINE_COLOR = Color::Black; 

    const int LINE_THICKNESS = OUTLINE_THICKNESS;
    const Color LINE_COLOR = OUTLINE_COLOR;

    namespace CoordText
    {
        const Vector2f OFFSET_POINT = Vector2f(0, -10);
        const Color FILL_COLOR = Color::White;
        const int CHAR_SIZE = 10;

        std::string Format(int x, int y)
        {
            return "(" + std::to_string(x) + " " + std::to_string(y) + ")";
        }

        void SetText(Text &text, Vector2f position)
        {
            text.setString(Format(position.x, position.y));

            text.setPosition(position - TextMidPoint(text) + OFFSET_POINT);
        }
    }

    namespace LineLengthText
    {
        const Color FILL_COLOR = Color::Black;
        const int CHAR_SIZE = 10;
        const int TEXT_STYLE = Text::Regular;

        void SetText(Text &text, Vector2f start, Vector2f end)
        {
            float length = sqrt(((end.x - start.x) * (end.x - start.x)) +  ((end.y - start.y) * (end.y - start.y)));

            Vector2f direction = end - start;
            Vector2f unit = direction / length;

            float angle = atan2(direction.y, direction.x);

            text.setString(std::to_string(int(length)));
            text.setRotation(radians(angle));
            
            Vector2f line_mid_point = Vector2f((start.x + end.x) * 0.5, (start.y + end.y) * 0.5);
            Vector2f text_mid_point = TextMidPoint(text);

            Vector2f position = line_mid_point;
            position -= Vector2f(unit.x * text_mid_point.x, unit.y * text_mid_point.y);

            text.setPosition(position);
        }
        
    }

    namespace ShapeAreaText
    {
        const Color FILL_COLOR = Color::Green;
        const int CHAR_SIZE = 10;
        const int TEXT_STYLE = Text::Bold;
        
        void SetText(Text &text, float area, const std::vector<Vector2f> &points)
        {
            Vector2f shape_mid_point;
            for(const auto &point : points)
                shape_mid_point += point;

            shape_mid_point.x /= points.size();
            shape_mid_point.y /= points.size();

            text.setString(std::to_string(int(area)));
            text.setPosition(shape_mid_point);
        }
    }
}

struct Triangle
{
    Vector2f vertex[3];
    float surface;
};  

namespace TriangleSurface
{
    const int OUTLINE_THICKNESS = 1;
    const Color FILL_COLOR = Color(153, 123, 0, 50);
    const Color OUTLINE_COLOR = Color(0, 0, 0, 20);

    namespace SurfaceText 
    {
        const Color FILL_COLOR = Color::Red;
        const int CHAR_SIZE = 10;

        void SetText(Text &text, const Triangle &triangle)
        {
            Vector2f triangle_mid_point = triangle.vertex[0] + triangle.vertex[1] + triangle.vertex[2];
            triangle_mid_point = Vector2f(triangle_mid_point.x / 3, triangle_mid_point.y / 3);   

            text.setString(std::to_string(int(triangle.surface)));
            text.setPosition(triangle_mid_point - TextMidPoint(text));
        }
    }  
}

CircleShape point_shape;
Text point_coord_text{font_JetBrains};
Text line_length_text{font_JetBrains};
Text triangle_surface_text{font_JetBrains};
Text mouse_coord_text{font_JetBrains};
Text shape_area_text{font_JetBrains};

std::vector<Vector2f> points;

float shape_area;

void DrawShape()
{
    for(int i = 0; i < points.size(); i++)
    {
        Vector2f pointA = points[i];

        if(points.size() > 1)
        {
            Vector2f pointB = points[i + 1];

            if(i + 1 >= points.size())   
            {
                pointA = points[i];
                pointB = points[0];
            }
        
            point_shape.setPosition(pointB);
            window.draw(point_shape);

            DrawLine(pointA, pointB, Point::LINE_COLOR, Point::LINE_THICKNESS);

            Point::CoordText::SetText(point_coord_text, pointB);
            window.draw(point_coord_text);
             
            Point::LineLengthText::SetText(line_length_text, pointA, pointB);
            window.draw(line_length_text);
        }

        point_shape.setPosition(pointA);
        window.draw(point_shape);

        Point::CoordText::SetText(point_coord_text, pointA);
        window.draw(point_coord_text);
    }
}

void DrawShapeAreaText()
{
    if(points.size() < 3) return;

    Point::ShapeAreaText::SetText(shape_area_text, shape_area, points);
    window.draw(shape_area_text);
}

/*
    |a   b| 0
    |  X  |
    |c   d| 1
    |e   f| 2
    |  X  |
    |g   h| 3
    |i   j| 4
    |  X  | 
    |k   l| 5
*/

float ShoelaceAlgo(const std::vector<Vector2f> &points)
{
    if(points.size() < 3) return 0;

    float area = 0;
    for(int i = 0; i < points.size(); i++)
    {
        Vector2f vertexA = points[i];
        Vector2f vertexB = points[(i + 1 ) % points.size()];

        float shooelace = vertexA.x * vertexB.y - vertexB.x * vertexA.y;

        area += shooelace;
    }

    return abs(area) * 0.5;
}

ConvexShape triangle_shape;
std::vector<Triangle> triangles;

void ConstructTriangleSurfaces(const std::vector<Vector2f> &points)
{
    triangles.clear();
    for(int i = 1; i + 1 < points.size(); i++)
    {
        Triangle triangle;
        triangle.vertex[0] = points[0];
        triangle.vertex[1] = points[i];
        triangle.vertex[2] = points[i + 1];

        float area = std::abs(points[0].x * (points[i].y - points[i + 1].y) +
                                points[i].x * (points[i + 1].y - points[0].y) +
                                points[i + 1].x * (points[0].y - points[i].y));
        
        triangle.surface = area * 0.5;

        triangles.push_back(triangle);
    }
}

void DrawTriangles()
{
    for(const auto &triangle : triangles)
    {
        triangle_shape.setPoint(0, triangle.vertex[0]);
        triangle_shape.setPoint(1, triangle.vertex[1]);
        triangle_shape.setPoint(2, triangle.vertex[2]);

        window.draw(triangle_shape);

        TriangleSurface::SurfaceText::SetText(triangle_surface_text, triangle);
        window.draw(triangle_surface_text);
    }
}

void UpdateCursorCoordPosition()
{
    mouse_coord_text.setString(Point::CoordText::Format(worldMousePosition.x, worldMousePosition.y));

    Vector2f coord_mouse_size = mouse_coord_text.getGlobalBounds().size;
    
    Vector2f coord_mouse_offset = Vector2f(20, 20);
    Vector2f bottom_right_corner = canvasMousePosition + Vector2f(20, 20) + coord_mouse_size;

    if(bottom_right_corner.x > window_width)
        coord_mouse_offset.x = -coord_mouse_size.x - 20;

    if(bottom_right_corner.y > window_height)
        coord_mouse_offset.y = -coord_mouse_size.y;
    

    mouse_coord_text.setPosition(canvasMousePosition + coord_mouse_offset);
}

void Start()
{
    canvas = window.getView();
    camera = View(sf::Vector2f(0, 0), sf::Vector2f(window_width, window_height));

    point_shape = CircleShape(Point::RADIUS);
    point_shape.setOrigin({Point::RADIUS, Point::RADIUS});
    point_shape.setOutlineThickness(Point::OUTLINE_THICKNESS);
    point_shape.setFillColor(Point::FILL_COLOR);
    point_shape.setOutlineColor(Point::OUTLINE_COLOR);

    point_coord_text.setFont(font_JetBrains);
    point_coord_text.setCharacterSize(Point::CoordText::CHAR_SIZE);
    point_coord_text.setFillColor(Point::CoordText::FILL_COLOR);
    
    line_length_text.setFont(font_JetBrains);
    line_length_text.setCharacterSize(Point::LineLengthText::CHAR_SIZE);
    line_length_text.setFillColor(Point::LineLengthText::FILL_COLOR);
    line_length_text.setStyle(Point::LineLengthText::TEXT_STYLE);

    shape_area_text.setFont(font_JetBrains);
    shape_area_text.setCharacterSize(Point::ShapeAreaText::CHAR_SIZE);
    shape_area_text.setFillColor(Point::ShapeAreaText::FILL_COLOR);
    shape_area_text.setStyle(Point::ShapeAreaText::TEXT_STYLE);

    triangle_surface_text.setFont(font_JetBrains);
    triangle_surface_text.setCharacterSize(TriangleSurface::SurfaceText::CHAR_SIZE);
    triangle_surface_text.setFillColor(TriangleSurface::SurfaceText::FILL_COLOR);

    triangle_shape.setPointCount(3);
    triangle_shape.setFillColor(TriangleSurface::FILL_COLOR);
    triangle_shape.setOutlineColor(TriangleSurface::OUTLINE_COLOR);
    triangle_shape.setOutlineThickness(TriangleSurface::OUTLINE_THICKNESS);

    mouse_coord_text.setCharacterSize(10);
    mouse_coord_text.setFillColor(Color::Black);
    mouse_coord_text.setStyle(Text::Bold);
}

void Update()
{   
    UpdateCursorCoordPosition();

    if(IsMouseButtonDown(Mouse::Button::Left))
    {
        points.push_back(worldMousePosition);
        shape_area = ShoelaceAlgo(points);
        ConstructTriangleSurfaces(points);
    }
}

void Draw()
{
    window.setView(camera);
    DrawTriangles();

    DrawShape();
    DrawShapeAreaText();

    window.setView(canvas);
    window.draw(mouse_coord_text);
}
