
#ifndef EVALUATE_H
#define EVALUATE_H

#include <iostream>
#include "position.h"
#include "types.h"
#include "Utils.h"

#define INFINITY 2000000000

#define NO_HASH_ENTRY 0x46464646

#define FLAG_EMPTY 0
#define FLAG_EXACT 1
#define FLAG_ALPHA 2
#define FLAG_BETA 3

typedef uint8_t TranspositionFlags;

using namespace std::chrono_literals;

struct Transposition {
    uint64_t hash;
    TranspositionFlags flags;

    int score;
    int depth;
    Move best;
};

class TranspositionTable {
private:
    Transposition* m_DataArray;
    uint32_t m_Capacity;
    const uint8_t m_BucketSize = 1;

    uint32_t m_Size;
public:
    TranspositionTable(uint32_t capacity) : m_Capacity(capacity), m_Size(0) {
        m_DataArray = new Transposition[capacity * m_BucketSize];
        memset(m_DataArray, 0, sizeof(Transposition) * m_Capacity * m_BucketSize);
    }

    ~TranspositionTable() {
        delete m_DataArray;
    }

    void push_position(Transposition t);
    int probe_hash(uint64_t hash, int alpha, int beta, int depth);

    inline uint32_t size() { return m_Size; };
};

static TranspositionTable table(0x4000000);

static int mSCalls = 0;
static std::chrono::system_clock::time_point start_time;

const int black_pawn_table[64] = {  0,  0,  0,  0,  0,  0,  0,  0,
                                   50, 50, 50, 50, 50, 50, 50, 50,
                                   10, 10, 20, 30, 30, 20, 10, 10,
                                    5,  5, 10, 25, 25, 10,  5,  5,
                                    0,  0,  0, 20, 20,  0,  0,  0,
                                    5, -5,-10,  0,  0,-10, -5,  5,
                                    5, 10, 10,-20,-20, 10, 10,  5,
                                    0,  0,  0,  0,  0,  0,  0,  0 };

const int white_pawn_table[64] = { 0,  0,  0,  0,  0,  0,  0,  0,
                                   5, 10, 10,-20,-20, 10, 10,  5,
                                   5, -5,-10,  0,  0,-10, -5,  5,
                                   0,  0,  0, 20, 20,  0,  0,  0,
                                   5,  5, 10, 25, 25, 10,  5,  5,
                                   10, 10, 20, 30, 30, 20, 10, 10,
                                   50, 50, 50, 50, 50, 50, 50, 50,
                                    0,  0,  0,  0,  0,  0,  0,  0 };

const int knight_table[64] = { -50,-40,-30,-30,-30,-30,-40,-50,
                               -40,-20,  0,  0,  0,  0,-20,-40,
                               -30,  0, 10, 15, 15, 10,  0,-30,
                               -30,  5, 15, 20, 20, 15,  5,-30,
                               -30,  0, 15, 20, 20, 15,  0,-30,
                               -30,  5, 10, 15, 15, 10,  5,-30,
                               -40,-20,  0,  5,  5,  0,-20,-40,
                               -50,-40,-30,-30,-30,-30,-40,-50 };

const int black_bishop_table[64] = { -20,-10,-10,-10,-10,-10,-10,-20,
                                     -10,  0,  0,  0,  0,  0,  0,-10,
                                     -10,  0,  5, 10, 10,  5,  0,-10,
                                     -10,  5,  5, 10, 10,  5,  5,-10,
                                     -10,  0, 10, 10, 10, 10,  0,-10,
                                     -10, 10, 10, 10, 10, 10, 10,-10,
                                     -10,  5,  0,  0,  0,  0,  5,-10,
                                     -20,-10,-10,-10,-10,-10,-10,-20 };

const int white_bishop_table[64] = { -20,-10,-10,-10,-10,-10,-10,-20,
                                     -10,  5,  0,  0,  0,  0,  5,-10,
                                     -10, 10, 10, 10, 10, 10, 10,-10,
                                     -10,  0, 10, 10, 10, 10,  0,-10,
                                     -10,  5,  5, 10, 10,  5,  5,-10,
                                     -10,  0,  5, 10, 10,  5,  0,-10,
                                     -10,  0,  0,  0,  0,  0,  0,-10,
                                     -20,-10,-10,-10,-10,-10,-10,-20 };

