#include "Platform/Platform.hpp"
#include "cmath"
#include "complex"

void generate_image(sf::Image& image, int resolution, double interval_range, double h, int max_iterations);

void generate_image(sf::Image& image, int resolution, double interval_range, double h, int max_iterations)
{
	image.create(resolution, resolution);
	double x0 = -interval_range / 2;
	double y0 = -interval_range / 2;
	double x, y, x2, y2;
	for (int i = 0; i < resolution; i++)
	{
		x0 = -interval_range / 2;

		for (int j = 0; j < resolution; j++)
		{
			x = 0.0;
			y = 0.0;
			x2 = 0.0;
			y2 = 0.0;
			sf::Uint8 n = 0;
			while (x * x + y * y <= 4 && n < max_iterations)
			{
				y = 2 * x * y + y0;
				x = x2 - y2 + x0;
				x2 = x * x;
				y2 = y * y;
				n += 1;
			}
			image.setPixel(j, i, sf::Color(255 * n / max_iterations, 50 * n / max_iterations, 50 * n / max_iterations));
			x0 += h;
		}
		y0 += h;
	}
}

int main()
{
	int resolution = 800;
	double interval_range = 4;
	double h = interval_range / (1.0 * resolution);

	int max_iterations = 30;

	util::Platform platform;

	sf::RenderWindow window;
	// in Windows at least, this must be called before creating the window
	float screenScalingFactor = platform.getScreenScalingFactor(window.getSystemHandle());
	// Use the screenScalingFactor
	window.create(sf::VideoMode(resolution * screenScalingFactor, resolution * screenScalingFactor), "SFML works!");
	platform.setIcon(window.getSystemHandle());
	sf::Image image;
	generate_image(image, resolution, interval_range, h, max_iterations);

	sf::Texture texture;
	texture.loadFromImage(image);

	sf::Sprite sprite(texture);

	sf::Event event;
	window.clear();
	while (window.isOpen())
	{
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}
		window.draw(sprite);
		window.display();
	}

	return 0;
}
