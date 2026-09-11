
#include <atomic>
#include <algorithm>

#include <messier/search/search.hpp>
#include <messier/search/evaluate.hpp>
#include <messier/search/movepicker.hpp>
#include <messier/search/parameters.hpp>
#include <messier/misc.hpp>
#include <messier/log.hpp>

#define DELTA_MARGIN 100

namespace Search {
    int lmr_reductions[MAX_DEPTH + 1][64];
    int lmp_margin[2][MAX_DEPTH + 1];

    const std::vector<std::string> BenchFENs = {
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", // Startpos
        "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", // Kiwipete
        "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1", // Endgame
        "4rrk1/pp1n3p/3q2pQ/2p1pb2/2PP4/2P3N1/P2B2PP/4RRK1 b - - 5 20", // Middlegame tactics
        "rq3rk1/ppp2ppp/1b3B2/4p3/1P1n4/P1NP1bP1/2P1NP1P/R2Q1RK1 b - - 0 14",
        "r1bq1r1k/1pp1n1pp/1p1p4/4p2Q/4Pp2/1BNP4/PPP2PPP/3R1RK1 w - - 2 14"
    };

    void Worker::idle_loop() {
        while (true) {
            std::unique_lock<std::mutex> lock(m_mutex);

            /* Stop waiting until we are put into state SEARCHING or we are killed*/
            m_cv.wait(lock, [&]{ return (m_state == WorkerState::SEARCHING || m_kill); });

            /* The mutex is locked by the current thread from here on out */
            if (m_kill) break;

            iterative_deepening(false); 

            m_state = WorkerState::IDLE;
        }

        std::unique_lock<std::mutex> lock(m_mutex);
        m_state = WorkerState::DEAD;
    }

    void Worker::run(Position& root, const SearchConfig& cfg) {
        // Stop any previous searches
        stop();

        std::unique_lock<std::mutex> lock(m_mutex);
        m_cfg = cfg;
        m_root = root;
        m_state = WorkerState::SEARCHING;
        m_stop = false;
        m_cv.notify_one();
    }

    void Worker::clear()
    {
        m_ctx  = {0};
        m_info = {0};
        m_generation = 0;
        m_tt.clear();
    }

    void Worker::stop() {
        m_stop = true;
        m_stop.notify_one();
    }

    void Worker::kill() {
        stop();
        m_kill = true;
        m_cv.notify_one();
        m_thread.join();
    }

    WorkerState Worker::get_state() {
        return m_state;
    }

    void Worker::bench(const SearchConfig& cfg) {
        m_stop = false;
        m_cfg = cfg;

        uint64_t total_nodes = 0;
        uint64_t total_qnodes = 0;
        uint64_t start_time = time_ms();

        for (const std::string& fen : BenchFENs) {
            Position::set(fen, m_root);
            clear();
            iterative_deepening(false);
            total_nodes += m_info.nodes;
            total_qnodes += m_info.qnodes;
        }

        uint64_t end_time = time_ms();
        uint64_t elapsed = end_time - start_time;

        uint64_t nps = (total_nodes * 1000) / (elapsed != 0 ? elapsed : 1);

        std::cout << "\n===========================\n";
        std::cout << "Total time (ms) : " << elapsed << "\n";
        std::cout << "Nodes searched  : " << total_nodes << "\n";
        std::cout << "Of which qnodes : " << total_qnodes << "\n";
        std::cout << "Nodes/second    : " << nps << "\n";
    }

    void compute_lmx_parameters() {
        lmr_reductions[0][0] = 0;

        for (int depth = 1; depth <= MAX_DEPTH; ++depth) {
            lmp_margin[0][depth] = 1.5 + 0.5 * std::pow(depth, 2.0);
            lmp_margin[1][depth] = 2.0 + 0.5 * std::pow(depth, 2.0);

            for (int move_count = 1; move_count < 64; ++move_count)
                lmr_reductions[depth][move_count] = static_cast<int>(0.7844 + std::log(depth) * std::log(move_count) / 2.4696);
        }
    }

    bool Worker::exit_search()
    {
        return (m_stop) 
            || (m_cfg.timeset && (m_info.nodes % time_check_nodes == 0) && (time_ms() >= m_cfg.search_end_time))
            || (m_cfg.nodeset && (m_info.nodes >= m_cfg.nodeslimit));
    }
   