const int black_rook_table[64] = { 0,  0,  0,  0,  0,  0,  0,  0,
                                   5, 10, 10, 10, 10, 10, 10,  5,
                                  -5,  0,  0,  0,  0,  0,  0, -5,
                                  -5,  0,  0,  0,  0,  0,  0, -5,
                                  -5,  0,  0,  0,  0,  0,  0, -5,
                                  -5,  0,  0,  0,  0,  0,  0, -5,
                                  -5,  0,  0,  0,  0,  0,  0, -5,
                                   0,  0,  0,  5,  5,  0,  0,  0 };

const int white_rook_table[64] = { 0,  0,  0,  5,  5,  0,  0,  0,
                                 - 5,  0,  0,  0,  0,  0,  0, -5,
                                  -5,  0,  0,  0,  0,  0,  0, -5,
                                  -5,  0,  0,  0,  0,  0,  0, -5,
                                  -5,  0,  0,  0,  0,  0,  0, -5,
                                  -5,  0,  0,  0,  0,  0,  0, -5, 
                                   5, 10, 10, 10, 10, 10, 10,  5, 
                                   0,  0,  0,  0,  0,  0,  0,  0, };

const int black_queen_table[64] = { -20,-10,-10, -5, -5,-10,-10,-20,
                                    -10,  0,  0,  0,  0,  0,  0,-10,
                                    -10,  0,  5,  5,  5,  5,  0,-10,
                                     -5,  0,  5,  5,  5,  5,  0, -5,
                                      0,  0,  5,  5,  5,  5,  0, -5,
                                    -10,  5,  5,  5,  5,  5,  0,-10,
                                    -10,  0,  5,  0,  0,  0,  0,-10,
                                    -20,-10,-10, -5, -5,-10,-10,-20 };

const int white_queen_table[64] = { -20,-10,-10, -5, -5,-10,-10,-20,
                                    -10,  0,  5,  0,  0,  0,  0,-10, 
                                    -10,  5,  5,  5,  5,  5,  0,-10,
                                      0,  0,  5,  5,  5,  5,  0, -5,
                                     -5,  0,  5,  5,  5,  5,  0, -5,
                                    -10,  0,  5,  5,  5,  5,  0,-10,
                                    -10,  0,  0,  0,  0,  0,  0,-10,
                                    -20,-10,-10, -5, -5,-10,-10,-20, };

const int black_king_mg_table[64] = { -30,-40,-40,-50,-50,-40,-40,-30,
                                      -30,-40,-40,-50,-50,-40,-40,-30,
                                      -30,-40,-40,-50,-50,-40,-40,-30,
                                      -30,-40,-40,-50,-50,-40,-40,-30,
                                      -20,-30,-30,-40,-40,-30,-30,-20,
                                      -10,-20,-20,-20,-20,-20,-20,-10,
                                       20, 20,  0,  0,  0,  0, 20, 20,
                                       20, 30, 10,  0,  0, 10, 30, 20 };

const int white_king_mg_table[64] = { 20, 30, 10,  0,  0, 10, 30, 20,
                                      20, 20,  0,  0,  0,  0, 20, 20, 
                                     -10,-20,-20,-20,-20,-20,-20,-10, 
                                     -20,-30,-30,-40,-40,-30,-30,-20, 
                                     -30,-40,-40,-50,-50,-40,-40,-30, 
                                     -30,-40,-40,-50,-50,-40,-40,-30, 
                                     -30,-40,-40,-50,-50,-40,-40,-30, 
                                     -30,-40,-40,-50,-50,-40,-40,-30, };

