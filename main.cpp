#include <iostream>
#include <vector>
#include <stdlib.h>
#include <SFML/Graphics.hpp>

#include "Maths.h"

#define POINTS 20

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

void render(sf::RenderWindow* window, std::vector<vec2f> points)
{
    sf::ConvexShape DrawTriangle;
    DrawTriangle.setPointCount(3);
    
    auto triangulation = BowyerWatson(points);
    for (auto& triangle : triangulation)
    {
        DrawTriangle.setPoint(0, { triangle.points[0].x, triangle.points[0].y });
        DrawTriangle.setPoint(1, { triangle.points[1].x, triangle.points[1].y });
        DrawTriangle.setPoint(2, { triangle.points[2].x, triangle.points[2].y });
        int r = rand() % 256;
        int g = rand() % 256;
        int b = rand() % 256;
        DrawTriangle.setFillColor(sf::Color(r, g, b, 255));
        window->draw(DrawTriangle);
    }

    std::cout << "Drew: " << triangulation.size() << " triangles\n";
}

int main()
{
    srand(time(0));

    sf::RenderWindow window({ 1280, 720 }, "Triangulation");

    // Super triangle
    // Jednakostranicni trokut u kojemu pravokutnik iznutra predstavlja ekran, jer sve tocke moraju biti unutar super
    float offset = 50; // jer je doljna stranica direktno na dnu ekrana malo povecanje cjelog trokuta
    float w = window.getSize().x;
    float h = window.getSize().y;
    vec2f p1 = { -1 * (h / sqrtf(3)) - offset, h + offset };
    vec2f p2 = { +1 * (h / sqrtf(3)) - offset + w, h + offset };
    vec2f p3 = { w / 2, -1 * (sqrtf(3) * w * 0.5f) - offset };
    super.Initialize(p1, p2, p3);

    // Generate random points
    // TODO - provjera svake tocke da unutar njenog radijusa r nema drugih tocaka, neki razmak
    std::vector<vec2f> points;
    for (int i = 0; i < POINTS; i++)
    {
        float x = rand() % window.getSize().x;
        float y = rand() % window.getSize().y;
        points.push_back({ x, y });
    }

    // Render only once
    render(&window, points);
    window.display();

    // Event loop
    while (window.isOpen())
    {
        sf::Event e;
        while (window.pollEvent(e))
        {
            if (e.type == sf::Event::Closed)
                window.close();
        }
    }
}