    template <Color C, bool PVnode>
    int Worker::quiescence(Position& pos, SearchStack* ss, int Aalpha, int Bbeta) 
    {
        if (exit_search()) 
            return 0;
        
        uint64_t hash = pos.get_hash();

        auto [tt_hit, tte] = m_tt.probe(hash);
        int tt_eval        = tt_hit ? tte->eval : NO_SCORE;
        int tt_score       = tt_hit ? tte->score : NO_SCORE;
        Move tt_move       = tt_hit ? tte->move : Move::none();
        uint8_t tt_bound   = tt_hit ? tte->flags : FLAG_EMPTY;
        int8_t tt_depth    = tt_hit ? tte->depth : 0;

        if (tt_hit && tt_score >= MATE_SCORE - MAX_PLY) 
            tt_score -= ss->ply; 
        else if (tt_hit && tt_score <= -MATE_SCORE + MAX_PLY)
            tt_score += ss->ply;

        // If we are not in a pv node, and we got a useful score from the TT, return early
        // NOTE: we do not check if the tte depth is greater (or equal) than the current depth because in quiescence any depth IS greater or equal than 0 
        if (!PVnode &&
            tt_hit && 
            ((tt_bound == FLAG_ALPHA && tt_score <= Aalpha) ||   
            (tt_bound == FLAG_BETA  && tt_score >= Bbeta) ||   
            (tt_bound == FLAG_EXACT))) 
        {
            m_info.tt_hits++;
            return tt_score;
        }

        int best_score = -INFTY;
        int score = 0;
        int move_count = 0;
        ss->in_check = pos.in_check<C>();
        Move m = Move::none();

        Transposition node(FLAG_ALPHA, hash, 0, NO_SCORE, NO_SCORE, Move::none(), m_info.generation);

        if (!ss->in_check) {
            ss->static_eval = node.eval = node.score = best_score = (tt_eval != NO_SCORE) ?  tt_eval : corrected_eval<C>(pos); 

            if (best_score > Aalpha) {
                Aalpha = best_score;

                // Stand pat, check if current position is already better than Beta (or atleast better than alfa)
                if (best_score >= Bbeta) {
                    node.flags = FLAG_BETA;
                    m_tt.push(hash, node);
                    return best_score;
                }
            }
        }
        else {
            ss->static_eval = NO_SCORE;
        }
        
        if (ss->ply >= MAX_PLY - 1) 
            return best_score != -INFTY ? best_score : corrected_eval<C>(pos);


        MovePicker<C> picker(pos, m_ctx, ss, ss->ply, -1, ss->in_check, tt_move);
        while ((m = picker.next()) != Move::none()) {
            move_count++;

            if (!ss->in_check && !m.is_promotion()) {
                const bool gives_check = pos.gives_check<C>(m);
                if (!m.is_capture()) 
                    continue;

                if ((ss->static_eval + piece_value[m.is_enpassant() ? PAWN : type_of(pos.at(m.to()))] + delta_margin <= Aalpha)
                    && !gives_check)
                    continue;

                if (!gives_check && !pos.see<C>(m, qsearch_see_threshold))
                    continue;
            }

            // Actual node count
            m_info.nodes++;
            m_info.qnodes++;
            ss->move = m;
            ss->moved = type_of(pos.at(m.from()));

            pos.play<C>(m);

            (ss + 1)->qply = ss->qply;
            score = -quiescence<~C, PVnode>(pos, ss + 1, -Bbeta, -Aalpha);

            pos.undo<C>(m);
            
            if (exit_search()) 
                return 0;

            if (score > best_score) {
                best_score = score;
                node.score = score;
                node.move  = m;

                if (score >= MATE_SCORE - MAX_PLY)
                    node.score = score + ss->ply;
                else if (score <= -MATE_SCORE + MAX_PLY)
                    node.score = score - ss->ply;

                if (score > Aalpha) {
                    Aalpha = best_score;

                    if (score >= Bbeta) {
                        node.flags = FLAG_BETA;
                        break;
                    }
                }
            }
        }

        if (ss->in_check && move_count == 0) {
            // If we are in check and there are no more moves available (i.e. best_score is still -INFTY), then we are in a checkmate
            best_score = ss->in_check ? -MATE_SCORE + ss->ply : 0;
            node.score = ss->in_check ? -MATE_SCORE : 0;
            node.flags = FLAG_EXACT;
        }

        m_tt.push(hash, node);
        return best_score;
    }



