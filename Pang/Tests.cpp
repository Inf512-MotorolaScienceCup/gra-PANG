#include <map>
#include <string>

#include "Tests.h"
#include "Game.h"
#include <time.h> 
#include <cstdlib>
#include <iostream>

struct TestResult {
    std::map<std::string, int> passed;
	int run;
};

TestResult TestMoveBall(Game* game)
{
	//
	// ZMIENNE POTRZEBNE DLA TESTOW
	//

	srand(time(NULL));

	float x;
	float y;
	float gravity = 0.3f;

	Vector2 ball1 = { 2.2f, -5 };
	Vector2 ball2 = { 1.8f, -5 };
	Vector2 ball3 = { 1.6f, -5 };
	Vector2 ball4 = { 1.4f, -5 };

	int heading;
	int randHeading;

	int num_tests = 10;
	int passedTests_b1 = 0, passedTests_b2 = 0, passedTests_b3 = 0, passedTests_b4 = 0;

	//
	// TESTY
	//

	//
	// LOSOWO GENEROWANE TESTY DLA KULEK TYPU 1
	//

	std::cout << "LOSOWO GENEROWANE TESTY DLA KULEK TYPU 1 \n";
	for (int i = 0; i < num_tests; i++)
	{
		//GENEROWANIE LICZB "LOSOWYCH"
		x = float((rand() % 100000) + 1) / 100;
		y = float((rand() % 100000) + 1) / 100;

		randHeading = (rand() % 2) + 1;
		if (randHeading == 2)
		{
			heading = 1;
		}
		if (randHeading == 1)
		{
			heading = -1;
		}

		//TWORZENIE TESTOWANEGO OBIEKTU I TRESC TESTU
		Sprite* ball = Enemy::create(game, x, y, Enemy::Kind::BALL1, heading);

		std::cout << "\n-----WARTOSCI DOSTARCZONE-----\n";
		std::cout << "Position X: " << ball->position.center.x << "\n";
		std::cout << "Position Y: " << ball->position.center.y << "\n";
		std::cout << "Heading: " << heading << "\n";


		ball->Move();


		std::cout << "\n-----WARTOSCI ODEBRANE-----\n";
		std::cout << "Position X: " << ball->position.center.x << "\n";
		std::cout << "Position Y: " << ball->position.center.y << "\n";
		std::cout << "Heading: " << heading << "\n \n";

		if (ball->position.center.x == x + (heading * ball1.x) && ball->position.center.y == y + (ball1.y + gravity))
		{
			std::cout << "WYNIK TESTU NR " << i + 1 << " POZYTYWNY\n";
			passedTests_b1++;
		}
		else
		{
			std::cout << "WYNIK TESTU NR " << i + 1 << " NEGATYWNY\n";
		}

		delete ball;
	}

	//
	// LOSOWO GENEROWANE TESTY DLA KULEK TYPU 2
	//

	std::cout << "LOSOWO GENEROWANE TESTY DLA KULEK TYPU 2 \n";
	for (int i = 0; i < num_tests; i++)
	{
		//GENEROWANIE LICZB "LOSOWYCH"
		x = float((rand() % 100000) + 1) / 100;
		y = float((rand() % 100000) + 1) / 100;

		randHeading = (rand() % 2) + 1;
		if (randHeading == 2)
		{
			heading = 1;
		}
		if (randHeading == 1)
		{
			heading = -1;
		}

		//TWORZENIE TESTOWANEGO OBIEKTU I TRESC TESTU
		Sprite* ball = Enemy::create(game, x, y, Enemy::Kind::BALL2, heading);

		std::cout << "\n-----WARTOSCI DOSTARCZONE-----\n";
		std::cout << "Position X: " << ball->position.center.x << "\n";
		std::cout << "Position Y: " << ball->position.center.y << "\n";
		std::cout << "Heading: " << heading << "\n";


		ball->Move();


		std::cout << "\n-----WARTOSCI ODEBRANE-----\n";
		std::cout << "Position X: " << ball->position.center.x << "\n";
		std::cout << "Position Y: " << ball->position.center.y << "\n";
		std::cout << "Heading: " << heading << "\n \n";

		if (ball->position.center.x == x + (heading * ball2.x) && ball->position.center.y == y + (ball2.y + gravity))
		{
			std::cout << "WYNIK TESTU NR " << i + 1 << " POZYTYWNY\n";
			passedTests_b2++;
		}
		else
		{
			std::cout << "WYNIK TESTU NR " << i + 1 << " NEGATYWNY\n";
		}

		delete ball;
	}

	//
	// LOSOWO GENEROWANE TESTY DLA KULKI TYPU 3
	//

	std::cout << "LOSOWO GENEROWANE TESTY DLA KULEK TYPU 3 \n";
	for (int i = 0; i < num_tests; i++)
	{
		//GENEROWANIE LICZB "LOSOWYCH"
		x = float((rand() % 100000) + 1) / 100;
		y = float((rand() % 100000) + 1) / 100;

		randHeading = (rand() % 2) + 1;
		if (randHeading == 2)
		{
			heading = 1;
		}
		if (randHeading == 1)
		{
			heading = -1;
		}

		//TWORZENIE TESTOWANEGO OBIEKTU I TRESC TESTU
		Sprite* ball = Enemy::create(game, x, y, Enemy::Kind::BALL3, heading);

		std::cout << "\n-----WARTOSCI DOSTARCZONE-----\n";
		std::cout << "Position X: " << ball->position.center.x << "\n";
		std::cout << "Position Y: " << ball->position.center.y << "\n";
		std::cout << "Heading: " << heading << "\n";


		ball->Move();


		std::cout << "\n-----WARTOSCI ODEBRANE-----\n";
		std::cout << "Position X: " << ball->position.center.x << "\n";
		std::cout << "Position Y: " << ball->position.center.y << "\n";
		std::cout << "Heading: " << heading << "\n \n";

		if (ball->position.center.x == x + (heading * ball3.x) && ball->position.center.y == y + (ball3.y + gravity))
		{
			std::cout << "WYNIK TESTU NR " << i + 1 << " POZYTYWNY\n";
			passedTests_b3++;
		}
		else
		{
			std::cout << "WYNIK TESTU NR " << i + 1 << " NEGATYWNY\n";
		}

		delete ball;
	}

	//
	// LOSOWO GENEROWANE TESTY DLA KULKI TYPU 4
	//

	std::cout << "LOSOWO GENEROWANE TESTY DLA KULEK TYPU 4 \n";
	for (int i = 0; i < num_tests; i++)
	{
		//GENEROWANIE LICZB "LOSOWYCH"
		x = float((rand() % 100000) + 1) / 100;
		y = float((rand() % 100000) + 1) / 100;

		randHeading = (rand() % 2) + 1;
		if (randHeading == 2)
		{
			heading = 1;
		}
		if (randHeading == 1)
		{
			heading = -1;
		}

		//TWORZENIE TESTOWANEGO OBIEKTU I TRESC TESTU
		Sprite* ball = Enemy::create(game, x, y, Enemy::Kind::BALL4, heading);

		std::cout << "\n-----WARTOSCI DOSTARCZONE-----\n";
		std::cout << "Position X: " << ball->position.center.x << "\n";
		std::cout << "Position Y: " << ball->position.center.y << "\n";
		std::cout << "Heading: " << heading << "\n";


		ball->Move();


		std::cout << "\n-----WARTOSCI ODEBRANE-----\n";
		std::cout << "Position X: " << ball->position.center.x << "\n";
		std::cout << "Position Y: " << ball->position.center.y << "\n";
		std::cout << "Heading: " << heading << "\n \n";

		if (ball->position.center.x == x + (heading * ball4.x) && ball->position.center.y == y + (ball4.y + gravity))
		{
			std::cout << "WYNIK TESTU NR " << i + 1 << " POZYTYWNY\n";
			passedTests_b4++;
		}
		else
		{
			std::cout << "WYNIK TESTU NR " << i + 1 << " NEGATYWNY\n";
		}
		delete ball;
	}

	TestResult result;
	result.run = num_tests;
	result.passed["DLA KULEK TYPU 1: "] = passedTests_b1;
	result.passed["DLA KULEK TYPU 2: "] = passedTests_b2;
	result.passed["DLA KULEK TYPU 3: "] = passedTests_b3;
	result.passed["DLA KULEK TYPU 4: "] = passedTests_b4;

	return result;
}

