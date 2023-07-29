#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <functional>

#define det(q, w, e, r) ((q)*(r) - (w)*(e))

sf::Vector2f find_barycenter(std::vector<sf::Vector2f> points){
	sf::Vector2f barycenter, next_coords, current_coords;
	double area, intervalue, area_sum = 0, x_sum = 0, y_sum = 0, n = points.size();
	for(int i = 0; i < n; i++){
		current_coords = points[i];
		if(i != n - 1){
			next_coords = points[i + 1];
		}
		else{
			next_coords = points[0];
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
	float num, den, num2, point_x, point_y;

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
	point_x = - num2/den;
	point_y = num/den;
	return onSegment(A, B, point_x, point_y) && onSegment(C, D, point_x, point_y);
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

void shape_as_points(sf::RenderWindow &window, std::vector<sf::Vector2f> shape, sf::Color fill_color, sf::Color outline_color, unsigned radius){
	for(unsigned long id = 0; id < shape.size(); id++){
			sf::CircleShape point(radius);
			point.setPosition(shape[id]);
			point.setFillColor(fill_color);
			point.setOutlineThickness(1);
			point.setOutlineColor(outline_color);
			window.draw(point);
		}
}

void draw_shape(sf::RenderWindow &window, std::vector<sf::Vector2f> shape, sf::Color fill_color, sf::Color outline_color){
	sf::ConvexShape drawed(shape.size());
	for(int point = 0; point < shape.size(); point++){
		drawed.setPoint(point, shape[point]);
		drawed.setFillColor(fill_color);
		drawed.setOutlineThickness(1);
		drawed.setOutlineColor(outline_color);
	}
	window.draw(drawed);
}

int main()
{
	bool button_pressed = false, enter_pressed = false;
	unsigned optimized = 0;
	int shiftx, y;
	sf::ContextSettings settings;
    settings.antialiasingLevel = 8.0;
	sf::RenderWindow window(sf::VideoMode(1366, 768), "Packing", sf::Style::Default, settings);
	sf::Vector2f barycenter, shift, new_point;
	std::vector<sf::Vector2f> points = {};
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
				points.push_back(sf::Vector2f(sf::Mouse::getPosition(window)));
				button_pressed = true;
			}
		}
		else{
			button_pressed = false;
		}

		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)){
			if(!enter_pressed){
				tiles.push_back(points);
				enter_pressed = true;
			}
		}
		else{
			enter_pressed = false;
		}
		
		barycenter = find_barycenter(points);
		n = barycenter.x;
		m = barycenter.y;
		
		if(tiles.size() > 0 && optimized != 2){
			std::vector<sf::Vector2f> optimized_points;
			double dist = 1e18;
			for(int i = -n -1366; i < 1366*2 - n; i += 10){
				for(int j =  -m -768; j < 768*2 - m; j += 10){
					std::vector<sf::Vector2f> new_points;
					for(int point = 0; point < points.size(); point++){
						new_points.push_back(sf::Vector2f(points[point].x + i, points[point].y + j));
					}
					new_dist = sqrt(pow(find_barycenter(new_points).x - n, 2) + pow(find_barycenter(new_points).y - m, 2));
					if((new_dist < dist) && (!intersect_tiles(tiles, new_points))){
						dist = new_dist;
						optimized_points = std::vector<sf::Vector2f>(new_points);
						shift = sf::Vector2f(sf::Vector2i({i, j}));
					}
				}
			}

			int number_tiles = tiles.size();

			for(int shape = 0; shape < number_tiles; shape++){
				unsigned multiplier = 1;
				while((std::abs(shift.x * multiplier) < 1366 * 2) && (std::abs(shift.y * multiplier) < 768 * 2)){
					std::vector<sf::Vector2f> new_shape;
					for(int point = 0; point < points.size(); point++){
						new_point = tiles[shape][point];
						new_point.x += shift.x * multiplier;
						new_point.y += shift.y * multiplier;
						new_shape.push_back(new_point);
					}
					tiles.push_back(new_shape);	
					new_shape = {};
					for(int point = 0; point < points.size(); point++){
						new_point = tiles[shape][point];
						new_point.x -= shift.x * multiplier;
						new_point.y -= shift.y * multiplier;
						new_shape.push_back(new_point);
					}
					tiles.push_back(new_shape);	
					multiplier++;
				}
			}
			optimized++;
		}		
		
		for(int i = 0; i < tiles.size(); i++){
			draw_shape(window, tiles[i], sf::Color(192, 192, 192), sf::Color::Black);
		}
		shape_as_points(window, points, sf::Color(192, 192, 192), sf::Color::Black, 4);
		window.display();
	}

	return 0;
}