    /* Main search function */
    template <Color C, bool PVnode>
    int Worker::search(Position& pos, SearchStack *ss, int Aalpha, int Bbeta, int depth) 
    {
        int ply = ss->ply;
        int score = 0;
        int best_score = -INFTY;
        uint64_t hash = pos.get_hash();
        Move m = Move::none();

        ss->in_check = pos.in_check<C>();

        // If depth is below zero, dip into quiescence search
        if (depth <= 0) {
            ss->qply = ply;
            return quiescence<C, PVnode>(pos, ss, Aalpha, Bbeta);
        }

        assert(-INFTY <= Aalpha && Aalpha < Bbeta && Bbeta <= INFTY);
        assert(0 < depth && depth < MAX_DEPTH + 1);
        assert(PVnode || (Aalpha == Bbeta - 1));

        // If out of time or hit any other constraints then exit the search
        if (exit_search()) 
            return 0;

        if (ply != 0) {
            int limit = pos.ply() - pos.halfmove;
            if (limit < 0) limit = 0;
            int counter = 0;

            for (int i = pos.ply() - 2; i >= limit; i -= 2) {
                /*
                    If we find, in our linear board history, the current position, then we have proof that there exists a path (a sequence of moves)
                    that connects this position to itself. If we were to rexplore this position, we would explore the circular path, which would lead
                    us to this position again, triggering a three-fold repetition. We then assign the value 0 to this position (it's a draw)
                */

                if (hash == pos.history[i].hash && ++counter == 2) {
                    return 0;
                }
            }

            if (pos.halfmove >= 100) return 0;
        }

        auto [tt_hit, tte] = m_tt.probe(hash);
        int tt_eval        = tt_hit ? tte->eval : NO_SCORE;
        int tt_score       = tt_hit ? tte->score : NO_SCORE;
        Move tt_move       = tt_hit ? tte->move : Move::none();
        uint8_t tt_bound   = tt_hit ? tte->flags : FLAG_EMPTY;
        int8_t tt_depth    = tt_hit ? tte->depth : 0;

        if (tt_hit && tt_score >= MATE_SCORE - MAX_PLY) 
            tt_score -= ss->ply; 
        else if (tt_hit && tt_score <= -MATE_SCORE + MAX_PLY)
            tt_score += ss->ply;

        if (tt_hit 
            && tt_depth >= depth 
            && ply != 0)
        {
            m_info.tt_hits++;
            if (tt_bound == FLAG_ALPHA && tt_score <= Aalpha
                || tt_bound == FLAG_BETA && tt_score >= Bbeta
                || tt_bound == FLAG_EXACT) 
                return tt_score;
        }

        if (ss->in_check) {
            ss->static_eval = NO_SCORE;
        }
        else {
            ss->static_eval = (tt_eval != NO_SCORE) ? tt_eval : corrected_eval<C>(pos); 

            /* Razoring */
            int margin = razoring_base * depth;
            if (!PVnode
                && depth <= razoring_depth
                && ss->static_eval + margin < Aalpha)
            {
                (ss + 1)->qply = ply;
                int qscore = quiescence<C, false>(pos, ss + 1, Aalpha - 1, Aalpha);
                if (qscore <= Aalpha) return qscore;
            }

            /* Reverse futility pruning */
            margin = rfp_base_margin * depth;
            if (!PVnode
                && depth <= rfp_depth
                && ss->static_eval - margin >= Bbeta)
                return (ss->static_eval + Bbeta)/2;

            /* Null move pruning */
            if (!PVnode 
                && depth >= nmp_depth 
                && ss->static_eval >= Bbeta
                && pos.npm(C) > nmp_npawn_material
                && !ss->null_move 
                && (!tt_hit || tt_bound == FLAG_BETA || tt_score >= Bbeta))
            {
                int NMPReduction = 5 + depth / 5 + std::min(2, (ss->static_eval - Bbeta) / 191);

                (ss + 1)->null_move = true;
                pos.play_null_move();

                int v = -search<~C, false>(pos, ss + 1, -Bbeta, -Bbeta + 1, depth - NMPReduction);

                pos.undo_null_move();
                (ss + 1)->null_move = false;

                if (v >= Bbeta)
                    return v >= MATE_SCORE - MAX_PLY ? Bbeta : v;
            }

            // IIR by Ed Schroder
            if (PVnode
                && tt_move == Move::none()
                && depth >= iir_depth)
                depth -= 1;
        }

        int margin = fp_margin * depth;
        const bool futility_candidate = !PVnode 
            && !ss->in_check
            && depth <= fp_depth 
            && ss->static_eval + margin <= Aalpha;

        const bool improving = ss->ply >= 2 
            && !ss->in_check 
            && ss->static_eval > (ss - 2)->static_eval;

        Transposition node(FLAG_ALPHA, hash, (int8_t)depth, NO_SCORE, ss->static_eval, tt_move, m_info.generation);

        Move quiets_searched[MAX_MOVES];
        Move captures_searched[MAX_MOVES];
        int quiets_count = 0;
        int captures_count = 0;

        int move_count = 0;
        int searched_count = 0;

        MovePicker<C> picker(pos, m_ctx, ss, ply, depth, ss->in_check, tt_move);
        while ((m = picker.next()) != Move::none()) {
            /* Explore a single branch of the main tree */
            if (m_cfg.searchmove != Move::none() 
                && ss->ply == 0 
                && m != m_cfg.searchmove)
                continue;

            move_count++;

            const int new_depth = depth - 1;
            const int reduction = lmr_reductions[depth][std::min(move_count, 63)];
            const int reduced_depth = std::max(0, new_depth - reduction);

            const bool is_quiet    = m.is_quiet();
            const bool is_capture  = m.is_capture();
            const bool gives_check = pos.gives_check<C>(m);


            /* Late Move Pruning */
            const bool lmp_prunable = !PVnode
                && !ss->in_check
                && is_quiet
                && depth <= 3 
                && move_count > lmp_margin[improving][depth];

            if (lmp_prunable && !gives_check)
                continue;

            /* Futility pruning */
            const bool futility_prunable = futility_candidate
                && is_quiet
                && move_count > fp_movecount;

            if (futility_prunable && !gives_check)
                continue;

            /* SEE pruning */
            int see_depth = reduced_depth + 1;
            int see_pruning_threshold = is_quiet ? -90 * see_depth : -20 * see_depth * see_depth;
            const bool see_prunable = !PVnode
                && picker.stage() != Stage::GOOD_CAPTURES
                && !ss->in_check
                && !m.is_promotion()
                && searched_count > 0
                && !pos.see<C>(m, see_pruning_threshold - 250 * gives_check);

            if (see_prunable)
                continue;

            searched_count++;
            m_info.nodes++;
            ss->move = m;
            ss->moved = type_of(pos.at(m.from()));

            pos.play<C>(m);

            // PVS
            if (searched_count < 2) {
                score = -search<~C, PVnode>(pos, ss + 1, -Bbeta, -Aalpha, depth - 1);
            }
            else {
                // Late Move Reductions
                if (depth >= lmr_depth 
                    && !ss->in_check
                    && searched_count > lmr_movecount
                    && !gives_check)
                {
                    score = -search<~C, false>(pos, ss + 1, -Aalpha - 1, -Aalpha, reduced_depth);

                    if (score > Aalpha) 
                        score = -search<~C, false>(pos, ss + 1, -Aalpha - 1, -Aalpha, new_depth);
                }
                else {
                    score = -search<~C, false>(pos, ss + 1, -Aalpha - 1, -Aalpha, new_depth);
                }

                if (Aalpha < score && score < Bbeta)
                    score = -search<~C, true>(pos, ss + 1, -Bbeta, -Aalpha, new_depth);
            }

            pos.undo<C>(m);

            if (is_quiet)
                quiets_searched[quiets_count++] = m;
            else if (is_capture)
                captures_searched[captures_count++] = m;

            if (exit_search()) 
                return 0;

            if (score > best_score) {
                best_score = score;
                node.score = score;
                node.move  = m;
                ss->bestmove = m;

                if (score >= MATE_SCORE - MAX_PLY)
                    node.score = score + ss->ply;
                else if (score <= -MATE_SCORE + MAX_PLY)
                    node.score = score - ss->ply;

                // We have found a move that is better than the current alpha
                if (best_score > Aalpha) {
                    Aalpha = best_score;
                    node.flags = FLAG_EXACT;

                    // Fail High Node, i.e. we have found a move that is better than what our opponent is guaranteed to take
                    if (best_score >= Bbeta) {
                        const int16_t bonus = std::min<int16_t>(MAX_HISTORY, 300 * depth - 250);
                        Square from        = m.from();
                        Square to          = m.to();
                        Piece hunter       = pos.at(from);

                        if (is_quiet) {
                            if (m != m_ctx.killer.moves[ply][0]) {
                                m_ctx.killer.moves[ply][1] = m_ctx.killer.moves[ply][0];
                                m_ctx.killer.moves[ply][0] = m;
                            }

                            if (ss->ply > 0 && !ss->null_move)
                                m_ctx.cont_one.update_history((ss - 1)->moved, (ss - 1)->move.to(), type_of(hunter), to, bonus);
                            m_ctx.quiet.update_history<C>(m, bonus);

                            for (int i = 0; i < quiets_count - 1; ++i) {
                                m_ctx.quiet.update_history<C>(quiets_searched[i], -bonus);
                                if (ss->ply > 0 && !ss->null_move)
                                    m_ctx.cont_one.update_history((ss - 1)->moved, (ss - 1)->move.to(), type_of(pos.at(quiets_searched[i].from())), quiets_searched[i].to(), -bonus);
                            }
                        }
                        else if (is_capture) {
                            PieceType captured = m.is_enpassant() ? PAWN : type_of(pos.at(to));

                            m_ctx.capture.update_history(hunter, captured, to, bonus);
                            for (int i = 0; i < captures_count - 1; ++i) {
                                from     = captures_searched[i].from();
                                to       = captures_searched[i].to();
                                captured = captures_searched[i].is_enpassant() ? PAWN : type_of(pos.at(to));
                                hunter   = pos.at(from);

                                m_ctx.capture.update_history(hunter, captured, to, -bonus);
                            }
                        }

                        node.flags = FLAG_BETA;
                        m_tt.push(hash, node);

                        return best_score;
                    }
                }
            }
        }

        if (move_count == 0) {
            // If we are in check and there are no more moves available (i.e. best_score is still -INFTY), then we are in a checkmate
            best_score = ss->in_check ? -MATE_SCORE + ss->ply : 0;
            node.score = ss->in_check ? -MATE_SCORE : 0;
            node.flags = FLAG_EXACT;
        }

        m_tt.push(hash, node);

        return best_score;
    }



