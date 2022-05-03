
#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <map>

#define KING 0
#define QUEEN 1
#define BISHOP 2
#define KNIGHT 3
#define ROOK 4
#define PAWN 5

#define WHITE 0
#define BLACK 1

typedef unsigned int uint;
typedef std::pair<uint, uint> Tile;

namespace Chess {


    class Piece {

        public:
            int Color;
            int Type;

            Piece();

            Piece(char fenChr);

            Piece(int pType, int pColor);

    };

    class Board {
        private:
            std::map<Tile, Piece> _board;
            bool whiteKingsidecastle = false;
            bool whiteQueensidecastle = false;
            bool blackKingsidecastle = false;
            bool blackQueensidecastle = false;
            uint currentPlayer = 0;
            

        public:
            Board(std::string fenStr);

            std::vector<Tile> getTiles();

            Piece getPiece(Tile t);

            void movePiece(Tile pos1, Tile pos2);

            std::string toFen();

        
    };
    
    std::vector<Tile> generateLegalMoves(Board board, Tile pos);
}