
#ifndef HISTORY_HPP
#define HISTORY_HPP

#include <messier/movegen/move.hpp>
#include <messier/search/parameters.hpp>

#define MAX_TABLE (MAX_PLY + 1)
#define MAX_HISTORY 8000

struct QuietHistory {
    int board[NSQUARES][NSQUARES][NCOLORS];

    template <Color C> inline void update_history(const Move& m, int bonus) 
    {
        int clamped_bonus = std::clamp(bonus, -MAX_HISTORY, MAX_HISTORY);
        board[m.from()][m.to()][C]
            += clamped_bonus - board[m.from()][m.to()][C] * std::abs(clamped_bonus) / MAX_HISTORY;
    }
};

struct KillerHistory {
    Move moves[MAX_TABLE][NCOLORS];
};

struct CaptureHistory {
    int board[NPIECES][NPIECE_TYPES][NSQUARES];

    inline void update_history(Piece hunter, PieceType captured, Square sq, int bonus) 
    {
        int clamped_bonus = std::clamp(bonus, -MAX_HISTORY, MAX_HISTORY);

        board[hunter][captured][sq]
            += clamped_bonus - board[hunter][captured][sq] * std::abs(clamped_bonus) / MAX_HISTORY;
    }
};

struct ContinuationHistory {

};

struct CounterMovesHistory {

};

#endif // HISTORY_HPP