    void Worker::iterative_deepening(bool silent = false) 
    {
        int max_depth = m_cfg.max_depth <= MAX_DEPTH ? m_cfg.max_depth : MAX_DEPTH;
        int current_depth = 1;
        int last_score = NO_SCORE;
        Color to_play = m_root.turn();
        Move best_move = Move::none();

        m_info = {0};
        m_info.generation = m_generation++;

        auto start_time = time_ms();

        for (; current_depth <= max_depth; ++current_depth) {
            int root_depth = current_depth;
            int depth      = root_depth;
            int aw_margin  = 50;
            int alpha      = last_score != NO_SCORE ? std::max(-INFTY, last_score - aw_margin) : -INFTY;
            int beta       = last_score != NO_SCORE ? std::min(INFTY, last_score + aw_margin) : INFTY;
            int score      = 0;

            m_info.aw_iterations = 0;

            // Wipe search stack
            for (int i = 0; i < 2 * MAX_PLY; ++i) {
                (m_ss + i)->ply         = i;
                (m_ss + i)->qply        = 0;
                (m_ss + i)->static_eval = 0;
                (m_ss + i)->move        = Move::none();
                (m_ss + i)->moved       = PieceType::NONE;
                (m_ss + i)->bestmove    = Move::none();
                (m_ss + i)->tt_hit      = false;
                (m_ss + i)->in_check    = false;
                (m_ss + i)->null_move   = false;
            }

            auto start_current_search = time_ms();

            // Aspiration window search
            while (true) {
                m_info.aw_iterations++;

                if (to_play == WHITE) 
                    score = search<WHITE, true>(m_root, m_ss, alpha, beta, depth);
                else 
                    score = search<BLACK, true>(m_root, m_ss, alpha, beta, depth);

                if (exit_search()) 
                    break;

                aw_margin *= 2;

                if (score <= alpha) {
                    alpha = std::max<int64_t>(-INFTY, alpha - aw_margin);
                    if (m_info.aw_iterations == 3)
                        alpha = -INFTY;
                    // alpha = -INFTY;
                    // beta  = INFTY;
                }
                else if (score >= beta) {
                    beta = std::min<int64_t>(INFTY, beta + aw_margin);
                    if (m_info.aw_iterations == 3)
                        beta = INFTY;
                    // alpha = -INFTY;
                    // beta  = INFTY;
                }
                else {
                    break; 
                }
            }

            if (exit_search()) 
                break;

            last_score = score;

            auto end_current_search = time_ms();

            uint64_t elapsed = end_current_search - start_time;
            uint64_t nps = elapsed > 0 ? (m_info.nodes* 1000ULL) / elapsed : 0;   

            int pv_len = extract_pv(m_root, m_tt, m_pv);

            if (!silent) {
                std::cout 
                    << "info depth " << current_depth 
                    << " score ";
                if (std::abs(score) >= MATE_SCORE - MAX_PLY)
                    std::cout << "mate " << (score > 0 ? (MATE_SCORE - score + 1) / 2 : -(MATE_SCORE + score) / 2);
                else
                    std::cout << "cp " << score;
                std::cout << " nodes " << m_info.nodes
                    << " qnodes " << m_info.qnodes 
                    << " nps " << nps 
                    << " tthits " << m_info.tt_hits 
                    << " ttstored " << m_tt.stored() 
                    << " BF " << std::pow(m_info.nodes, 1.0f / current_depth) 
                    << " AWit " << m_info.aw_iterations 
                    << " pvlen " << pv_len 
                    << " pv ";
                for (int j = 0; j < pv_len; j++) {
                    std::cout << m_pv[j] << " ";
                }
                std::cout << std::endl;
            }

            best_move = m_ss->bestmove;
        }

        if (m_cfg.infinite)
            m_stop.wait(false);

        auto end_time = time_ms();

        if (!silent) {
            LOG_INFO("searchtime {}ms", (end_time - start_time));

            std::cout << "bestmove ";
            std::cout << best_move;
            std::cout << std::endl;
        }
    }
    
