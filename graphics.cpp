#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cmath>

#define det(q, w, e, r) ((q)*(r) - (w)*(e))

sf::Vector2f find_barycenter(std::vector<sf::Vector2f> dots){
	sf::Vector2f barycenter, next_coords, current_coords;
	double area, intervalue, area_sum = 0, x_sum = 0, y_sum = 0, n = dots.size();
	for(int i = 0; i < n; i++){
		current_coords = dots[i];
		if(i != n - 1){
			next_coords = dots[i + 1];
		}
		else{
			next_coords = dots[0];
		}

		intervalue = (current_coords.x * next_coords.y - next_coords.x * current_coords.y);
		x_sum += (current_coords.x + next_coords.x)*intervalue;
		y_sum += (current_coords.y + next_coords.y)*intervalue;
		area_sum += intervalue;
	}

	area = area_sum/2;
	barycenter.x = x_sum/(6*area);
	barycenter.y = y_sum/(6*area);
	return barycenter;
}

bool onSegment(sf::Vector2f A, sf::Vector2f B, float x, float y){
	float x1, x2, y1, y2;
	x1 = std::max(A.x, B.x);
	x2 = std::min(A.x, B.x);
	y1 = std::max(A.y, B.y);
	y2 = std::min(A.y, B.y);
	return (x1 - x >= -1e-9) && (x2 - x <= 1e-9) && (y1 - y >= -1e-9) && (y2 - y <= 1e-9);
}

bool segment_intersect(sf::Vector2f A, sf::Vector2f B, sf::Vector2f C, sf::Vector2f D){
	float a1, b1, c1, a2, b2, c2;
	float num, den, num2, dot_x, dot_y;

	if(A.x == B.x){
		a1 = 0;
		b1 = 1;
		c1 = -A.x;
	}
	else{
		a1 = 1;
		b1 = (A.y - B.y)/(A.x - B.x);
		c1 = A.y - b1*A.x;
	}

	if(C.x == D.x){
		a2 = 0;
		b2 = 1;
		c2 = -C.x;
	}
	else{
		a2 = 1;
		b2 = (C.y - D.y)/(C.x - D.x);
		c2 = C.y - b2*C.x;
	}

	den = det(a1, b1, a2, b2);
	num = det(c1, b1, c2, b2);
	if((std::abs(den) < 1e-9) && (std::abs(num) < 1e-9)){
		return onSegment(A, B, C.x, C.y) || onSegment(A, B, D.x, D.y);
	}
	if(std::abs(den) < 1e-9) return false;
	num2 = det(a1, c1, a2, c2);
	dot_x = - num2/den;
	dot_y = num/den;
	return onSegment(A, B, dot_x, dot_y) && onSegment(C, D, dot_x, dot_y);
}

bool intersect_shape(std::vector<sf::Vector2f> first_shape, std::vector<sf::Vector2f> second_shape){
	sf::Vector2f A, B, C, D;
	for(int i = 0; i < first_shape.size(); i++ ){
		A = first_shape[i];
		if(i == first_shape.size() - 1){
			B = first_shape[0];
		}
		else{
			B = first_shape[i + 1];	
		}
		for(int j = 0; j < second_shape.size(); j++){
			C = second_shape[j];
			if(j == second_shape.size() - 1){
				D = second_shape[0];
			}
			else{
				D = second_shape[j + 1];	
			}
			if(segment_intersect(A, B, C, D)) return true;
		}
	}
	return false;
}

bool intersect_tiles(std::vector<std::vector<sf::Vector2f>> tiles, std::vector<sf::Vector2f> shape){
	for(int i = 0; i < tiles.size(); i++){
		if(intersect_shape(tiles[i], shape)){
			return true;
		}
	}
	return false;
}

