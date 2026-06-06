#pragma once
#include "include/chess.hpp"
#include "search.hpp"

enum class TTFlag {EXACT, TT_BETA, TT_ALPHA};

struct TTEntry {
    uint64_t key;
    TTFlag flag;
    chess::Move move;

    int16_t score;
    uint8_t depth;
    uint8_t age;
};

// just to check the size of the tt entry.
constexpr int t = sizeof(TTEntry);

constexpr size_t TT_SIZE = 1 << 20;
inline TTEntry tt[TT_SIZE];
inline uint8_t global_age = 0;

inline int16_t CorrectScore(int16_t score, int search_ply) {
    if (score > MATE_IN_MAX_PLY) return score - search_ply;
    else if (score < MATED_IN_MAX_PLY) return score + search_ply;
    else return score;
}

inline int16_t CorrectForStore(int16_t score, int search_ply) {
    if (score > MATE_IN_MAX_PLY) return score + search_ply;
    else if (score < MATED_IN_MAX_PLY) return score - search_ply;
    else return score;
}

inline void storeTT(uint64_t key, int16_t score, TTFlag flag, uint8_t depth, chess::Move move) {
    size_t index = key % TT_SIZE;
    tt[index].move = move;

    // only store newer entries or entries with greater depth.
    // i could try making the age dynamic.
    // but not today.
    if (depth >= tt[index].depth || tt[index].age >= 5) {
        tt[index].key   = key;
        tt[index].score = score;
        tt[index].flag  = flag;
        tt[index].depth = depth;
        tt[index].age   = global_age;
    }
}

inline TTEntry* probeTT(uint64_t key) {
    size_t index = key % TT_SIZE;
    return &tt[index];
}

inline void clearTT() {
    global_age = 0;
    for (size_t index = 0; index < TT_SIZE; index++) {
        tt[index].key = 0;
        tt[index].score = 0;
        tt[index].flag = TTFlag::EXACT;
        tt[index].depth = 0;
        tt[index].move = chess::Move::NO_MOVE;
        tt[index].age = 0;
    }
}