TestResult TestMoveCrab(Game* game)
{
	//
	// ZMIENNE POTRZEBNE DLA TESTOW
	//

	srand(time(NULL));

	float x;
	float y;
	float gravity = 0.3f;

	int heading;
	int randHeading;

	Vector2 crab1 = { 1.7f, 0.5 };

	int num_tests = 100;
	int passedTests_c1 = 0;

	//
	// TESTY
	//

	//
	// LOSOWO GENEROWANE TESTY DLA KULEK TYPU 1
	//

	std::cout << "LOSOWO GENEROWANE TESTY DLA KRABOW\n";
	for (int i = 0; i < num_tests; i++)
	{
		//GENEROWANIE LICZB "LOSOWYCH"
		x = float((rand() % 100000) + 1) / 100;
		y = float((rand() % 100000) + 1) / 100;

		randHeading = (rand() % 2) + 1;
		if (randHeading == 2)
		{
			heading = 1;
		}
		if (randHeading == 1)
		{
			heading = -1;
		}

		//TWORZENIE TESTOWANEGO OBIEKTU I TRESC TESTU
		Sprite* crab = new Crab(game, x, y, heading);

		std::cout << "\n-----WARTOSCI DOSTARCZONE-----\n";
		std::cout << "Position X: " << crab->position.rectangle.x << "\n";
		std::cout << "Position Y: " << crab->position.rectangle.y << "\n";
		std::cout << "Heading: " << heading << "\n";

		crab->Move();

		std::cout << "\n-----WARTOSCI ODEBRANE-----\n";
		std::cout << "Position X: " << crab->position.rectangle.x << "\n";
		std::cout << "Position Y: " << crab->position.rectangle.y << "\n";
		std::cout << "Heading: " << heading << "\n \n";

		if (crab->position.rectangle.x == x + (crab1.x * heading) && crab->position.rectangle.y == y + crab1.y)
		{
			std::cout << "WYNIK TESTU NR " << i + 1 << " POZYTYWNY\n";
			passedTests_c1++;
		}
		else
		{
			std::cout << "WYNIK TESTU NR " << i + 1 << " NEGATYWNY\n";
		}
		delete crab;
	}
	TestResult result;
	result.run = num_tests;
	result.passed["DLA KRABA TYPU 1: "] = passedTests_c1;
	
	return result;
}

bool RunTests() {
	Game game;
	TestResult r1 = TestMoveCrab(&game);
	TestResult r2 = TestMoveBall(&game);

	std::cout << "===================================\n";
	std::cout << "WYNIK PRZEPROWADZONYCH TESTOW: \n";
	for (auto& result : r1.passed) {
		std::cout << result.first << result.second << "/" << r1.run << "\n";	
	}
	std::cout << "===================================\n";

	std::cout << "===================================\n";
	std::cout << "WYNIK PRZEPROWADZONYCH TESTOW: \n";
	for (auto& result : r2.passed) {
		std::cout << result.first << result.second << "/" << r2.run << "\n";	
	}
	std::cout << "===================================\n";
	
	return true;
}