const int black_king_eg_table[64] = { -50,-40,-30,-20,-20,-30,-40,-50,
                                      -30,-20,-10,  0,  0,-10,-20,-30,
                                      -30,-10, 20, 30, 30, 20,-10,-30,
                                      -30,-10, 30, 40, 40, 30,-10,-30,
                                      -30,-10, 30, 40, 40, 30,-10,-30,
                                      -30,-10, 20, 30, 30, 20,-10,-30,
                                      -30,-30,  0,  0,  0,  0,-30,-30,
                                      -50,-30,-30,-30,-30,-30,-30,-50 };

const int white_king_eg_table[64] = { -50,-30,-30,-30,-30,-30,-30,-50,
                                      -30,-30,  0,  0,  0,  0,-30,-30,
                                      -30,-10, 20, 30, 30, 20,-10,-30,
                                      -30,-10, 30, 40, 40, 30,-10,-30,
                                      -30,-10, 30, 40, 40, 30,-10,-30,
                                      -30,-10, 20, 30, 30, 20,-10,-30,
                                      -30,-20,-10,  0,  0,-10,-20,-30,
                                      -50,-40,-30,-20,-20,-30,-40,-50, };

int newEvaluate(Position& eB);

int Evaluate(Position& evaluateBoard);

int piece_to_idx(Piece& p);
int piece_color(Piece& p);
int piece_value(bool mg, Piece& square);
int psqt_value(bool mg, Piece& square, int sq);
int imbalance(Piece& square, Piece* pieces);
int imbalance_t(Piece& square, Piece* pieces);
int middle_game_eval(Position& b);

template<Color color>
int moveScore(Position& board, int Aalpha, int Bbeta, int depth) {
    ++mSCalls;
    MoveList<color> mL(board);

    TranspositionFlags flags = FLAG_EXACT;
    if (depth == 0 || mL.size() == 0 || (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start_time) > 15 * 1000ms)) {
        int score = Evaluate(board);

        table.push_position({board.get_hash(), flags, score, depth, Move(0)});
        return score;
    }

    depth--;

    if (color == WHITE) {
        int maxEval = -INFINITY;
        for (Move m : mL) {
            board.play<color>(m);

            int score = table.probe_hash(board.get_hash(), Aalpha, Bbeta, depth);

            if (score == NO_HASH_ENTRY) {
                score = moveScore<~color>(board, Aalpha, Bbeta, depth);
                board.undo<color>(m);
            }
            else {
                board.undo<color>(m);
            }

            if (score > maxEval)
                maxEval = score;
            if (score > Aalpha)
                Aalpha = score;

            if (score >= Bbeta) {
                flags = FLAG_BETA;
                break;
            }
        }

        table.push_position(Transposition{ board.get_hash(), flags, maxEval, depth, Move(0)});

        return maxEval;
    }
    else {
        int minEval = INFINITY;
        for (Move m : mL) {
            board.play<color>(m);

            int score = table.probe_hash(board.get_hash(), Aalpha, Bbeta, depth);

            if (score == NO_HASH_ENTRY) {
                score = moveScore<~color>(board, Aalpha, Bbeta, depth);
                board.undo<color>(m);
            }
            else {
                board.undo<color>(m);
            }

            if (score < minEval)
                minEval = score;
            if (score < Bbeta)
                Bbeta = score;

            if (score <= Aalpha) {
                flags = FLAG_ALPHA;
                break;
            }
        }

        table.push_position(Transposition{ board.get_hash(), flags, minEval, depth, Move(0) });

        return minEval;
    }

}


template <Color color>
Move findBestMove(Position& board, int depth) {

    Move bestMove;
    int bestMoveScore = color == WHITE ? -INFINITY : INFINITY;


    MoveList<color> mL(board);
    start_time = std::chrono::system_clock::now();

    for (Move m : mL) {
        board.play<color>(m);
        int score = moveScore<~color>(board, -INFINITY, INFINITY, depth);
        board.undo<color>(m);

        if (color == BLACK && score < bestMoveScore) {
            bestMove = m;
            bestMoveScore = score;
        }
        else if (color == WHITE && score > bestMoveScore) {
            bestMove = m;
            bestMoveScore = score;
        }
    }


    return bestMove;
}

#endif // EVALUATE_H