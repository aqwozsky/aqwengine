#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>

struct Vec3
{
    float x, y, z;
};

Vec3 rotateY(Vec3 v, float angle)
{
    float c = cos(angle);
    float s = sin(angle);
    return {
        v.x * c - v.z * s,
        v.y,
        v.x * s + v.z * c};
}

sf::Vector2f project(Vec3 v, float fov, float width, float height)
{
    float z = v.z + 5.0f; // kameradan uzaklık
    float factor = fov / z;

    return {
        v.x * factor + width / 2.0f,
        -v.y * factor + height / 2.0f};
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "Software 3D");

    std::vector<Vec3> cube = {
        {-1, -1, -1}, {1, -1, -1}, {1, 1, -1}, {-1, 1, -1}, {-1, -1, 1}, {1, -1, 1}, {1, 1, 1}, {-1, 1, 1}};

    std::vector<std::pair<int, int>> edges = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0}, {4, 5}, {5, 6}, {6, 7}, {7, 4}, {0, 4}, {1, 5}, {2, 6}, {3, 7}};

    float angle = 0;

    while (window.isOpen())
    {
        sf::Event e;
        while (window.pollEvent(e))
            if (e.type == sf::Event::Closed)
                window.close();

        angle += 0.01f;

        window.clear(sf::Color::Black);

        std::vector<sf::Vector2f> projected;

        for (auto v : cube)
        {
            Vec3 r = rotateY(v, angle);
            projected.push_back(project(r, 400, 800, 600));
        }

        for (auto [a, b] : edges)
        {
            sf::Vertex line[] = {
                sf::Vertex(projected[a], sf::Color::White),
                sf::Vertex(projected[b], sf::Color::White)};
            window.draw(line, 2, sf::Lines);
        }

        window.display();
    }
}