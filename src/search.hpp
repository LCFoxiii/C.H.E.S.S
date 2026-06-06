#pragma once
#include "include/chess.hpp"
#include <cassert>
#include <chrono>
#include <atomic>
#include <thread>

using clk = std::chrono::steady_clock;

inline static constexpr int MAX_PLY  = 60;
inline static constexpr int INFINITE = 50000;
inline static constexpr int INVALID  = 50001;
inline static constexpr int MATE     = 32000;
inline static constexpr int DRAW     = 0;
inline static constexpr int MATE_IN_MAX_PLY  = MATE - MAX_PLY;
inline static constexpr int MATED_IN_MAX_PLY = -MATE_IN_MAX_PLY;

inline int max_depth = MAX_PLY;

inline static constexpr int mate_in(const int ply) {
    assert(ply >= 0);
    assert(ply <= MAX_PLY);

    return MATE - ply;
}

inline static constexpr int mated_in(const int ply) {
    assert(ply >= 0);
    assert(ply <= MAX_PLY);

    return ply - MATE;
}

inline chess::Move pv[MAX_PLY][MAX_PLY]{};
inline int pv_length[MAX_PLY]{};
inline uint64_t nodes_searched = 0;
inline std::atomic<bool> time_over(false);

// time limit in milliseconds
inline std::chrono::milliseconds time_limit(0);

inline std::string grabPV() {
    std::string output;

    for (int index = 0; index < pv_length[0]; index++) {
        output += chess::uci::moveToUci(pv[0][index]) + " ";
    }

    return output;
}

inline void addPV(const int search_ply, chess::Move move) {
    assert(search_ply >= 0);
    assert(search_ply < MAX_PLY);
    assert(move != chess::Move::NO_MOVE);

    if (time_over) return;

    pv[search_ply][search_ply] = move;

    for (int index = search_ply + 1; index < pv_length[search_ply + 1]; index++) {
        pv[search_ply][index] = pv[search_ply + 1][index];
    }

    pv_length[search_ply] = pv_length[search_ply + 1];
}

inline std::string scoreToString(int score) {
    if (score >= MATE_IN_MAX_PLY) {
        return "mate " +
            std::to_string(((MATE - score) / 2) + ((MATE - score) & 1));

    } else if (score <= MATED_IN_MAX_PLY) {
        return "mate " +
            std::to_string(
                -((MATE + score) / 2) + ((MATE + score) & 1)
            );

    } else {
        return "cp " + std::to_string(score);
    }
}

inline std::string stats(const int depth, const int score, const clk::time_point start_time, const clk::time_point end_time) {
    std::string output;

    int64_t time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

    output += "info depth " + std::to_string(depth);
    output += " score " + scoreToString(score);
    output += " nodes " + std::to_string(nodes_searched);
    output += " time " + std::to_string(time_ms);
    output += " nps " + std::to_string(nodes_searched / std::max(1, static_cast<int>(time_ms / 1000)));
    output += " pv " + grabPV();
    return output;
}

int Quiescence(chess::Board& board, int alpha, int beta, int search_ply, clk::time_point start_time);
int AlphaBeta(chess::Board& board, int alpha, int beta, int depth, int search_ply, clk::time_point start_time);
void IterativeDeepening(chess::Board& board);