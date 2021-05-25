#include <iostream>
#include <vector>
#include <stdlib.h>
#include <SFML/Graphics.hpp>

#include "Maths.h"
#include "Options.h"

bool IsPointCircumCircle(float x, float y, vec2f point)
{
    return sqrtf(powf(x - point.x, 2) + powf(y - point.y, 2)) <= Options::radius;
}

// Globals
Triangle super;

std::vector<Triangle> BowyerWatson(std::vector<vec2f> points)
{
    // pointList is a set of coordinates defining the points to be triangulated
    // triangulation := empty triangle mesh data structure
    std::vector<Triangle> triangulation;
    // add super-triangle to triangulation
    triangulation.push_back(super);
    // for each point in pointList do // add all the points one at a time to the triangulation
    for (auto& point : points)
    {
        std::vector<Triangle> badTriangles;
        // for each triangle in triangulation do // first find all the triangles that are no longer valid due to the insertion
        for (auto& triangle : triangulation)
        {
            if (triangle.isPointInCircumcircle(point))
            {
                // add triangle to badTriangles
                badTriangles.push_back(triangle);
                triangle.isBad = true;
            }
        }

        std::vector<Edge> polygon;
        for (auto& triangle : badTriangles)
        {
            for (auto& edge : triangle.edges)
            {
                bool shared = false;
                for (auto& triangle2 : badTriangles)
                {
                    if (triangle.equals(triangle2))
                    {
                        continue;
                    }
                    else
                    {
                        for (auto& edge2 : triangle2.edges)
                        {
                            if (edge.equals(edge2))
                                shared = true;
                        }
                        
                    }
                }

                // if edge is not shared by any other triangles in badTriangles
                if (!shared)
                {
                    // add edge to polygon
                    polygon.push_back(edge);
                }
            }
        }

        // Remove all bad triangles from triangulation
        for (auto itr = triangulation.begin(); itr != triangulation.end(); )
        {
            if (itr->isBad)
                itr = triangulation.erase(itr);
            else itr++;
        }

        for (auto& edge : polygon)
        {
            Triangle newTriangle;
            newTriangle.Initialize(edge.a, edge.b, point);
            triangulation.push_back(newTriangle);
        }
    }

    // Remove all that contain vertexes from super
    for (auto& superVertex : super.points)
    {
        for (auto& triangle : triangulation)
        {
            for (auto& vertex : triangle.points)
            {
                if ((int)vertex.x == (int)superVertex.x && (int)vertex.y == (int)superVertex.y)
                {
                    triangle.isBad = true;
                    break;
                }
            }
        }
    }
    // Removes all triangles flagged as bad in this case the triangles with a point same as the super triangle
    for (auto itr = triangulation.begin(); itr != triangulation.end(); )
    {
        if (itr->isBad)
            itr = triangulation.erase(itr);
        else itr++;
    }

    // Return created triangles as vector<Triangle>
    return triangulation;
}

void render(sf::RenderTexture* renderTexture, std::vector<vec2f> points)
{
    sf::ConvexShape DrawTriangle;
    DrawTriangle.setPointCount(3);
    
    auto triangulation = BowyerWatson(points);
    for (auto& triangle : triangulation)
    {
        DrawTriangle.setPoint(0, { triangle.points[0].x, triangle.points[0].y });
        DrawTriangle.setPoint(1, { triangle.points[1].x, triangle.points[1].y });
        DrawTriangle.setPoint(2, { triangle.points[2].x, triangle.points[2].y });
        const float factor = rand() / (float)RAND_MAX;
        int r = 50.0f * factor;
        int g = 50.0f * factor;
        int b = 50.0f * factor;
        DrawTriangle.setFillColor(sf::Color(r, g, b, 255));
        renderTexture->draw(DrawTriangle);
    }
    renderTexture->display();

    std::cout << "Drew: " << triangulation.size() << " triangles\n";
}

void StartupSettingInput()
{
    std::cout << "Use default settings? y/n" << std::endl;
    char answer;
    std::cin >> answer;
    if (answer == 'y' || answer == 'Y')
    {
        Options::displayResolution = { 1280, 720 };
        Options::renderResolution  = { 1920 * 2, 1080 * 2 };
        Options::points = 200;
        Options::radius = 150;
    } 
    else if (answer == 'n' || answer == 'N')
    {
        std::cout << "Display size x: ";
        std::cin >> Options::displayResolution.x;
        std::cout << "Display size y: ";
        std::cin >> Options::displayResolution.y;

        std::cout << "Render size x: ";
        std::cin >> Options::renderResolution.x;
        std::cout << "Render size y: ";
        std::cin >> Options::renderResolution.y;

        std::cout << "Number of Points: ";
        std::cin >> Options::points;
        std::cout << "Radius of distance between points: ";
        std::cin >> Options::radius;
    }
    else
    {
        std::cout << "Invalid input!" << std::endl;
        StartupSettingInput();
    }
}

int main()
{
    // Get user settings
    StartupSettingInput();

    srand(time(0));

    sf::RenderWindow window({ Options::displayResolution.x, Options::displayResolution.y }, "Triangulation");
    
    sf::RenderTexture renderTexture;
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    renderTexture.create(Options::renderResolution.x, Options::renderResolution.y, settings);

    // Super triangle
    // Jednakostranicni trokut u kojemu pravokutnik iznutra predstavlja ekran, jer sve tocke moraju biti unutar super
    float offset = 250; // jer je doljna stranica direktno na dnu ekrana malo povecanje cjelog trokuta
    float w = renderTexture.getSize().x;
    float h = renderTexture.getSize().y;
    vec2f p1 = { -1 * (h / sqrtf(3)) - offset, h + offset };
    vec2f p2 = { +1 * (h / sqrtf(3)) + offset + w, h + offset };
    vec2f p3 = { w / 2, -1 * (sqrtf(3) * w * 0.5f) - offset };
    super.Initialize(p1, p2, p3);

    // Generate random points
    std::vector<vec2f> points;
    points.push_back({ w / 2, h / 2 });
    while (points.size() != Options::points)
    {
        float x = rand() % renderTexture.getSize().x;
        float y = rand() % renderTexture.getSize().y;

        bool valid = true;
        for (const auto& point : points)
        {
            if (IsPointCircumCircle(x, y, point))
            {
                valid = false;
                break;
            }
        }

        if (valid)
            points.push_back({ x, y });
    }

    // Render only once
    render(&renderTexture, points);
    sf::Sprite spr(renderTexture.getTexture());
    spr.setScale(1.0f / (w / window.getSize().x), 1.0f / (h / window.getSize().y)); // scale renderTexture to window by doing renderTextureRes / windowRes
    window.draw(spr);
    window.display();

    // Event loop
    while (window.isOpen())
    {
        sf::Event e;
        while (window.pollEvent(e))
        {
            if (e.type == sf::Event::Closed)
                window.close();
            else if (e.type == sf::Event::Resized)
            {
                window.draw(spr);
                window.display();
            }
            else if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::S)
            {
                renderTexture.getTexture().copyToImage().saveToFile("wallpaper.png");
                std::cout << "Saved wallpaper!\n";
            }
        }
    }
}