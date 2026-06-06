#pragma once
#include "include/chess.hpp"

constexpr int32_t S(const int32_t mg, const int32_t eg) {
    return static_cast<int32_t>(static_cast<uint32_t>(eg) << 16) + mg;
}

static constexpr int16_t mg_score(const int32_t packed_score) {
    return static_cast<int16_t>(packed_score);
}

static constexpr int16_t eg_score(const int32_t packed_score) {
    return static_cast<int16_t>((packed_score + 0x8000) >> 16);
}

int16_t evaluate(const chess::Board& board);