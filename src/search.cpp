#include "search.hpp"
#include "evaluate.hpp"
#include "move_picker.hpp"
#include "tt.hpp"

void addKillerMove(chess::Move move, uint8_t ply) {
    if (killer_moves[0][ply] != move.move()) {
        killer_moves[1][ply] = killer_moves[0][ply];
        killer_moves[0][ply] = move.move();
    }
}

int computeLMR(
    chess::Board& board, int depth, int move_count, chess::Move tt_move, int SEE_score,
    bool is_pv, bool is_capture, bool is_promotion, chess::Move move, bool in_nmp, int search_ply
) {
    int reductions = (
        move_count < 2 - (tt_move != chess::Move::NO_MOVE) + is_pv ||
        depth <= 2 || (is_capture && SEE_score > 0) ||
        (is_promotion && move.promotionType() == chess::PieceType::QUEEN)
    ) ? 0 : lmr[depth][move_count];

    if (move_count > 0 && depth > 2 && in_nmp) reductions++;

    if (reductions) {
        reductions -= is_pv;

        if (reductions > MAX_PLY) reductions = 0;
        if (reductions > depth - 2) reductions = depth - 2;
    }

    return reductions;
}

int Quiescence(chess::Board& board, int alpha, int beta, int search_ply, clk::time_point start_time) {
    assert(alpha >= -INFINITE);
    assert(beta <= INFINITE);
    assert(search_ply >= 0);
    assert(search_ply <= MAX_PLY);

    // check time every 1024 nodes.
    if ((nodes_searched & 1023) == 0) {
        auto now = std::chrono::steady_clock::now();
        if (now - start_time >= time_limit) {
            time_over = true;
            return 0;
        }
    }

    if (time_over) return 0;
    
    int best_score = evaluate(board);
    if (best_score >= beta) return best_score;
    if (best_score > alpha) alpha = best_score;

    chess::Movelist moves;
    chess::movegen::legalmoves<chess::movegen::MoveGenType::CAPTURE>(moves, board);
    ScoreMoves<true>(board, moves, search_ply, chess::Move::NO_MOVE);

    for (int index = 0; index < moves.size(); index++) {
        PickMove(moves, index);
        const chess::Move& move = moves[index];
        nodes_searched++;

        board.makeMove(move);
        int score = -Quiescence(board, -beta, -alpha, search_ply + 1, start_time);
        board.unmakeMove(move);

        if (time_over) return 0;

        if (score > best_score) best_score = score;
        if (score > alpha) alpha = score;
        if (alpha >= beta) break;
    }

    return best_score;
}

