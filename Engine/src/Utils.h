
#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <string>
#include <chrono>
#include <sstream>
#include <iostream>
#include <algorithm>

#define isinbtw(x, l1, l2) x > l2 && x < l1

typedef unsigned short uint;
typedef std::pair<short, short> Tile;

std::string vectp(std::vector<Tile> vec); // vector print, returns a string that can be printed that visualizes the contents of the vector | <Tile> type

bool invect(std::vector<Tile> haystack, Tile needle);

bool is_in_table(std::vector<uint64_t>& table, uint64_t position_hash);

std::string pairp(Tile t);

std::vector<std::string> split(std::string string, char token);

//#define START_POSITION "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR/ w KQkq"
//#define DEBUG_POSITION "rnb2b2/2p1k1p1/1p1q4/3p4/P7/8/P4PPP/RNB1KBNR w KQ - 0 9"
//
//#define get_bit(Bitboard, square) ((Bitboard & (1ULL << square)) ? 1 : 0)
//
//#define ntos(rank, file) (8*rank + file)
//
//void printBoard(Bitboard bb) {
//	for (int rank = 0; rank < 8; rank++) {
//		for (int file = 0; file < 8; file++) {
//			std::cout << get_bit(bb, ntos(rank, file));
//		}
//		std::cout << std::endl;
//	}
//}
//
//template<Color Us>
//unsigned long long perft(Position& p, unsigned int depth) {
//	int nmoves;
//	unsigned long long nodes = 0;
//
//	MoveList<Us> list(p);
//
//	if (depth == 1) return (unsigned long long) list.size();
//
//	for (Move move : list) {
//		p.play<Us>(move);
//		nodes += perft<~Us>(p, depth - 1);
//		p.undo<Us>(move);
//	}
//
//	return nodes;
//}
//
////A variant of perft, listing all moves and for each move, the perft of the decremented depth
////It is used solely for debugging
//template<Color Us>
//void perftdiv(Position& p, unsigned int depth) {
//	unsigned long long nodes = 0, pf;
//
//	MoveList<Us> list(p);
//
//	for (Move move : list) {
//		std::cout << move;
//
//		p.play<Us>(move);
//		pf = perft<~Us>(p, depth - 1);
//		std::cout << ": " << pf << " moves\n";
//		nodes += pf;
//		p.undo<Us>(move);
//	}
//
//	std::cout << "\nTotal: " << nodes << " moves\n";
//}

//void test_perft() {
//	Position p;
//	Position::set(START_POSITION, p);
//	std::cout << p;
//
//	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
//	auto n = perft<WHITE>(p, 6);
//	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
//	auto diff = end - begin;
//
//	std::cout << "Nodes: " << n << "\n";
//	std::cout << "NPS: "
//		<< int(n * 1000000.0 / std::chrono::duration_cast<std::chrono::microseconds>(diff).count())
//		<< "\n";
//	std::cout << "Time difference = "
//		<< std::chrono::duration_cast<std::chrono::microseconds>(diff).count() << " [microseconds]\n";
//}

#endif // UTILS_H