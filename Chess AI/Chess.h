
#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <map>
#include <algorithm>

#include "Utils.h"

#define KING 0
#define QUEEN 1
#define BISHOP 2
#define KNIGHT 3
#define ROOK 4
#define PAWN 5

#define WHITE 0
#define BLACK 1


namespace Chess {


    class Piece {

        public:
            int color;
            int type;
            std::string uniqueCode;

            Piece();

            Piece(char fenChr);

            Piece(int pType, int pColor);

    };

    class Board {
        private:
            std::map<Tile, Piece> _board;
            std::vector<Tile> w_kingBeam; // If the white king is in a black piece beam (N, R, Q, B) the beam will be copied here
            std::vector<Tile> b_kingBeam; // If the black king is in a white piece beam (N, R, Q, B) the beam will be copied here
            std::vector<Tile> enpassant;
            bool whiteKingsidecastle = false;
            bool whiteQueensidecastle = false;
            bool blackKingsidecastle = false;
            bool blackQueensidecastle = false;
            uint currentPlayer = 0;
            

        public:
            Board(std::string fenStr);

            bool inBoard(Tile t);

            int addKingBeam(std::vector<Tile> beam, int kingColor);

            std::vector<Tile> getTiles();

            bool inEnpassant(Tile t);

            Piece getPiece(Tile t);

            void movePiece(Tile pos1, Tile pos2);

            std::string toFen();

        
    };


    // Chess engine functions migrated from python to C++
    
    std::vector<Tile> generateLegalMoves(Board& board, Tile pos);

    std::vector<Tile> generateLegalMovesNoSafety(Board& board, Tile pos);

    std::vector<Tile> findBeam(Board& board, Tile pos);

    std::map<Tile, std::vector<Tile>> checkForKingSafety(Board& board, int currentPlayer);

    std::vector<Tile> pawnMoves(Board& board, Tile pos);

    std::vector<Tile> rookMoves(Board& board, Tile pos);

    std::vector<Tile> bishopMoves(Board& board, Tile pos);

    std::vector<Tile> knightMoves(Board& board, Tile pos);

    std::vector<Tile> queenMoves(Board& board, Tile pos);

    std::vector<Tile> kingMoves(Board& board, Tile pos);
}