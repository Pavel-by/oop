// Примите, пожалуйста, вторую лабораторную. Там оставалась только кодировка :(
#include "Dot.h"
#define _USE_MATH_DEFINES 
#include <math.h>
#include <random>
#include "StepShared.h"
#include "Shape.h"
#include "Rhombus.h"
#include "Square.h"
#include "Trapezium.h"
#include "StepVector.h"


std::random_device rd;
std::default_random_engine gen(rd());

const long int BORDER = 50;

//ЭТО ТОЛЬКО ГЕНЕРАТОР АЛГОРИТМЫ В main-e 


//генератор
Dot generate_random_dot(long int BORDER) {

	Dot random_dot(rand() % BORDER, rand() % BORDER);
	return random_dot;
}

stepik::shared_ptr<Shape> generate_random_square(long int BORDER) {
	std::vector<Dot> corners(4);
	Dot first_corner = generate_random_dot(BORDER);
	std::uniform_real_distribution<double> rnd1(0, M_PI);
	double first_angle = rnd1(gen);
	double second_angle = first_angle + M_PI / 2;
	double side = rand() % BORDER + 1;
	corners[0] = generate_random_dot(BORDER);
	corners[1] = { Dot(corners[0].x + side*cos(first_angle), corners[0].y + side*sin(first_angle)) };
	corners[2] = { Dot(corners[1].x + side*cos(second_angle), corners[1].y + side*sin(second_angle)) };
	corners[3] = { Dot(corners[0].x + side*cos(second_angle), corners[0].y + side*sin(second_angle)) };
	std::uniform_int_distribution<unsigned int> color(0, 255);
	Color c(color(gen), color(gen), color(gen));
	Square* sq = new Square(corners);
	sq->set_color(c);
	return stepik::shared_ptr<Shape>(sq);
}

stepik::shared_ptr<Shape>  generate_random_rhombus(long int BORDER) {
	std::vector<Dot> corners(4);
	//случайная вершина от которой всё строится
	Dot first_corner = generate_random_dot(BORDER);
	std::uniform_real_distribution<double> rnd1(0, M_PI);
	double first_angle = rnd1(gen);
	std::uniform_real_distribution<double> rnd2(first_angle, M_PI);
	double second_angle = rnd2(gen);
	double side = rand() % BORDER + 1;
	corners[0] = generate_random_dot(BORDER);
	corners[1] = { Dot(corners[0].x + side*cos(first_angle), corners[0].y + side*sin(first_angle)) };
	corners[2] = { Dot(corners[1].x + side*cos(second_angle), corners[1].y + side*sin(second_angle)) };
	corners[3] = { Dot(corners[0].x + side*cos(second_angle), corners[0].y + side*sin(second_angle)) };

	//генерим цвет
	std::uniform_int_distribution<unsigned int> color(0, 255);
	Color c(color(gen), color(gen), color(gen));
	Rhombus* rh = new Rhombus(corners);
	rh->set_color(c);
	return stepik::shared_ptr<Shape>(rh);
}
stepik::shared_ptr<Shape>  generate_random_trapezium(long int BORDER) {
	std::vector<Dot> corners(4);
	//случайная вершина от которой всё строится
	std::uniform_real_distribution<double> rnd1(0, M_PI);
	double first_angle = rnd1(gen);
	double first_side = rand() % BORDER + 1;
	double second_side = rand() % BORDER + 1;

	corners[0] = generate_random_dot(BORDER);
	corners[1] = generate_random_dot(BORDER);
	corners[2] = { Dot(corners[1].x + first_side*cos(first_angle), corners[1].y + first_side*sin(first_angle)) };
	corners[3] = { Dot(corners[0].x + second_side*cos(first_angle), corners[0].y + second_side*sin(first_angle)) };

	//генерим цвет
	std::uniform_int_distribution<unsigned int> color(0, 255);
	Color c(color(gen), color(gen), color(gen));
	Trapezium* tr = new Trapezium(corners);
	tr->set_color(c);
	return stepik::shared_ptr<Shape>(tr);
}
stepik::shared_ptr<Shape> generate_random_shape() {
	int type = rand() % 3;

	switch (type)
	{
	case 0:
		return generate_random_square(100);
	case 1:
		generate_random_rhombus(100);
	case 2:
		generate_random_trapezium(100);


	default:
		return stepik::shared_ptr<Shape>();
	}
}
stepik::vector< stepik::shared_ptr<Shape> > generate_conteiner(size_t n)
{
	srand(time(NULL));
	stepik::vector< stepik::shared_ptr<Shape> > shapes(n);

	for (size_t i = 0; i < n; i++) {
		shapes[i] = stepik::shared_ptr<Shape>(generate_random_shape());
	}

	return shapes;
}