int min_nmp_ply = 0;
int AlphaBeta(chess::Board& board, int alpha, int beta, int depth, int search_ply, clk::time_point start_time, bool allow_null_move) {
    assert(alpha >= -INFINITE);
    assert(beta <= INFINITE);

    assert(depth >= 0);
    assert(depth <= MAX_PLY);
    assert(search_ply >= 0);
    assert(search_ply <= MAX_PLY);

    pv_length[search_ply] = search_ply;
    int old_alpha = alpha;
    
    // check time every 1024 nodes.
    if ((nodes_searched & 1023) == 0) {
        auto now = std::chrono::steady_clock::now();
        if (now - start_time >= time_limit) {
            time_over = true;
            return 0;
        }
    }

    if (time_over) return 0;

    if (depth <= 0) return Quiescence(board, alpha, beta, search_ply, start_time);

    bool root_node = (search_ply == 0);
    if (!root_node) {
        if (board.isRepetition())           return DRAW;
        if (board.isHalfMoveDraw())         return DRAW;
        if (board.isInsufficientMaterial()) return DRAW;

        // Mate distance pruning.
        alpha = std::max(alpha, mated_in(search_ply));
        beta  = std::min(beta, mate_in(search_ply + 1));

        if (alpha >= beta) return alpha;
    }

    bool is_pv = (beta - alpha) > 1;
    int score  = 0;

    // Transposition Table Lookup
    // tt_hits and tt_misses are just for statistics and debugging
    // and has no effect on the actual search.
    uint64_t hash = board.hash();
    TTEntry* tt_entry = probeTT(hash);
    chess::Move tt_move = chess::Move::NO_MOVE;
    if (tt_entry && tt_entry->key == hash) {
        tt_hits++;
        tt_move = tt_entry->move;

        if (tt_entry->depth >= depth) {
            TTFlag flag = tt_entry->flag;
            int16_t tt_score = CorrectScore(tt_entry->score, search_ply);

            if (!root_node) {
                if (flag == TTFlag::EXACT)                                        return tt_score;
                else if (!is_pv && flag == TTFlag::TT_BETA && tt_score >= beta)   return tt_score;
                else if (!is_pv && flag == TTFlag::TT_ALPHA && tt_score <= alpha) return tt_score;
            }
        }
    } else {tt_misses++;}

    bool in_check    = board.inCheck();
    int16_t static_eval = evaluate(board);
    
    if (!is_pv && !root_node && !in_check && beta < MATE_IN_MAX_PLY && depth <= 7) {

        // Reverse Futility Pruning
        if (static_eval - 70 * depth >= beta) {
            return static_eval - 70 * depth;
        }

        // Razoring
        if (depth <= 3 && static_eval + 300 + 60 * depth < alpha) {
            return Quiescence(board, alpha, beta, search_ply, start_time);
        }
    }

    // Null move pruning
    if (allow_null_move && !is_pv && !root_node && hasNonPawnMaterial(board) && static_eval >= beta && !in_check && search_ply >= min_nmp_ply) {
        int R = 7 + depth / 3;

        board.makeNullMove();
        int null_score = -AlphaBeta(board, -beta, -beta + 1, depth - R, search_ply + 1, start_time, false);
        board.unmakeNullMove();

        if (null_score >= beta) {
            if (min_nmp_ply || depth < 16) return null_score;
            
            min_nmp_ply = search_ply + 3 * (depth - R) / 4;
            int validation_score = -AlphaBeta(board, beta - 1, beta, depth - R, search_ply + 1, start_time, false);
            min_nmp_ply = 0;

            if (validation_score >= beta) return validation_score;
        }
    }

    // Internal Iterative Reduction (IIR)
    if (depth >= 4 && tt_move == chess::Move::NO_MOVE && !in_check) depth--;

    chess::Color stm = board.sideToMove();
    int best_score = -INFINITE;
    chess::Move best_move = chess::Move::NO_MOVE;

    int extension = 0;
    extension += in_check;

    chess::Movelist moves;
    chess::movegen::legalmoves(moves, board);
    ScoreMoves<false>(board, moves, search_ply, tt_move);

    int move_size = moves.size();

    for (int index = 0; index < move_size; index++) {
        PickMove(moves, index);
        const chess::Move& move = moves[index];
        nodes_searched++;

        bool is_capture   = board.isCapture(move);
        bool is_promotion = move.typeOf() == chess::Move::PROMOTION;
        int  SEE_score    = SEE(board, move);

        board.makeMove(move);

        // Principal Variation Search (PVS)
        if (index == 0) {
            score = -AlphaBeta(board, -beta, -alpha, depth - 1 + extension, search_ply + 1, start_time, true);
        } else {
            int reductions = computeLMR(
                board, depth, move_size, tt_move, SEE_score, is_pv,
                is_capture, is_promotion,
                move, !allow_null_move, search_ply
            );

            score = -AlphaBeta(board, -alpha - 1, -alpha, depth - 1 + extension - reductions, search_ply + 1, start_time, true);
            if (score > alpha && reductions > 0) {
                score = -AlphaBeta(board, -alpha - 1, -alpha, depth - 1 + extension, search_ply + 1, start_time, true);
            }

            if (score > alpha && is_pv) {
                score = -AlphaBeta(board, -beta, -alpha, depth - 1 + extension, search_ply + 1, start_time, true);
            }
        }

        board.unmakeMove(move);

        if (time_over) return 0;

        if (score > best_score) best_score = score;
        
        if (score > alpha) {
            alpha = score;
            best_move = move;
            addPV(search_ply, move);
        }

        if (alpha >= beta) {
            if (!is_capture && !is_promotion) {
                addKillerMove(move, search_ply);

                int bonus = depth * depth;
                bool side = static_cast<bool>(stm);
                int from = move.from().index();
                int to = move.to().index();
                
                int history_score = (
                    bonus
                    - history_heuristic[side][from][to]
                    * bonus
                    / 16384
                );

                history_heuristic[side][from][to] += history_score;
            }
            break;
        }
    }

    if (moves.empty()) {
        return in_check ? mated_in(search_ply) : DRAW;
    }

    // Storing in the Transposition Table
    if (!time_over && tt_entry) {
        TTFlag flag = (best_score <= old_alpha) ? TTFlag::TT_ALPHA : (best_score >= beta) ? TTFlag::TT_BETA : TTFlag::EXACT;
        best_move = (best_move != chess::Move::NO_MOVE) ? best_move : tt_move;
        storeTT(hash, CorrectForStore(best_score, search_ply), flag, depth, best_move);
    }

    return best_score;
}

void IterativeDeepening(chess::Board& board) {
    chess::Move best_move = chess::Move::NO_MOVE;

    memset(killer_moves, 0, sizeof(killer_moves));
    memset(history_heuristic, 0, sizeof(history_heuristic));
    memset(pv, 0, sizeof(pv));
    memset(pv_length, 0, sizeof(pv_length));

    int score = 0;
    int previous_score = 0;
    nodes_searched = 0;
    tt_hits = 0;
    tt_misses = 0;
    min_nmp_ply = 0;

    global_age++;

    auto start_time = std::chrono::steady_clock::now();
    for (int depth = 1; depth <= max_depth; ++depth) {
        if (depth < 5) {
            score = AlphaBeta(board, -INFINITE, INFINITE, depth, 0, start_time, true);

            if (time_over) break;
        } else {
            
            // Aspiration windows
            // Take the previous score and create a window around it.
            // If the score falls outside the window, widen the window and try again.
            int delta = 15 + previous_score * previous_score / 16384; 
            int alpha = std::max(previous_score - delta, -INFINITE);
            int beta  = std::min(previous_score + delta, INFINITE);

            while (true) {
                score = AlphaBeta(board, alpha, beta, depth, 0, start_time, true);
                if (time_over) break;

                if (score <= alpha) {
                    beta = (alpha + beta) / 2;
                    alpha = std::max(alpha - delta, -INFINITE);
                } else if (score >= beta) {
                    beta = std::min(beta + delta, INFINITE);
                } else {
                    break;
                }

                delta += delta / 2;
            }
        }
        if (time_over) break;
        best_move = pv[0][0];

        auto now = std::chrono::steady_clock::now();
        previous_score = score;

        std::cout << stats(depth, score, start_time, now) << std::endl;
    }
    
    std::cout << "bestmove " << chess::uci::moveToUci(best_move) << std::endl;
}