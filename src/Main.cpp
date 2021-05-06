#include "Platform/Platform.hpp"
#include "cmath"
#include "complex"

void generate_image(sf::Image& image, int resolution, double interval_range, double h, int max_iterations);

void generate_image(sf::Image& image, int resolution, double interval_range, double h, int max_iterations)
{
	image.create(resolution, resolution);
	double imag = -interval_range / 2;
	double real = -interval_range / 2;
	for (int i = 0; i < resolution; i++)
	{
		real = -interval_range / 2;
		for (int j = 0; j < resolution; j++)
		{
			std::complex<double> c(real, imag);
			std::complex<double> z;
			sf::Uint8 n = 0;
			while (norm(z) < 4 && n < max_iterations)
			{
				z = z * z + c;
				n += 1;
			}
			image.setPixel(j, i, sf::Color(255 * n / max_iterations, 50 * n / max_iterations, 50 * n / max_iterations));
			real += h;
		}
		imag += h;
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
