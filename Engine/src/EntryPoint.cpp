#include <iostream>
#include <chrono>
#include "tables.h"
#include "Evaluate.h"
#include "Engine.h"


#define START_POSITION "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR/ w KQkq"
#define DEBUG_POSITION "rnb2b2/2p1k1p1/1p1q4/3p4/P7/8/P4PPP/RNB1KBNR w KQ - 0 9"


/*
	Table of commands:

	SET <fen_string>							---> Set the board using a FEN string
	MOVE <color> <unint16_t move>				---> Play move <move> as <color>
	UNDO										---> Undo the last move
	MOVEREQ <AI_level> <AI_color>				---> Request a move from the AI at level <AI_level> as <AI_color>
	GENMOVES <piece_square>					    ---> Generate all legal moves for given piece at <piece_square>, if <piece_square> is equal to 100 + <color> then generate all legal moves for <color>
	CHECK <color>								---> Check if side <color> is in check
	CHECKMATE <color>							---> Check if side <color> is in checkmate
	COLOR <color>								---> Return the current side to play
	EXIT										---> Exit the program
*/

int main(int argc, char** argv) {
	initialise_all_databases();
	zobrist::initialise_zobrist_keys();
	Log::Init();

	/*Position p;
	Position::set("8/8/8/8/3B4/2Q5/6K1/3k4 b - -", p);
	std::cout << p << std::endl;*/
	// "8/5p2/1p6/2k5/K5p1/6P1/2q3r1/8 w - - 3 3"

	/*int s = 0;
	
	auto start = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < 10e6; ++i) {
		s = Evaluate(p);
	}
	auto stop = std::chrono::high_resolution_clock::now();

	auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start) / 10e6;
	std::cout << duration.count() << std::endl;*/
	//std::cout << p.stalemate<WHITE>() << std::endl;

	/*Move m = findBestMove<BLACK>(p, 4);
	std::cout << m << std::endl;
	p.play<BLACK>(m);

	m = findBestMove<WHITE>(p, 4);
	std::cout << m << std::endl;
	p.play<WHITE>(m);*/

	if (argc < 2) {
		printf("[ERROR] Insufficient arguments!\nUSAGE: engine [PIPE_KEY]");
		exit(EXIT_FAILURE);
	}

	auto app = Engine::CreateApplication(argv[1]);

	app->Run();
	
	delete app;

	return 0;
}