    int extract_pv(const Position& root, const TTable& table, std::array<Move, MAX_PLY>& out)
    {
        Position pos = root;
        Color C = root.turn();
        uint64_t visited[MAX_PLY] = {0};
        size_t cnt = 0;

        for (; cnt < MAX_PLY; ++cnt) {
            uint64_t hash = pos.get_hash();

            for (size_t i = 0; i < cnt; ++i) if (visited[i] == hash) return cnt;
            visited[cnt] = hash;

            auto [tt_hit, tte] = table.probe(hash);

            if (!tt_hit || tte->move == Move::none() || !pos.is_pseudo_legal(tte->move)) {
                // LOG_INFO("Broken PV: {}, {}, {}, {}", tt_hit, tte.move, pos.is_pseudo_legal(tte.move), pos.turn());
                break;
            }

            out[cnt] = tte->move;
            pos.play_dynamic(tte->move, C);
            C = ~C;
        }

        return cnt;
    }

    // Explicit template definitions

    template int Worker::quiescence<WHITE, false>(Position& pos, SearchStack *ss, int Aalpha, int Bbeta);
    template int Worker::quiescence<BLACK, false>(Position& pos, SearchStack *ss, int Aalpha, int Bbeta);
    template int Worker::quiescence<WHITE, true>(Position& pos, SearchStack *ss, int Aalpha, int Bbeta);
    template int Worker::quiescence<BLACK, true>(Position& pos, SearchStack *ss, int Aalpha, int Bbeta);

    template int Worker::search<WHITE, false>(Position& pos, SearchStack *ss, int Aalpha, int Bbeta, int depth);
    template int Worker::search<WHITE, true>(Position& pos, SearchStack *ss, int Aalpha, int Bbeta, int depth);
    template int Worker::search<BLACK, false>(Position& pos, SearchStack *ss, int Aalpha, int Bbeta, int depth);
    template int Worker::search<BLACK, true>(Position& pos, SearchStack *ss, int Aalpha, int Bbeta, int depth);
} // namespace Search
