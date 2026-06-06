#include "search.hpp"
#include "evaluate.hpp"

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

    for (const chess::Move& move : moves) {
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

int AlphaBeta(chess::Board& board, int alpha, int beta, int depth, int search_ply, clk::time_point start_time) {
    assert(alpha >= -INFINITE);
    assert(beta <= INFINITE);

    assert(depth >= 0);
    assert(depth <= MAX_PLY);
    assert(search_ply >= 0);
    assert(search_ply <= MAX_PLY);

    pv_length[search_ply] = search_ply;
    
    // check time every 1024 nodes.
    if ((nodes_searched & 1023) == 0) {
        auto now = std::chrono::steady_clock::now();
        if (now - start_time >= time_limit) {
            time_over = true;
            return 0;
        }
    }

    if (time_over) return 0;

    if (depth == 0) return Quiescence(board, alpha, beta, search_ply, start_time);

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

    bool in_check  = board.inCheck();
    int best_score = -INFINITE;

    chess::Movelist moves;
    chess::movegen::legalmoves(moves, board);

    for (const chess::Move& move : moves) {
        nodes_searched++;

        board.makeMove(move);
        int score = -AlphaBeta(board, -beta, -alpha, depth - 1, search_ply + 1, start_time);
        board.unmakeMove(move);

        if (time_over) return 0;

        if (score > best_score) best_score = score;
        
        if (score > alpha) {
            alpha = score;
            addPV(search_ply, move);
        }

        if (alpha >= beta) {
            break;
        }
    }

    if (moves.empty()) {
        return in_check ? mated_in(search_ply) : DRAW;
    }

    return best_score;
}

void IterativeDeepening(chess::Board& board) {
    chess::Move best_move = chess::Move::NO_MOVE;

    memset(pv, 0, sizeof(pv));
    memset(pv_length, 0, sizeof(pv_length));

    int score = 0;
    int previous_score = 0;
    nodes_searched = 0;

    auto start_time = std::chrono::steady_clock::now();
    for (int depth = 1; depth <= max_depth; ++depth) {
        if (depth < 5) {
            score = AlphaBeta(board, -INFINITE, INFINITE, depth, 0, start_time);

            if (time_over) break;
        } else {
            
            // Aspiration windows
            // Take the previous score and create a window around it.
            // If the score falls outside the window, widen the window and try again.
            int delta = 15 + previous_score * previous_score / 16384; 
            int alpha = std::max(previous_score - delta, -INFINITE);
            int beta  = std::min(previous_score + delta, INFINITE);

            while (true) {
                score = AlphaBeta(board, alpha, beta, depth, 0, start_time);
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

    // I guess my logic is that since searching never goes past the time limit (with some noise of course)
    // the last completed search probably never went past the time limit.
    
    std::cout << "bestmove " << chess::uci::moveToUci(best_move) << std::endl;
}