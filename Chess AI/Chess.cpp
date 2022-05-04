#include "Chess.h"

namespace Chess {
    
    std::string fen = "kqbnrp";

    Piece::Piece() {
        Color = 0;
        Type = 1;
    }

    Piece::Piece(char fenChr) : Type( fen.find(tolower(fenChr)) ), Color( 0 ) {
        if (!isupper(fenChr)) Color = 1;
    }

    Piece::Piece(int pType, int pColor) : Type(pType), Color(pColor) {}







    Board::Board(std::string fenStr) {
        uint row = 0;
        uint col = 0;
        size_t space = fenStr.find(" ");
        for (size_t i = 0; i < fenStr.length(); i++) {
            if (fenStr[i] == '/') { row++; col = 0; } 
            else if (std::isdigit(fenStr[i]))  { col += (int)fenStr[i] - '0'; }
            else if (i < space) 
            {   
                _board.insert( std::pair<Tile, Piece>(Tile(row, col), Piece(fenStr[i])) );
                col++;
            
            if (i > space && fenStr[i] == 0x4B) {
                whiteKingsidecastle = true;
            }
            if (i > space && fenStr[i] == 0x51) {
                whiteQueensidecastle = true;
            }
            if (i > space && fenStr[i] == 0x6B) {
                blackKingsidecastle = true;
            }
            if (i > space && fenStr[i] == 0x71) {
                blackQueensidecastle = true;
            }
            if (std::tolower(fenStr[i]) == 0x62) currentPlayer = 1;
            }
        }
    }

    std::vector<Tile> Board::getTiles() {
        std::vector<Tile> tiles;
        for (std::map<Tile, Piece>::iterator it = _board.begin(); it != _board.end(); it++) {
            tiles.push_back(it->first);
        }
        return tiles;
    }

    void Board::movePiece(Tile pos1, Tile pos2) {
        if (_board.find(pos2) != _board.end()) 
        {
            _board.erase(pos2);
        }

        _board[pos2] = _board[pos1];
        _board.erase(pos1);
    }

    Piece Board::getPiece(Tile t) {
        if (t.first < 8 && t.second < 8 && _board.find(t) != _board.end()) return _board[t];
        else return Piece(-1, -1);
    };

    std::string Board::toFen() {
        std::string fenStr = "";
        uint spaces = 0;
        for (int row = 0; row < 8; row++) {
            for (int col = 0; col < 8; col++) {
                if (_board.find(Tile(row, col)) != _board.end())
                {   
                    if (spaces > 0) fenStr += std::to_string(spaces);
                    spaces = 0;
                    if(_board[Tile(row, col)].Color)
                    fenStr += fen[_board[Tile(row, col)].Type];
                    else
                    fenStr += std::toupper(fen[_board[Tile(row, col)].Type]);
                }
                else {
                    spaces++;
                }
                
            }
            if (spaces > 0) fenStr += std::to_string(spaces);
            spaces = 0;
            if (row != 7)
            fenStr += "/";
        }
        return fenStr;
    }


    std::vector<Tile> Chess::generateLegalMoves(Chess::Board board, Tile pos) {
        

    }

    std::vector<Tile> Chess::generateLegalMovesNoSafety(Chess::Board board, Tile pos) {
        

    }

    std::map<Tile, std::vector<Tile>> Chess::checkForKingSafety(Chess::Board board, uint color) {

    }

    std::vector<Tile> Chess::pawnMoves(Chess::Board board, Tile pos) {
        
    }
}