int main()
{
	bool button_pressed = false, key_pressed = false, optimized = false;
	int shiftx, y;
	sf::ContextSettings settings;
    settings.antialiasingLevel = 8.0;
	sf::RenderWindow window(sf::VideoMode(1366, 768), "Packing", sf::Style::Default, settings);
	sf::Vector2f barycenter, shift, new_dot;
	std::vector<sf::Vector2f> dots = {};
	std::vector<std::vector<sf::Vector2f>> tiles = {};
	float m, n;
	double new_dist;
	
	while (window.isOpen()){
		window.clear(sf::Color(128, 128, 128));

		sf::Event event;
		while (window.pollEvent(event)){
			if (event.type == sf::Event::Closed)
				window.close();
		}

		if (sf::Mouse::isButtonPressed(sf::Mouse::Left)){
			if(!button_pressed){
				dots.push_back(sf::Vector2f(sf::Mouse::getPosition(window)));
				button_pressed = true;
			}
		}
		else{
			button_pressed = false;
		}

		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)){
			if(!key_pressed){
				tiles.push_back(dots);
				key_pressed = true;
			}
		}
		else{
			key_pressed = false;
		}
		
		barycenter = find_barycenter(dots);
		n = barycenter.x;
		m = barycenter.y;
		
		if(tiles.size() > 0 && !optimized){
			std::vector<sf::Vector2f> optimized_dots;
			double dist = 1e18;
			for(int i = -n - 1000; i < 2366 - n; i += 10){
				for(int j =  -m - 1000; j < 1768 - m; j += 10){
					std::vector<sf::Vector2f> new_dots;
					for(int dot = 0; dot < dots.size(); dot++){
						new_dots.push_back(sf::Vector2f(dots[dot].x + i, dots[dot].y + j));
					}
					new_dist = sqrt(pow(find_barycenter(new_dots).x - n, 2) + pow(find_barycenter(new_dots).y - m, 2));
					if((new_dist < dist) && (!intersect_tiles(tiles, new_dots))){
						dist = new_dist;
						optimized_dots = std::vector<sf::Vector2f>(new_dots);
						shift = sf::Vector2f(sf::Vector2i({i, j}));
					}
				}
			}
			if(dist == 1e18){
				optimized = true;
				std::cout << "optimized" << std::endl;
			}

			int number_tiles = tiles.size();

			for(int shape = 0; shape < number_tiles; shape++){
				unsigned multiplier = 1;
				while((std::abs(shift.x * multiplier) < 1366) && (std::abs(shift.y * multiplier) < 768)){
					std::vector<sf::Vector2f> new_shape;
					for(int dot = 0; dot < dots.size(); dot++){
						new_dot = tiles[shape][dot];
						new_dot.x += shift.x * multiplier;
						new_dot.y += shift.y * multiplier;
						new_shape.push_back(new_dot);
					}
					if(!intersect_tiles(tiles, new_shape)){
						tiles.push_back(new_shape);	
					}

					new_shape = {};
					for(int dot = 0; dot < dots.size(); dot++){
						new_dot = tiles[shape][dot];
						new_dot.x -= shift.x * multiplier;
						new_dot.y -= shift.y * multiplier;
						new_shape.push_back(new_dot);
					}
					if(!intersect_tiles(tiles, new_shape)){
						tiles.push_back(new_shape);	
					}
					multiplier++;
				}
			}
		}		
		
		for(int i = 0; i < tiles.size(); i++){
			sf::ConvexShape shape(tiles[i].size());
			for(int j = 0; j < tiles[i].size(); j++){
				shape.setPoint(j, tiles[i][j]);
				shape.setFillColor(sf::Color(192, 192, 192));
				shape.setOutlineThickness(1);
				shape.setOutlineColor(sf::Color::Black);
			}
			window.draw(shape);
		}

		for(unsigned long i = 0; i < dots.size(); i++){
			sf::CircleShape shape(10);
			shape.setPosition(dots[i]);
			shape.setFillColor(sf::Color::Green);
			window.draw(shape);
		}
		window.display();
	}

	return 0;
}

