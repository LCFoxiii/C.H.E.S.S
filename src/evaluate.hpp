#pragma once
#include "include/chess.hpp"

template <chess::Color::underlying color>
inline chess::Bitboard PawnAttacks(chess::Bitboard bb) {
    return chess::attacks::pawnLeftAttacks<color>(bb) | chess::attacks::pawnRightAttacks<color>(bb);
}

inline chess::Bitboard FillForward(chess::Bitboard bb) {
    bb |= (bb << 8);
    bb |= (bb << 16);
    bb |= (bb << 32);
    return bb;
}

inline chess::Bitboard FillBackward(chess::Bitboard bb) {
    bb |= (bb >> 8);
    bb |= (bb >> 16);
    bb |= (bb >> 32);
    return bb;
}

constexpr int32_t S(const int32_t mg, const int32_t eg) {
    return static_cast<int32_t>(static_cast<uint32_t>(eg) << 16) + mg;
}

static constexpr int16_t mg_score(const int32_t packed_score) {
    return static_cast<int16_t>(packed_score);
}

static constexpr int16_t eg_score(const int32_t packed_score) {
    return static_cast<int16_t>((packed_score + 0x8000) >> 16);
}

inline chess::Bitboard rook_xray(const chess::Board& board, chess::Square sq, chess::Color color) {
    chess::Bitboard our_rooks = board.pieces(chess::PieceType::ROOK, color);
    chess::Bitboard queens    = board.pieces(chess::PieceType::QUEEN);
    chess::Bitboard occ = board.occ() & ~(our_rooks | queens);

    return chess::attacks::rook(sq, occ);
}

inline chess::Bitboard bishop_xray(const chess::Board& board, chess::Square sq, chess::Color color) {
    chess::Bitboard queens    = board.pieces(chess::PieceType::QUEEN);
    chess::Bitboard occ = board.occ() & ~queens;

    return chess::attacks::bishop(sq, occ);
}

// will be needed for mobility area.
template <chess::Color::underlying color, bool mobility_mode = false>
inline chess::Bitboard get_blocked_pawns(const chess::Board& board) {
    chess::Bitboard our_pawns = board.pieces(chess::PieceType::PAWN, color);
    chess::Bitboard opp_pawns = board.pieces(chess::PieceType::PAWN, ~color);
    chess::Bitboard mask;

    // only check the pawns that are on the 5th and 6th rank for white,
    // and the 3rd and 4th rank for black.
    // if mobility_mode is true, we also check the 4th rank for white,
    // and the 5th rank for black.
    if constexpr (color == chess::Color::WHITE) {
        if constexpr (!mobility_mode) {
            mask = (((chess::Bitboard(chess::Rank::RANK_5) | chess::Bitboard(chess::Rank::RANK_6)) & our_pawns) << 8) & opp_pawns;
        } else {
            mask = (((chess::Bitboard(chess::Rank::RANK_4) | chess::Bitboard(chess::Rank::RANK_5) | chess::Bitboard(chess::Rank::RANK_6)) & our_pawns) << 8) & (our_pawns | opp_pawns);
        }
    } else {
        if constexpr (!mobility_mode) {
            mask = (((chess::Bitboard(chess::Rank::RANK_3) | chess::Bitboard(chess::Rank::RANK_4)) & our_pawns) >> 8) & opp_pawns;
        } else {
            mask = (((chess::Bitboard(chess::Rank::RANK_3) | chess::Bitboard(chess::Rank::RANK_4) | chess::Bitboard(chess::Rank::RANK_5)) & our_pawns) >> 8) & (our_pawns | opp_pawns);
        }
    }

    return mask;
}

template <chess::Color::underlying color>
inline chess::Bitboard get_mobility_area(const chess::Board& board) {
    chess::Bitboard opp_pawn = board.pieces(chess::PieceType::PAWN, ~color);
    chess::Bitboard our_pawn = board.pieces(chess::PieceType::PAWN, color);
    chess::Bitboard area;

    // discard king and queen squares from mobility area.
    area |= board.pieces(chess::PieceType::KING, color);
    area |= board.pieces(chess::PieceType::QUEEN, color);

    // discard squares attacked by opponent pawns from mobility area.
    area |= PawnAttacks<~color>(opp_pawn);
    
    // discard squares blocked by our pawns from mobility area.
    // and discard any pawns on the 2nd and 3rd rank for white,
    // or the 6th and 7th rank for black.
    if constexpr (color == chess::Color::WHITE) {
        area |= get_blocked_pawns<chess::Color::WHITE, true>(board) >> 8;
        area |= (chess::Bitboard(chess::Rank::RANK_2) | chess::Bitboard(chess::Rank::RANK_3)) & our_pawn;
    } else {
        area |= get_blocked_pawns<chess::Color::BLACK, true>(board) << 8;
        area |= (chess::Bitboard(chess::Rank::RANK_6) | chess::Bitboard(chess::Rank::RANK_7)) & our_pawn;
    }

    // flip the bits to get the mobility area.
    return ~area;
}

int16_t evaluate(const chess::Board& board);