#include "Chess.h"

namespace Chess {
    
    std::string fen = "kqbnrp";

    Piece::Piece() {
        color = 0;
        type = 1;
    }

    Piece::Piece(char fenChr) : type( fen.find(tolower(fenChr)) ), color( 0 ) {
        if (!isupper(fenChr)) color = 1;
    }

    Piece::Piece(int pType, int pColor) : type(pType), color(pColor) {}







    Board::Board(std::string fenStr) {
        uint row = 0;
        uint col = 0;
        uint counter = 1;
        std::vector<uint> enpst;
        size_t space = fenStr.find(" ");
        for (size_t i = 0; i < fenStr.length(); i++) {
            if (i > space && std::isdigit(fenStr[i])) {
                enpst.emplace_back((int)fenStr[i] - '0');
                if ((counter % 2) == 0) {
                    enpassant.emplace_back(enpst[0], enpst[1]);
                    enpst.clear();
                }
                counter++;
            }
            if (fenStr[i] == '/') { row++; col = 0; } 
            else if (i < space && std::isdigit(fenStr[i]))  { col += (int)fenStr[i] - '0'; }
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
                    if(_board[Tile(row, col)].color)
                    fenStr += fen[_board[Tile(row, col)].type];
                    else
                    fenStr += std::toupper(fen[_board[Tile(row, col)].type]);
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

    bool Board::inBoard(Tile t) {
        if (Board::getPiece(t).color == -1)
            return false;

        return true;
    }

    bool Board::inEnpassant(Tile t) {
        for (auto element : enpassant) {
            if (element.first == t.first && element.second == t.second){
                return true;
            }
        }
        return false;
    }


    std::vector<Tile> generateLegalMoves(Chess::Board& board, Tile pos) {
        

    }

    std::vector<Tile> generateLegalMovesNoSafety(Chess::Board& board, Tile pos) {
        

    }

    std::map<Tile, std::vector<Tile>> checkForKingSafety(Chess::Board& board, uint color) {

    }

    int Board::addKingBeam(std::vector<Tile> beam, int kingColor){
        if (beam.size() > 0) {
            kingColor == WHITE ? w_kingBeam = beam : b_kingBeam = beam;
            std::cout << "color: " << kingColor << std::endl << vectp(beam) << std::endl;
            return 1;
        }
        else {
            return -1;
        }
    }

    std::vector<Tile> pawnMoves(Chess::Board& board, Tile pos) {
        std::vector<Tile> moves;

        int pieceColor = board.getPiece(pos).color;
        int pieceType = board.getPiece(pos).type;
        if (pieceColor == -1 || pieceType != PAWN) return std::vector<Tile>();

        int offset;
        pieceColor == WHITE ? offset = -1 : offset = 1;

        moves.emplace_back(pos.first + offset, pos.second);
        if ((pos.first == 6 || pos.first == 1) && !board.inBoard(Tile(pos.first+2*offset, pos.second)) && !board.inBoard(Tile(pos.first+offset, pos.second)))
            moves.emplace_back(pos.first+2*offset, pos.second);
        for (auto i : {1, -1}) {
            if (board.inBoard(Tile(pos.first, pos.second+i)) && board.getPiece(Tile(pos.first, pos.second+i)).type == PAWN && board.getPiece(Tile(pos.first, pos.second+i)).color != pieceColor && board.inEnpassant(Tile(pos.first, pos.second+i))) {
                moves.emplace_back(pos.first+offset, pos.second+i);
            }
            if (board.inBoard(Tile(pos.first+offset, pos.second+i)) && board.getPiece(Tile(pos.first+offset, pos.second+i)).color != pieceColor)
                moves.emplace_back(pos.first+offset, pos.second+i);
        if (board.inBoard(Tile(pos.first+offset, pos.second)))
            moves.erase(std::remove(moves.begin(), moves.end(), Tile(pos.first+offset, pos.second)), moves.end());
        }
        return moves;
    }

    std::vector<Tile> knightMoves(Chess::Board& board, Tile pos) {
        std::vector<Tile> moves;

        int pieceColor = board.getPiece(pos).color;
        int pieceType = board.getPiece(pos).type;
        if (pieceColor == -1 || pieceType != KNIGHT) return std::vector<Tile>();

        for (Tile move : {Tile(pos.first+1, pos.second+2), Tile(pos.first+1, pos.second-2), Tile(pos.first+2, pos.second+1), Tile(pos.first+2, pos.second-1), Tile(pos.first-1, pos.second+2), Tile(pos.first-1, pos.second-2), Tile(pos.first-2, pos.second+1), Tile(pos.first-2, pos.second-1)}) {
            if (!(board.getPiece(move).color == pieceColor) && move.first <= 8 && move.first >= 0 && move.second <= 8 && move.second >= 0) {
                moves.emplace_back(move.first, move.second);
            }
        }

        return moves;
    }

    std::vector<Tile> rookMoves(Chess::Board& board, Tile pos) {
        std::vector<Tile> moves;

        std::vector<Tile> beam;

        int pieceColor = board.getPiece(pos).color;
        int pieceType = board.getPiece(pos).type;
        if (pieceColor == -1 || pieceType != ROOK) return std::vector<Tile>();

        for (int i = 1; i < 8-pos.first; i++) {
            if (board.inBoard(Tile(pos.first+i, pos.second))) {
                if (board.getPiece(Tile(pos.first+i, pos.second)).color != pieceColor && board.getPiece(Tile(pos.first+i, pos.second)).type == KING) {
                    beam.emplace_back(pos.first+i, pos.second);
                    pieceColor == WHITE ? board.addKingBeam(beam, BLACK) : board.addKingBeam(beam, WHITE);
                    break;
                }
                if (board.getPiece(Tile(pos.first+i, pos.second)).color != pieceColor) {
                    beam.emplace_back(pos.first+i, pos.second);
                    break;
                }
                break;
            }
            beam.emplace_back(pos.first+i, pos.second);
        }
        moves.insert(moves.end(), beam.begin(), beam.end());
        beam.clear();
        for (int i = pos.first-1; i > -1; i--) {
            if (board.inBoard(Tile(i, pos.second))) {
                if (board.getPiece(Tile(pos.first+i, pos.second)).color != pieceColor && board.getPiece(Tile(pos.first+i, pos.second)).type == KING) {
                    beam.emplace_back(pos.first+i, pos.second);
                    pieceColor == WHITE ? board.addKingBeam(beam, BLACK) : board.addKingBeam(beam, WHITE);
                    break;
                }
                if (board.getPiece(Tile(pos.first+i, pos.second)).color != pieceColor) {
                    beam.emplace_back(pos.first+i, pos.second);
                    break;
                }
                break;
            }
            beam.emplace_back(i, pos.second);
        }
        moves.insert(moves.end(), beam.begin(), beam.end());
        beam.clear();
        for (int i = 1; i < 8-pos.second; i++) {
            if (board.inBoard(Tile(pos.first, pos.second+i))) {
                if (board.getPiece(Tile(pos.first, pos.second+i)).color != pieceColor && board.getPiece(Tile(pos.first, pos.second+i)).type == KING) {
                    beam.emplace_back(pos.first, pos.second+i);
                    pieceColor == WHITE ? board.addKingBeam(beam, BLACK) : board.addKingBeam(beam, WHITE);
                    break;
                }
                if (board.getPiece(Tile(pos.first, pos.second+i)).color != pieceColor) {
                    beam.emplace_back(pos.first, pos.second+i);
                    break;
                }
                break;
            }
            beam.emplace_back(pos.first, pos.second+i);
        }
        moves.insert(moves.end(), beam.begin(), beam.end());
        beam.clear();
        for (int i = pos.second-1; i > -1; i--) {
            if (board.inBoard(Tile(pos.first, i))) {
                if (board.getPiece(Tile(pos.first+i, pos.second)).color != pieceColor && board.getPiece(Tile(pos.first+i, pos.second)).type == KING) {
                    beam.emplace_back(pos.first+i, pos.second);
                    pieceColor == WHITE ? board.addKingBeam(beam, BLACK) : board.addKingBeam(beam, WHITE);
                    break;
                }
                if (board.getPiece(Tile(pos.first+i, pos.second)).color != pieceColor) {
                    beam.emplace_back(pos.first+i, pos.second);
                    break;
                }
                break;
            }
            beam.emplace_back(pos.first, i);
        }
        moves.insert(moves.end(), beam.begin(), beam.end());
        beam.clear();

        return moves;
    }
}