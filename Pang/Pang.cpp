#include <string>
#include <iostream>

#include "Game.h"
#include "Tests.h"

int main(int argc, const char* argv[]) {
    if (argc > 1) { 
        std::string arg(argv[1]);
        if (arg == "--tests") {
	        return RunTests();
        }
	}
    Game game;
    return game.MainLoop();
}

