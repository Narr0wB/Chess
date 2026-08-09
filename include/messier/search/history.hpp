
#ifndef HISTORY_HPP
#define HISTORY_HPP

#include <messier/movegen/move.hpp>
#include <messier/search/parameters.hpp>

struct QuietHistory {
    int16_t board[NSQUARES][NSQUARES][NCOLORS];

    template <Color C> inline void update_history(const Move& m, int16_t bonus) 
    {
        int16_t clamped_bonus = std::clamp<int16_t>(bonus, -MAX_HISTORY, MAX_HISTORY);
        board[m.from()][m.to()][C]
            += clamped_bonus - board[m.from()][m.to()][C] * std::abs(clamped_bonus) / MAX_HISTORY;
    }
};

struct CaptureHistory {
    int16_t board[NPIECES][NPIECE_TYPES][NSQUARES];

    inline void update_history(Piece hunter, PieceType captured, Square sq, int16_t bonus) 
    {
        int16_t clamped_bonus = std::clamp<int16_t>(bonus, -MAX_HISTORY, MAX_HISTORY);

        board[hunter][captured][sq]
            += clamped_bonus - board[hunter][captured][sq] * std::abs(clamped_bonus) / MAX_HISTORY;
    }
};

struct ContinuationHistory {
    int16_t board[NPIECE_TYPES * NSQUARES][NPIECE_TYPES * NSQUARES];

    inline void update_history(PieceType prev, Square prev_sq, PieceType curr, Square curr_sq, int16_t bonus)
    {
        int16_t clamped_bonus = std::clamp<int16_t>(bonus, -MAX_HISTORY, MAX_HISTORY);

        board[prev * NSQUARES + prev_sq][curr * NSQUARES + curr_sq]
            += clamped_bonus - board[prev * NSQUARES + prev_sq][curr * NSQUARES + curr_sq] * std::abs(clamped_bonus) / MAX_HISTORY;
    }
};

struct KillerHistory {
    Move moves[MAX_TABLE][NCOLORS];
};


struct CounterMovesHistory {

};

#endif // HISTORY_HPP