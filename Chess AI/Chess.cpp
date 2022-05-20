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

    Piece::Piece(short pType, short pColor) : type(pType), color(pColor) {}







    Board::Board(std::string fenStr) {
        currentPlayer = 1;
        uint row = 0;
        uint col = 0;
        uint counter = 1;
        std::vector<uint> enpst;
        size_t space = fenStr.find(" ");
        for (size_t i = 0; i < fenStr.length(); i++) {
            if (fenStr[i] == 'w')  {
                currentPlayer = 0;
            }
            if (i > space && std::isdigit(fenStr[i])) {
                enpst.emplace_back((short)fenStr[i] - '0');
                if ((counter % 2) == 0) {
                    enpassant.emplace_back(enpst[0], enpst[1]);
                    enpst.clear();
                }
                counter++;
            }
            if (fenStr[i] == '/') { row++; col = 0; } 
            else if (i < space && std::isdigit(fenStr[i]))  { col += (short)fenStr[i] - '0'; }
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
            }
        }
    }

    std::vector<Tile> Board::getPiecesPositions() {
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
        for (short row = 0; row < 8; row++) {
            for (short col = 0; col < 8; col++) {
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

    short Board::getCurrentPlayer() {
        return currentPlayer;
    }

    bool Board::inEnpassant(Tile t) {
        for (Tile element : enpassant) {
            if (element.first == t.first && element.second == t.second){
                return true;
            }
        }
        return false;
    }

    Bitboard kingMask(Tile pos) {
        Bitboard kM;
        for (short i = 0; i < 8; i++) {
            kM.emplace_back(std::string("00000000"));
        }

        for (Tile possible : {Tile(pos.first, pos.second+1), Tile(pos.first, pos.second-1), Tile(pos.first+1, pos.second), Tile(pos.first-1, pos.second), Tile(pos.first-1, pos.second-1), Tile(pos.first-1, pos.second+1), Tile(pos.first+1, pos.second+1), Tile(pos.first+1, pos.second-1)}) {
            if (isinbtw(possible.first, 8, -1) && isinbtw(possible.second, 8, -1)) {
                kM[possible.first].flip(7-possible.second);
            }
        }
        
        return kM;
    }


    std::vector<Tile> generateLegalMoves(Chess::Board& board, Tile pos) {
        std::vector<Tile> moves;

        short pieceColor = board.getPiece(pos).color;
        short pieceType = board.getPiece(pos).type;
        if (pieceColor == -1) return std::vector<Tile>();

        switch(pieceType) {
            case PAWN:
            return Chess::pawnMoves(board, pos);
            case KNIGHT:
            return Chess::knightMoves(board, pos);
            case ROOK:
            return Chess::rookMoves(board, pos);
            case BISHOP:
            return Chess::bishopMoves(board, pos);
            case QUEEN:
            return Chess::queenMoves(board, pos);
            case KING:
            return Chess::kingMoves(board, pos);
            default:
            return std::vector<Tile>();
        }

        
        return std::vector<Tile>();
    }   

    std::vector<Tile> generateLegalMovesNoSafety(Chess::Board& board, Tile pos) {
        

    }

    std::map<Tile, std::vector<Tile>> checkForKingSafety(Chess::Board& board, short currentPlayer) {

    }

    bool checkIfInAllLegalMoves(Chess::Board& board, Tile move, short currentPlayer) {
        std::vector<Tile> boardTiles = board.getPiecesPositions();
        for (Tile element : boardTiles) {
            short elemColor = board.getPiece(element).color;
            short elemType = board.getPiece(element).type;
            if (elemColor == currentPlayer && elemType != KING) {
                switch (elemType)
                {
                case PAWN:
                    if (invect(Chess::pawnMoves(board, element), move))
                    return true;
                    break;
                case KNIGHT:
                    if (invect(Chess::knightMoves(board, element), move))
                    return true; 
                    break;
                case ROOK:
                    if (invect(Chess::rookMoves(board, element), move))
                    return true; 
                    break;
                case BISHOP:
                    if (invect(Chess::bishopMoves(board, element), move))
                    return true; 
                    break;
                case QUEEN:
                    if (invect(Chess::queenMoves(board, element), move))
                    return true; 
                    break;
                }
            }
            else if (elemColor == currentPlayer && elemType == KING) {
                if (invect({Tile(element.first, element.second+1), Tile(element.first, element.second-1), Tile(element.first+1, element.second), Tile(element.first-1, element.second), Tile(element.first-1, element.second-1), Tile(element.first-1, element.second+1), Tile(element.first+1, element.second+1), Tile(element.first+1, element.second-1)}, move))
                return true;
            }
        }

        return false;
    }








    // Check for king's safety utilities ------------------------------------------------------------------------------------------------------------------------------------------------------------------

    std::vector<Tile> findBeam(Chess::Board& board, Tile pos) {
        std::vector<Tile> beam;

        short pieceColor = board.getPiece(pos).color;
        short pieceType = board.getPiece(pos).type;

        switch (pieceType) {
            case ROOK: {
            for (short i = 1; i < 8-pos.first; i++) {
                if (board.inBoard(Tile(pos.first+i, pos.second))) {
                    if (board.getPiece(Tile(pos.first+i, pos.second)).color != pieceColor && board.getPiece(Tile(pos.first+i, pos.second)).type == KING) {
                        beam.emplace_back(pos.first+i, pos.second);
                        // board.addKingBeam(beam, pieceColor); --> gotta see whats more efficient
                        return beam;
                    }
                    if (board.getPiece(Tile(pos.first+i, pos.second)).color != pieceColor) {
                        beam.emplace_back(pos.first+i, pos.second);
                        break;
                    }
                    break;
                }
                beam.emplace_back(pos.first+i, pos.second);
            }
            beam.clear();
            for (short i = pos.first-1; i > -1; i--) {
                if (board.inBoard(Tile(i, pos.second))) {
                    if (board.getPiece(Tile(pos.first+i, pos.second)).color != pieceColor && board.getPiece(Tile(pos.first+i, pos.second)).type == KING) {
                        beam.emplace_back(pos.first+i, pos.second);
                        // board.addKingBeam(beam, pieceColor); --> gotta see whats more efficient
                        return beam;
                    }
                    if (board.getPiece(Tile(pos.first+i, pos.second)).color != pieceColor) {
                        beam.emplace_back(pos.first+i, pos.second);
                        break;
                    }
                    break;
                }
                beam.emplace_back(i, pos.second);
            }
            beam.clear();
            for (short i = 1; i < 8-pos.second; i++) {
                if (board.inBoard(Tile(pos.first, pos.second+i))) {
                    if (board.getPiece(Tile(pos.first, pos.second+i)).color != pieceColor && board.getPiece(Tile(pos.first, pos.second+i)).type == KING) {
                        beam.emplace_back(pos.first, pos.second+i);
                        // board.addKingBeam(beam, pieceColor); --> gotta see whats more efficient
                        return beam;
                    }
                    if (board.getPiece(Tile(pos.first, pos.second+i)).color != pieceColor) {
                        beam.emplace_back(pos.first, pos.second+i);
                        break;
                    }
                    break;
                }
                beam.emplace_back(pos.first, pos.second+i);
            }
            beam.clear();
            for (short i = pos.second-1; i > -1; i--) {
                if (board.inBoard(Tile(pos.first, i))) {
                    if (board.getPiece(Tile(pos.first+i, pos.second)).color != pieceColor && board.getPiece(Tile(pos.first+i, pos.second)).type == KING) {
                        beam.emplace_back(pos.first+i, pos.second);
                        // board.addKingBeam(beam, pieceColor); --> gotta see whats more efficient
                        return beam;
                    }
                    if (board.getPiece(Tile(pos.first+i, pos.second)).color != pieceColor) {
                        beam.emplace_back(pos.first+i, pos.second);
                        break;
                    }
                    break;
                }
                beam.emplace_back(pos.first, i);
            }
            beam.clear();
            break;
            }
            case BISHOP: {
            for (short i = 1; i < std::min(8-pos.first, 8-pos.second); i++) {
                if (board.inBoard(Tile(pos.first+i, pos.second+i)) && board.getPiece(Tile(pos.first+i, pos.second+i)).color != pieceColor && board.getPiece(Tile(pos.first+i, pos.second+i)).type == KING) {
                    beam.emplace_back(pos.first+i, pos.second+i);
                    return beam;
                }
                else if (board.inBoard(Tile(pos.first+i, pos.second+i))) {
                    break;
                }
                beam.emplace_back(pos.first+i, pos.second+i);
            }
            beam.clear();
            for (short i = 1; i < std::min(pos.first, pos.second)+1; i++) {
                if (board.inBoard(Tile(pos.first-i, pos.second-i)) && board.getPiece(Tile(pos.first-i, pos.second-i)).color != pieceColor && board.getPiece(Tile(pos.first-i, pos.second-i)).type == KING) {
                    beam.emplace_back(pos.first-i, pos.second-i);
                    return beam;
                }
                else if (board.inBoard(Tile(pos.first-i, pos.second-i))) {
                    break;
                }
                beam.emplace_back(pos.first-i, pos.second-i);
            }
            beam.clear();
            for (short i = 1; i < std::min(pos.first+1, 8-pos.second); i++) {
                if (board.inBoard(Tile(pos.first-i, pos.second+i)) && board.getPiece(Tile(pos.first-i, pos.second+i)).color != pieceColor && board.getPiece(Tile(pos.first-i, pos.second+i)).type == KING) {
                    beam.emplace_back(pos.first-i, pos.second+i);
                    return beam;
                }
                else if (board.inBoard(Tile(pos.first-i, pos.second+i))) {
                    break;
                }
                beam.emplace_back(pos.first-i, pos.second+i);
            }
            beam.clear();
            for (short i = 1; i < std::min(8-pos.first, pos.second+1); i++) { 
                if (board.inBoard(Tile(pos.first+i, pos.second-i)) && board.getPiece(Tile(pos.first+i, pos.second-i)).color != pieceColor && board.getPiece(Tile(pos.first+i, pos.second-i)).type == KING) {
                    beam.emplace_back(pos.first+i, pos.second-i);
                    return beam;
                }
                else if (board.inBoard(Tile(pos.first+i, pos.second-i))) {
                    break;
                }
                beam.emplace_back(pos.first+i, pos.second-i);
            }
            beam.clear();
            }
            break;
            case QUEEN: {
            for (short i = 1; i < 8-pos.first; i++) {
                if (board.inBoard(Tile(pos.first+i, pos.second))) {
                    if (board.getPiece(Tile(pos.first+i, pos.second)).color != pieceColor && board.getPiece(Tile(pos.first+i, pos.second)).type == KING) {
                        beam.emplace_back(pos.first+i, pos.second);
                        // board.addKingBeam(beam, pieceColor); --> gotta see whats more efficient
                        return beam;
                    }
                    if (board.getPiece(Tile(pos.first+i, pos.second)).color != pieceColor) {
                        beam.emplace_back(pos.first+i, pos.second);
                        break;
                    }
                    break;
                }
                beam.emplace_back(pos.first+i, pos.second);
            }
            beam.clear();
            for (short i = pos.first-1; i > -1; i--) {
                if (board.inBoard(Tile(i, pos.second))) {
                    if (board.getPiece(Tile(pos.first+i, pos.second)).color != pieceColor && board.getPiece(Tile(pos.first+i, pos.second)).type == KING) {
                        beam.emplace_back(pos.first+i, pos.second);
                        // board.addKingBeam(beam, pieceColor); --> gotta see whats more efficient
                        return beam;
                    }
                    if (board.getPiece(Tile(pos.first+i, pos.second)).color != pieceColor) {
                        beam.emplace_back(pos.first+i, pos.second);
                        break;
                    }
                    break;
                }
                beam.emplace_back(i, pos.second);
            }
            beam.clear();
            for (short i = 1; i < 8-pos.second; i++) {
                if (board.inBoard(Tile(pos.first, pos.second+i))) {
                    if (board.getPiece(Tile(pos.first, pos.second+i)).color != pieceColor && board.getPiece(Tile(pos.first, pos.second+i)).type == KING) {
                        beam.emplace_back(pos.first, pos.second+i);
                        // board.addKingBeam(beam, pieceColor); --> gotta see whats more efficient
                        return beam;
                    }
                    if (board.getPiece(Tile(pos.first, pos.second+i)).color != pieceColor) {
                        beam.emplace_back(pos.first, pos.second+i);
                        break;
                    }
                    break;
                }
                beam.emplace_back(pos.first, pos.second+i);
            }
            beam.clear();
            for (short i = pos.second-1; i > -1; i--) {
                if (board.inBoard(Tile(pos.first, i))) {
                    if (board.getPiece(Tile(pos.first+i, pos.second)).color != pieceColor && board.getPiece(Tile(pos.first+i, pos.second)).type == KING) {
                        beam.emplace_back(pos.first+i, pos.second);
                        // board.addKingBeam(beam, pieceColor); --> gotta see whats more efficient
                        return beam;
                    }
                    if (board.getPiece(Tile(pos.first+i, pos.second)).color != pieceColor) {
                        beam.emplace_back(pos.first+i, pos.second);
                        break;
                    }
                    break;
                }
                beam.emplace_back(pos.first, i);
            }
            beam.clear();
            for (short i = 1; i < std::min(8-pos.first, 8-pos.second); i++) {
                if (board.inBoard(Tile(pos.first+i, pos.second+i)) && board.getPiece(Tile(pos.first+i, pos.second+i)).color != pieceColor && board.getPiece(Tile(pos.first+i, pos.second+i)).type == KING) {
                    beam.emplace_back(pos.first+i, pos.second+i);
                    return beam;
                }
                else if (board.inBoard(Tile(pos.first+i, pos.second+i))) {
                    break;
                }
                beam.emplace_back(pos.first+i, pos.second+i);
            }
            beam.clear();
            for (short i = 1; i < std::min(pos.first, pos.second)+1; i++) {
                if (board.inBoard(Tile(pos.first-i, pos.second-i)) && board.getPiece(Tile(pos.first-i, pos.second-i)).color != pieceColor && board.getPiece(Tile(pos.first-i, pos.second-i)).type == KING) {
                    beam.emplace_back(pos.first-i, pos.second-i);
                    return beam;
                }
                else if (board.inBoard(Tile(pos.first-i, pos.second-i))) {
                    break;
                }
                beam.emplace_back(pos.first-i, pos.second-i);
            }
            beam.clear();
            for (short i = 1; i < std::min(pos.first+1, 8-pos.second); i++) {
                if (board.inBoard(Tile(pos.first-i, pos.second+i)) && board.getPiece(Tile(pos.first-i, pos.second+i)).color != pieceColor && board.getPiece(Tile(pos.first-i, pos.second+i)).type == KING) {
                    beam.emplace_back(pos.first-i, pos.second+i);
                    return beam;
                }
                else if (board.inBoard(Tile(pos.first-i, pos.second+i))) {
                    break;
                }
                beam.emplace_back(pos.first-i, pos.second+i);
            }
            beam.clear();
            for (short i = 1; i < std::min(8-pos.first, pos.second+1); i++) { 
                if (board.inBoard(Tile(pos.first+i, pos.second-i)) && board.getPiece(Tile(pos.first+i, pos.second-i)).color != pieceColor && board.getPiece(Tile(pos.first+i, pos.second-i)).type == KING) {
                    beam.emplace_back(pos.first+i, pos.second-i);
                    return beam;
                }
                else if (board.inBoard(Tile(pos.first+i, pos.second-i))) {
                    break;
                }
                beam.emplace_back(pos.first+i, pos.second-i);
            }
            beam.clear();
            }
            }

            return std::vector<Tile>();
        }


    





    // Single piece move generators ---------------------------------------------------------------------------------------------------------------------------------------

    std::vector<Tile> pawnMoves(Chess::Board& board, Tile pos) {
        std::vector<Tile> moves;

        short pieceColor = board.getPiece(pos).color;
        short pieceType = board.getPiece(pos).type;
        if (pieceColor == -1 || pieceType != PAWN) return std::vector<Tile>();

        short offset;
        pieceColor == WHITE ? offset = -1 : offset = 1;

        moves.emplace_back(pos.first + offset, pos.second);
        if ((pos.first == 6 || pos.first == 1) && !board.inBoard(Tile(pos.first+2*offset, pos.second)) && !board.inBoard(Tile(pos.first+offset, pos.second)))
            moves.emplace_back(pos.first+2*offset, pos.second);
        for (short i : {1, -1}) {
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

        short pieceColor = board.getPiece(pos).color;
        short pieceType = board.getPiece(pos).type;
        if (pieceColor == -1 || pieceType != KNIGHT) return std::vector<Tile>();

        for (Tile move : {Tile(pos.first+1, pos.second+2), Tile(pos.first+1, pos.second-2), Tile(pos.first+2, pos.second+1), Tile(pos.first+2, pos.second-1), Tile(pos.first-1, pos.second+2), Tile(pos.first-1, pos.second-2), Tile(pos.first-2, pos.second+1), Tile(pos.first-2, pos.second-1)}) {
            if (!(board.getPiece(move).color == pieceColor) && isinbtw(move.first, 8, -1) && isinbtw(move.second, 8, -1)) {
                moves.emplace_back(move.first, move.second);
            }
        }

        return moves;
    }

    std::vector<Tile> rookMoves(Chess::Board& board, Tile pos) {
        std::vector<Tile> moves;

        short pieceColor = board.getPiece(pos).color;
        short pieceType = board.getPiece(pos).type;
        if (pieceColor == -1 || pieceType != ROOK) return std::vector<Tile>();


        for (short i = 1; i < 8-pos.first; i++) {
            if (board.inBoard(Tile(pos.first+i, pos.second))) {
                if (board.getPiece(Tile(pos.first+i, pos.second)).color != pieceColor) {
                    moves.emplace_back(pos.first+i, pos.second);
                }
                break;
            }
            moves.emplace_back(pos.first+i, pos.second);
        }
        for (short i = pos.first-1; i > -1; i--) {
            if (board.inBoard(Tile(i, pos.second))) {
                if (board.getPiece(Tile(pos.first+i, pos.second)).color != pieceColor) {
                    moves.emplace_back(pos.first+i, pos.second);
                }
                break;
            }
            moves.emplace_back(i, pos.second);
        }
        for (short i = 1; i < 8-pos.second; i++) {
            if (board.inBoard(Tile(pos.first, pos.second+i))) {
                if (board.getPiece(Tile(pos.first, pos.second+i)).color != pieceColor) {
                    moves.emplace_back(pos.first, pos.second+i);
                }
                break;
            }
            moves.emplace_back(pos.first, pos.second+i);
        }
        for (short i = pos.second-1; i > -1; i--) {
            if (board.inBoard(Tile(pos.first, i))) {
                if (board.getPiece(Tile(pos.first, i)).color != pieceColor) {
                    moves.emplace_back(pos.first, i);
                    break;
                }
                break;
            }
            moves.emplace_back(pos.first, i);
        }

        return moves;
    }

    std::vector<Tile> bishopMoves(Chess::Board& board, Tile pos) {
        std::vector<Tile> moves;

        short pieceColor = board.getPiece(pos).color;
        short pieceType = board.getPiece(pos).type;
        if (pieceColor == -1 || pieceType != BISHOP) return std::vector<Tile>();

        for (short i = 1; i < std::min(8-pos.first, 8-pos.second); i++) {
            if (board.inBoard(Tile(pos.first+i, pos.second+i)) && board.getPiece(Tile(pos.first+i, pos.second+i)).color != pieceColor) {
                moves.emplace_back(pos.first+i, pos.second+i);
                break;
            }
            else if (board.inBoard(Tile(pos.first+i, pos.second+i))) {
                break;
            }
            moves.emplace_back(pos.first+i, pos.second+i);
        }
        for (short i = 1; i < std::min(pos.first, pos.second)+1; i++) {
            if (board.inBoard(Tile(pos.first-i, pos.second-i)) && board.getPiece(Tile(pos.first-i, pos.second-i)).color != pieceColor) {
                moves.emplace_back(pos.first-i, pos.second-i);
                break;
            }
            else if (board.inBoard(Tile(pos.first-i, pos.second-i))) {
                break;
            }
            moves.emplace_back(pos.first-i, pos.second-i);
        }
        for (short i = 1; i < std::min(pos.first+1, 8-pos.second); i++) {
            if (board.inBoard(Tile(pos.first-i, pos.second+i)) && board.getPiece(Tile(pos.first-i, pos.second+i)).color != pieceColor) {
                moves.emplace_back(pos.first-i, pos.second+i);
                break;
            }
            else if (board.inBoard(Tile(pos.first-i, pos.second+i))) {
                break;
            }
            moves.emplace_back(pos.first-i, pos.second+i);
        }
        for (short i = 1; i < std::min(8-pos.first, pos.second+1); i++) { 
            if (board.inBoard(Tile(pos.first+i, pos.second-i)) && board.getPiece(Tile(pos.first+i, pos.second-i)).color != pieceColor) {
                moves.emplace_back(pos.first+i, pos.second-i);
                break;
            }
            else if (board.inBoard(Tile(pos.first+i, pos.second-i))) {
                break;
            }
            moves.emplace_back(pos.first+i, pos.second-i);
        }

        return moves;
    }

    std::vector<Tile> queenMoves(Chess::Board& board, Tile pos) {
        std::vector<Tile> moves;

        short pieceColor = board.getPiece(pos).color;
        short pieceType = board.getPiece(pos).type;
        if (pieceColor == -1 || pieceType != QUEEN) return std::vector<Tile>();

        for (short i = 1; i < 8-pos.first; i++) {
            if (board.inBoard(Tile(pos.first+i, pos.second))) {
                if (board.getPiece(Tile(pos.first+i, pos.second)).color != pieceColor) {
                    moves.emplace_back(pos.first+i, pos.second);
                }
                break;
            }
            moves.emplace_back(pos.first+i, pos.second);
        }
        for (short i = pos.first-1; i > -1; i--) {
            if (board.inBoard(Tile(i, pos.second))) {
                if (board.getPiece(Tile(pos.first+i, pos.second)).color != pieceColor) {
                    moves.emplace_back(pos.first+i, pos.second);
                }
                break;
            }
            moves.emplace_back(i, pos.second);
        }
        for (short i = 1; i < 8-pos.second; i++) {
            if (board.inBoard(Tile(pos.first, pos.second+i))) {
                if (board.getPiece(Tile(pos.first, pos.second+i)).color != pieceColor) {
                    moves.emplace_back(pos.first, pos.second+i);
                }
                break;
            }
            moves.emplace_back(pos.first, pos.second+i);
        }
        for (short i = pos.second-1; i > -1; i--) {
            if (board.inBoard(Tile(pos.first, i))) {
                if (board.getPiece(Tile(pos.first, i)).color != pieceColor) {
                    moves.emplace_back(pos.first, i);
                    break;
                }
                break;
            }
            moves.emplace_back(pos.first, i);
        }
        for (short i = 1; i < std::min(8-pos.first, 8-pos.second); i++) {
            if (board.inBoard(Tile(pos.first+i, pos.second+i)) && board.getPiece(Tile(pos.first+i, pos.second+i)).color != pieceColor) {
                moves.emplace_back(pos.first+i, pos.second+i);
                break;
            }
            else if (board.inBoard(Tile(pos.first+i, pos.second+i))) {
                break;
            }
            moves.emplace_back(pos.first+i, pos.second+i);
        }
        for (short i = 1; i < std::min(pos.first, pos.second)+1; i++) {
            if (board.inBoard(Tile(pos.first-i, pos.second-i)) && board.getPiece(Tile(pos.first-i, pos.second-i)).color != pieceColor) {
                moves.emplace_back(pos.first-i, pos.second-i);
                break;
            }
            else if (board.inBoard(Tile(pos.first-i, pos.second-i))) {
                break;
            }
            moves.emplace_back(pos.first-i, pos.second-i);
        }
        for (short i = 1; i < std::min(pos.first+1, 8-pos.second); i++) {
            if (board.inBoard(Tile(pos.first-i, pos.second+i)) && board.getPiece(Tile(pos.first-i, pos.second+i)).color != pieceColor) {
                moves.emplace_back(pos.first-i, pos.second+i);
                break;
            }
            else if (board.inBoard(Tile(pos.first-i, pos.second+i))) {
                break;
            }
            moves.emplace_back(pos.first-i, pos.second+i);
        }
        for (short i = 1; i < std::min(8-pos.first, pos.second+1); i++) { 
            if (board.inBoard(Tile(pos.first+i, pos.second-i)) && board.getPiece(Tile(pos.first+i, pos.second-i)).color != pieceColor) {
                moves.emplace_back(pos.first+i, pos.second-i);
                break;
            }
            else if (board.inBoard(Tile(pos.first+i, pos.second-i))) {
                break;
            }
            moves.emplace_back(pos.first+i, pos.second-i);
        }

        return moves;
    }

    std::vector<Tile> kingMoves(Board& board, Tile pos) {
        std::vector<Tile> moves;

        short pieceColor = board.getPiece(pos).color;
        short pieceType = board.getPiece(pos).type;
        if (pieceColor == -1 || pieceType != KING) return std::vector<Tile>();

        for (Tile move : {Tile(pos.first, pos.second+1), Tile(pos.first, pos.second-1), Tile(pos.first+1, pos.second), Tile(pos.first-1, pos.second), Tile(pos.first-1, pos.second-1), Tile(pos.first-1, pos.second+1), Tile(pos.first+1, pos.second+1), Tile(pos.first+1, pos.second-1)}) {
            if (!checkIfInAllLegalMoves(board, move, !pieceColor)) {
                if (board.getPiece(move).color != pieceColor && isinbtw(move.first, 8, -1) && isinbtw(move.second, 8, -1))
                moves.emplace_back(move.first, move.second);
            }
        }

        return moves;
        // TODO: add king and queen-side castle
    }
}