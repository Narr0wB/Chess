#include <iostream>
#include <chrono>
#include "tables.h"
#include "Evaluate.h"


#define START_POSITION "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR/ w KQkq"
#define DEBUG_POSITION "rnb2b2/2p1k1p1/1p1q4/3p4/P7/8/P4PPP/RNB1KBNR w KQ - 0 9"

#define get_bit(Bitboard, square) ((Bitboard & (1ULL << square)) ? 1 : 0)

#define ntos(rank, file) (8*rank + file)

void printBoard(Bitboard bb) {
	for (int rank = 0; rank < 8; rank++) {
		for (int file = 0; file < 8; file++) {
			std::cout << get_bit(bb, ntos(rank, file));
		}
		std::cout << std::endl;
	}
}

template<Color Us>
unsigned long long perft(Position& p, unsigned int depth) {
	int nmoves;
	unsigned long long nodes = 0;

	MoveList<Us> list(p);

	if (depth == 1) return (unsigned long long) list.size();

	for (Move move : list) {
		p.play<Us>(move);
		nodes += perft<~Us>(p, depth - 1);
		p.undo<Us>(move);
	}

	return nodes;
}

//A variant of perft, listing all moves and for each move, the perft of the decremented depth
//It is used solely for debugging
template<Color Us>
void perftdiv(Position& p, unsigned int depth) {
	unsigned long long nodes = 0, pf;

	MoveList<Us> list(p);

	for (Move move : list) {
		std::cout << move;

		p.play<Us>(move);
		pf = perft<~Us>(p, depth - 1);
		std::cout << ": " << pf << " moves\n";
		nodes += pf;
		p.undo<Us>(move);
	}

	std::cout << "\nTotal: " << nodes << " moves\n";
}

void test_perft() {
	Position p;
	Position::set(START_POSITION, p);
	std::cout << p;

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	auto n = perft<WHITE>(p, 6);
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	auto diff = end - begin;

	std::cout << "Nodes: " << n << "\n";
	std::cout << "NPS: "
		<< int(n * 1000000.0 / std::chrono::duration_cast<std::chrono::microseconds>(diff).count())
		<< "\n";
	std::cout << "Time difference = "
		<< std::chrono::duration_cast<std::chrono::microseconds>(diff).count() << " [microseconds]\n";
}

int main(int argc, char* argv[]) {
	//Make sure to initialise all databases before using the library!
	initialise_all_databases();
	zobrist::initialise_zobrist_keys();


	Position p;
	Position::set(argv[1], p);

	p.getCurrentColor() == WHITE ? std::cout << findBestMove<WHITE>(p, 4) << std::endl : std::cout << findBestMove<BLACK>(p, 4) << std::endl;
	std::cout << "Nodes searched: " << mSCalls << std::endl;
	

	/*while (1) {
		std::string fen;
		std::getline(std::cin, fen);
		Position p;
		Position::set(fen, p);
		const Color currentPlayer = p.getCurrentColor();
		std::cout << "Board evaluation: " << Evaluate(p) << std::endl;
		std::cout << "Move: " << findBestMove<BLACK>(p, 4) << std::endl;
		std::cout << "Nodes searched: " << mSCalls << std::endl;
	}*/

	
	return 0;
}
