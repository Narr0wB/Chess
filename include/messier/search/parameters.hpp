
#ifndef PARAMETERS_HPP
#define PARAMETERS_HPP

#define MAX_DEPTH 30
#define MAX_PLY   35 

/* Histories */
constexpr int MAX_TABLE       = (MAX_PLY + 1);
constexpr int16_t MAX_HISTORY = ((int16_t)(1 << 13));

/* Search */
constexpr int time_check_nodes      = 2048;
constexpr int qsearch_see_threshold = -75;

/* Futility pruning */
constexpr int fp_depth     = 3;
constexpr int fp_margin    = 150;
constexpr int fp_movecount = 1;

/* Reverse futility pruning */
constexpr int rfp_base_margin = 75;
constexpr int rfp_depth       = 3;

/* Razoring */
constexpr int razoring_base  = 60;
constexpr int razoring_depth = 2;

/* Null move pruning */
constexpr int nmp_depth          = 2;
constexpr int nmp_npawn_material = 0;

/* Internal Iterative Reductions */
constexpr int iir_depth = 7;

/* Delta margin */
constexpr int delta_margin = 100;

/* Late move reductions */
constexpr int lmr_depth     = 4;
constexpr int lmr_movecount = 1;

/* Movepicker */
constexpr int good_quiet_threshold   = -1;
constexpr int good_capture_threshold = 0;
constexpr int moves_before_sorting   = 128;
constexpr int sacrificial_check_threshold = -75;

/* Eval */
constexpr int attacked_shield_penalty = 16;
constexpr int pawn_inner_weight       = 8;
constexpr int pawn_outer_weight       = 4;
constexpr int multi_attacker_divisor  = 32;
constexpr int single_attacker_divisor = 4;
constexpr int max_king_penalty        = 300;
constexpr int weak_square_weight      = 3;
constexpr int safe_check_weight       = 0;

constexpr int attacker_weight[NPIECE_TYPES]  = { 0, 8, 8, 12, 20, 0 };
constexpr int inner_hit_weight[NPIECE_TYPES] = { 0, 4, 4, 6, 8, 0 };
constexpr int outer_hit_weight[NPIECE_TYPES] = { 0, 1, 1, 2, 3, 0 };

constexpr int shelter_penalty[9] = { 0, 0, 5, 10, 16, 20, 22, 24, 24 };
constexpr int storm_penalty[9] = { 0, 28, 18, 10, 5, 2, 0, 0, 0 };
constexpr int blocked_storm_penalty[9] = { 0, 8, 6, 3, 1, 0, 0, 0, 0 };

constexpr int semi_open_penalty = 8;
constexpr int fully_open_penalty = 18;

#endif // PARAMETERS_HPP