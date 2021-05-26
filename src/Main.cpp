#include "Platform/Platform.hpp"
#include <chrono>
#include <cmath>
#include <thread>
using namespace std::chrono;

#define height 600
#define width 900

void generate_image(sf::Image& image, std::vector<double> interval, int max_iterations);
void panning(std::vector<double>& interval, double sensitivity, int mouse_x, int mouse_y, int& init_x, int& init_y);
void generate_image(sf::Image& image, std::vector<double> interval, int max_iterations)
{

	double h_x = (interval.at(1) - interval.at(0)) / (1.0 * width);
	double h_y = (interval.at(2) - interval.at(3)) / (1.0 * height);
	double x0 = interval.at(0), y0 = interval.at(3);
	double x, y, x2, y2;
	bool symmetry;
	sf::Uint8* pixels = new sf::Uint8[4 * width * height * 16]; // Multiplied by 4 because of RGBA
	int index = 0;
	auto start = high_resolution_clock::now();
	for (int i = 0; i < height; i++)
	{
		x0 = interval.at(0);

		symmetry = y0 < 0.0 && -y0 <= interval.at(3);

		for (int j = 0; j < width; j++)
		{
			if (symmetry) // Utilize symmetry
			{
				int reflected_y_index = (int)(-1.0 + (-y0 - interval.at(3)) / h_y);
				int reflected_index = j + width * reflected_y_index;
				pixels[index] = pixels[4 * reflected_index];
				pixels[index + 1] = pixels[4 * reflected_index + 1];
				pixels[index + 2] = pixels[4 * reflected_index + 2];
				pixels[index + 3] = pixels[4 * reflected_index + 3];
			}
			else // Escape algorithm
			{
				double p = sqrt((x0 - 0.25) * (x0 - 0.25) + y0 * y0);
				// Check if inside cardioid or period-2 bulb
				if (x0 <= p - 2 * p * p + 0.25 || (x0 + 1) * (x0 + 1) + y0 * y0 <= 0.0625)
				{
					pixels[index] = (int)(255.0);
					pixels[index + 1] = (int)(50.0);
					pixels[index + 2] = (int)(50.0);
					pixels[index + 3] = 255;
				}
				else
				{
					x = y = x2 = y2 = 0.0;

					int n = 0;
					while (x * x + y * y <= 4 && n < max_iterations)
					{
						y = 2 * x * y + y0;
						x = x2 - y2 + x0;
						x2 = x * x;
						y2 = y * y;
						n += 1;
					}

					pixels[index] = (int)(255.0 * n / 1.0 * max_iterations);
					pixels[index + 1] = (int)(50.0 * n / 1.0 * max_iterations);
					pixels[index + 2] = (int)(50.0 * n / 1.0 * max_iterations);
					pixels[index + 3] = 255;
				}
				x0 += h_x;
			}

			index += 4;
		}
		y0 += h_y;
	}
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);
	std::cout << "First loop " << duration.count() << std::endl;
	start = high_resolution_clock::now();
	image.create(width, height, &pixels[0]);
	stop = high_resolution_clock::now();
	duration = duration_cast<microseconds>(stop - start);
	std::cout << "Second loop " << duration.count() << std::endl;
	delete[] pixels;
}

void panning(std::vector<double>& interval, double sensitivity, int mouse_x, int mouse_y, int& init_x, int& init_y)
{
	double delta_x = mouse_x - init_x;
	double delta_y = mouse_y - init_y;

	interval.at(0) -= sensitivity * (interval.at(1) - interval.at(0)) * delta_x / (2.0 * width);
	interval.at(1) -= sensitivity * (interval.at(1) - interval.at(0)) * delta_x / (2.0 * width);
	interval.at(2) += sensitivity * (interval.at(3) - interval.at(2)) * delta_y / (2.0 * height);
	interval.at(3) += sensitivity * (interval.at(3) - interval.at(2)) * delta_y / (2.0 * height);

	init_x = mouse_x;
	init_y = mouse_y;
}
void zoom(std::vector<double>& interval, double zoom_factor, int mouse_x, int mouse_y)
{
	double mouse_x_before = interval.at(0) + (interval.at(1) - interval.at(0)) * mouse_x / (1.0 * width);
	double mouse_y_before = interval.at(3) + (interval.at(2) - interval.at(3)) * mouse_y / (1.0 * height);

	//Zoom
	interval.at(0) /= zoom_factor;
	interval.at(1) /= zoom_factor;
	interval.at(2) /= zoom_factor;
	interval.at(3) /= zoom_factor;

	double mouse_x_after = interval.at(0) + (interval.at(1) - interval.at(0)) * mouse_x / (1.0 * width);
	double mouse_y_after = interval.at(3) + (interval.at(2) - interval.at(3)) * mouse_y / (1.0 * height);

	//Correction
	interval.at(0) -= mouse_x_after - mouse_x_before;
	interval.at(1) -= mouse_x_after - mouse_x_before;
	interval.at(2) -= mouse_y_after - mouse_y_before;
	interval.at(3) -= mouse_y_after - mouse_y_before;
}
int main()
{
	double zoom_factor = 1.1;
	double sensitivity = 1.0;
	int max_iterations = 1000;
	std::vector<double> interval = { -2, 1, -1, 1 }; //x_start, x_end, y_start, y_end

	util::Platform platform;

	sf::RenderWindow window;
	// in Windows at least, this must be called before creating the window
	float screenScalingFactor = platform.getScreenScalingFactor(window.getSystemHandle());
	// Use the screenScalingFactor
	window.create(sf::VideoMode(width * screenScalingFactor, height * screenScalingFactor), "Mandelbrot");
	platform.setIcon(window.getSystemHandle());
	sf::Image image;
	generate_image(image, interval, max_iterations);

	sf::Texture texture;
	texture.loadFromImage(image);
	sf::Sprite sprite(texture);

	sf::Event event;
	bool buttonHold = false;
	int init_x = 0;
	int init_y = 0;
	while (window.isOpen())
	{
		window.clear();
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::MouseButtonPressed)
			{
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					buttonHold = true;
					init_x = event.mouseButton.x;
					init_y = event.mouseButton.y;
				}
			}
			else if (event.type == sf::Event::MouseButtonReleased)
			{
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					buttonHold = false;
				}
			}
			if (event.type == sf::Event::MouseMoved)
			{
				if (buttonHold)
				{
					if (event.mouseMove.x != init_x || event.mouseMove.y != init_y) //Panning
					{
						panning(interval, sensitivity, event.mouseMove.x, event.mouseMove.y, init_x, init_y);
						generate_image(image, interval, max_iterations);
						texture.loadFromImage(image);
						sf::Sprite sprite(texture);
					}
				}
			}

			if (event.type == sf::Event::MouseWheelMoved)
			{
				zoom(interval, zoom_factor, event.mouseWheel.x, event.mouseWheel.y);
				generate_image(image, interval, max_iterations);
				texture.loadFromImage(image);
				sf::Sprite sprite(texture);
			}
		}
		window.draw(sprite);
		window.display();
	}

	return 0;
}
