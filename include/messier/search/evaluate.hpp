
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
    constexpr int NO_PAWN = 8;

    Bitboard our_pawns = pos.bitboard_of(make_piece(Defender, PAWN));
    Bitboard their_pawns = pos.bitboard_of(make_piece(Attacker, PAWN));
    
    const Square king = bsf(pos.bitboard_of(make_piece(Defender, KING)));
    const int king_rank = int(rank_of(king));
    const int king_file = int(file_of(king));
    const int center = std::clamp(king_file, int(BFILE), int(GFILE));

    auto nearest_dist = [&](Bitboard pawns, int file) {
        int nearest = NO_PAWN;
        pawns &= MASK_FILE[file];

        while (pawns) {
            const int rank = int(rank_of(pop_lsb(&pawns)));
            int distance = Defender == WHITE ? rank - king_rank : king_rank - rank;
            if (distance > 0)
                nearest = std::min(nearest, distance);
        }

        return nearest;
    };

    for (int file = center - 1; file <= center + 1; ++file) {
        const int our_distance = nearest_dist(our_pawns, file);
        const int their_distance = nearest_dist(their_pawns, file);

        const int file_distance = file > king_file ? file - king_file : king_file - file;
        const int file_scale = 
            file_distance == 0 ? 100 : 
            file_distance == 1 ? 80 : 50;

        safety.shelter += shelter_penalty[our_distance] * file_scale / 100;

        if (their_distance != NO_PAWN) {
            const bool blocked =
                our_distance != NO_PAWN
                && their_distance == our_distance + 1;

            const int penalty = blocked
                ? blocked_storm_penalty[their_distance]
                : storm_penalty[their_distance];

            safety.storm += penalty * file_scale / 100;
        }
        else {
            safety.open_files +=
                (our_distance == NO_PAWN
                    ? fully_open_penalty
                    : semi_open_penalty)
                * file_scale / 100;
        }

    }

    Bitboard attacked_shield = 
        our_pawns
        & (info.king_inner[Defender] | info.king_outer[Defender])
        & info.attacks_by[Attacker][PAWN];
    
    safety.storm += attacked_shield_penalty * pop_count(attacked_shield);
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
