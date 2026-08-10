
#ifndef EVALUATE_H
#define EVALUATE_H

#include <messier/movegen/position.hpp>
#include <messier/movegen/types.hpp>
#include <messier/search/search.hpp>
#include <messier/search/parameters.hpp>

#include <chrono>

struct EvalInfo {
    Bitboard occupancy[NCOLORS];

    Bitboard attacks_by[NCOLORS][NPIECE_TYPES];

    Bitboard attacks[NCOLORS];
    Bitboard attacks_twice[NCOLORS];

    Bitboard king_inner[NCOLORS];
    Bitboard king_outer[NCOLORS];

    int king_attackers[NCOLORS];
    int king_attack_units[NCOLORS];
    int king_inner_hits[NCOLORS];
    int king_outer_hits[NCOLORS];
};

struct KingSafety {
    int shelter       = 0;
    int storm         = 0;
    int open_files    = 0;

    int attack_units  = 0;
    int attackers     = 0;
    int inner_hits    = 0;
    int outer_hits    = 0;

    int weak_squares  = 0;
    int safe_checks   = 0;
};

int evaluate(const Position& position);

template <Color Defender>
inline void evaluate_shelter(KingSafety& safety, const EvalInfo& info, const Position& pos)
{
    constexpr Color Attacker = ~Defender;

    Bitboard our_pawns = pos.bitboard_of(make_piece(Defender, PAWN));

    Bitboard attacked_shield = 
        our_pawns
        & (info.king_inner[Defender] | info.king_outer[Defender])
        & info.attacks_by[Attacker][PAWN];
    
    safety.shelter += attacked_shield_penalty * pop_count(attacked_shield);
}

template <Color Defender>
inline void evaluate_tactical(KingSafety& safety, const EvalInfo& info)
{
    constexpr Color Attacker = ~Defender;

    safety.attack_units = info.king_attack_units[Defender];
    safety.attackers    = info.king_attackers[Defender];
    safety.inner_hits   = info.king_inner_hits[Defender];
    safety.outer_hits   = info.king_outer_hits[Defender];

    Bitboard pawn_inner = info.attacks_by[Attacker][PAWN] & info.king_inner[Defender] & ~info.occupancy[Attacker];
    Bitboard pawn_outer = info.attacks_by[Attacker][PAWN] & info.king_outer[Defender] & ~info.occupancy[Attacker];

    safety.inner_hits += pop_count(pawn_inner) * pawn_inner_weight;
    safety.outer_hits += pop_count(pawn_outer) * pawn_outer_weight;
}

template <Color Defender>
inline void evaluate_weak(KingSafety& safety, const EvalInfo& info)
{
    constexpr Color Attacker = ~Defender;

    Bitboard usable = (info.king_inner[Defender] | info.king_outer[Defender]) & ~info.occupancy[Attacker];
    Bitboard weak = 
        usable
        & info.attacks[Attacker]
        & ~info.attacks_twice[Defender]
        & (~info.attacks[Defender] | info.attacks_twice[Attacker]);

    safety.weak_squares = pop_count(weak);
}

template <Color Defender>
int evaluate_king(const EvalInfo& info, const Position& pos)
{
    KingSafety safety = {0};

    evaluate_shelter<Defender>(safety, info, pos);
    evaluate_tactical<Defender>(safety, info);
    evaluate_weak<Defender>(safety, info);

    int structural =
        safety.shelter +
        safety.storm +
        safety.open_files;

    int units =
        safety.attack_units +
        safety.inner_hits +
        safety.outer_hits +
        weak_square_weight * safety.weak_squares +
        safe_check_weight * safety.safe_checks;

    int tactical = 0;

    if (safety.attackers >= 2)
        tactical = units * units / multi_attacker_divisor;
    else
        tactical = units / single_attacker_divisor;

    return std::clamp(structural + tactical, 0, max_king_penalty);
}


template <Color Us>
int corrected_eval(const Position& position) { return evaluate(position) * (Us == WHITE ? 1 : -1); };

#endif // EVALUATE_H
