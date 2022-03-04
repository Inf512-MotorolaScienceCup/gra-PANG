#include <string>
#include <iostream>

#include "Game.h"
#include "Tests.h"

int main(int argc, const char* argv[]) {
    std::cout << "Running..." << argc << argv[0] << std::endl;
    if (argc > 1) { 
        std::string arg(argv[1]);
        if (arg == "--tests") {
	        return RunTests();
        }
	}
    std::cout << "Running..." << std::endl;
    Game game;
    return game.MainLoop();
}

