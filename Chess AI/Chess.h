
#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <map>
#include <algorithm>
#include <bitset>
#include <iostream>

#include "Utils.h"

#define KING 0
#define QUEEN 1
#define BISHOP 2
#define KNIGHT 3
#define ROOK 4
#define PAWN 5

#define WHITE 0
#define BLACK 1

typedef std::vector<std::bitset<8>> Bitboard;

namespace Chess {

    class Piece {

        public:
            short color;
            short type;
            std::string uniqueCode;

            Piece();

            Piece(char fenChr);

            Piece(short pType, short pColor);

    };

    class Board {
        private:
            std::map<Tile, Piece> _board;
            std::vector<Tile> enpassant;
            bool whiteKingsidecastle = false;
            bool whiteQueensidecastle = false;
            bool blackKingsidecastle = false;
            bool blackQueensidecastle = false;
            short currentPlayer = 0;
            

        public:
            Board(std::string fenStr); // Construct a board object given a fen board

            bool inBoard(Tile t); // Check if there is a piece at a given position (t)

            std::vector<Tile> getPiecesPositions(); // Get all the pieces positions

            bool inEnpassant(Tile t); // Check if a piece at a given position (t) is eligible for enpassant (it's first move and it has moved by two tiles)

            Piece getPiece(Tile t); // Get the piece that is at a given position (t)

            void movePiece(Tile pos1, Tile pos2); // Move a piece at a give position (pos1) to a new position (pos2)

            std::string toFen(); // Return the fen equivalent of the current board

            short getCurrentPlayer(); // Get the color of whoever has the right to move

        
    };

    // bitboard functions

    Bitboard kingMask(Tile pos);


    // Chess engine functions migrated from python to C++
    
    std::vector<Tile> generateLegalMoves(Board& board, Tile pos); // Choose based on the type of the piece at a given position (pos) which one of the move-generating functions to call

    bool checkIfInAllLegalMoves(Board& board, Tile move, short color); // Check if a given position (or move) is in the array of possible moves that a side/player can make

    std::vector<Tile> findBeam(Board& board, Tile pos);

    std::vector<Tile> pawnMoves(Board& board, Tile pos);

    std::vector<Tile> rookMoves(Board& board, Tile pos);

    std::vector<Tile> bishopMoves(Board& board, Tile pos);

    std::vector<Tile> knightMoves(Board& board, Tile pos);

    std::vector<Tile> queenMoves(Board& board, Tile pos);

    std::vector<Tile> kingMoves(Board& board, Tile pos);
}