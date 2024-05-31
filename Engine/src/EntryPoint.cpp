#include <iostream>
#include <chrono>
#include "tables.h"
#include "Evaluate.h"
#include "Engine.h"

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

#ifdef __DLL
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
#else
int main(void) {
#endif
// 	int ul_reason_for_call = 1;

#ifdef __DLL
	switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
			//std::cout << "Attached (Process)" << std::endl;
			Log::Init();
			initialise_all_databases();
			zobrist::initialise_zobrist_keys();
			break;
        case DLL_THREAD_ATTACH:
			//std::cout << "Attached (Thread)" << std::endl;
			break;
        case DLL_THREAD_DETACH:
			//std::cout << "Detached (Thread)" << std::endl;
			break;
        case DLL_PROCESS_DETACH:
			//std::cout << "Detached (Process)" << std::endl;
			break;
    }
#endif


#ifdef __EXE
	Engine::Application app(true);
	app.UCICommandLoop();
#endif
   
	// Position p;
	// Position::set("r3kb1Q/pp3p1p/2p5/8/2b1q3/Pn6/5PPP/1R4K1 w q", p);


	// MoveList<WHITE> mL(p, Square::h8);
	// Move m = Move(Square::h8, Square::h7, MoveFlags::CAPTURE);
	// const int factor = 10e6;
	// int sc = 0;

	// auto start = std::chrono::high_resolution_clock::now();
	// for (int i = 0; i < factor; ++i) {
	// 	sc = mvv_lva(m, p);
	// }
	// auto stop = std::chrono::high_resolution_clock::now();

	// auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start);
	// std::cout << (float) duration.count() / factor << std::endl;
	// std::cout << (float) ( - start.time_since_epoch().count() + stop.time_since_epoch().count() ) / (1) << std::endl;
	// std::cout << sc << std::endl;

	/*Move m = findBestMove<WHITE>(p, 4);
	std::cout << m << std::endl;*/
	/*const Color PLAYER1 = WHITE;
	const Color PLAYER2 = BLACK;

	while (TRUE) {
		Move m = findBestMove<PLAYER1>(p, 4);
		std::cout << "Engine 0.0 played: " << m << std::endl;
		p.play<PLAYER1>(m);

		std::cout << p.fen() << std::endl;
		if (p.stalemate<PLAYER1>() or p.stalemate<PLAYER2>()) {
			std::cout << "STALEMATE" << std::endl;
			break;
		}

		if (p.checkmate<PLAYER1>() or p.checkmate<PLAYER2>()) {
			std::cout << "CHECKMATE" << std::endl;
			break;
		}

		m = findBestMove<PLAYER2>(p, 4);
		std::cout << "Engine 0.1 played: " << m << std::endl;
		p.play<PLAYER2>(m);


	}*/

	// if (argc < 2) {
	// 	printf("[ERROR] Insufficient arguments!\nUSAGE: engine [PIPE_KEY]");
	// 	exit(EXIT_FAILURE);
	// }

	// auto app = Engine::CreateApplication(argv[1]);

	// app->Run();
	
	// delete app;
	return TRUE;
}
