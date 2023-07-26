debug:
	g++ ./graphics.cpp -O0 -lsfml-graphics -lsfml-window -lsfml-audio -lsfml-system
release:
	g++ ./graphics.cpp -O3 -lsfml-graphics -lsfml-window -lsfml-audio -lsfml-system
