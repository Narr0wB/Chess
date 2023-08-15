#include <iostream>
#include <chrono>
#include "tables.h"
#include "Evaluate.h"
#include "Engine.h"


#define START_POSITION "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR/ w KQkq"
#define DEBUG_POSITION "rnb2b2/2p1k1p1/1p1q4/3p4/P7/8/P4PPP/RNB1KBNR w KQ - 0 9"


/*
	Table of commands:

	SET <fen_string>							---> Set the board to the fen string board equivalent
	MOVE <color> <unint16_t move>				---> Play move "fXtX" as "color"
	UNDO										---> Undo the last move
	MOVEREQ <AI_level> <AI_color>				---> Request a move from the AI at level "AI_level" as "AI_color"
	GENMOVES <piece_position>					---> Generate all legal moves for given piece at "piece_position", if piece_position is equal to ALLX then generate all legal moves for X side
	INCHECK <color>								---> Check if side "color" is in check
	EXIT										---> Exit the program
*/

int main(void) {
	initialise_all_databases();
	zobrist::initialise_zobrist_keys();

	auto app = Engine::CreateApplication();

	app->Run();
	
	delete app;
	return 0;
}
