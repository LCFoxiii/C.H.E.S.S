#pragma once
#include "include/chess.hpp"
#include "search.hpp"

inline chess::Bitboard GetLeastValuablePieceSQ(const chess::Board& board, chess::Color color, chess::Bitboard attacks_bits, chess::PieceType &piece) {
    for (chess::PieceType p : {chess::PieceType::PAWN, chess::PieceType::KNIGHT, chess::PieceType::BISHOP, chess::PieceType::ROOK, chess::PieceType::QUEEN, chess::PieceType::KING}) {
        chess::Bitboard subsets = attacks_bits & board.pieces(p, color);
        if (subsets) {
            piece = p;
            return chess::Bitboard::fromSquare(subsets.pop());
        }
    }

    return 0;
}

inline int SEE(const chess::Board& board, chess::Move move) {
    int gain[32];
    int depth = 0;

    chess::Square from = move.from();
    chess::Square to = move.to();
    chess::PieceType target = board.at(to).type();
    chess::PieceType attacker = board.at(from).type();

    chess::Color stm = board.sideToMove();

    chess::Bitboard occ = board.occ();
    chess::Bitboard from_bb = chess::Bitboard::fromSquare(from);

    gain[0] = search_piece_values[static_cast<int>(target)];

    do {
        depth++;

        // CURRENT attacker value
        chess::PieceType current_attacker = attacker;

        gain[depth] =
            search_piece_values[static_cast<int>(current_attacker)]
            - gain[depth - 1];

        occ ^= from_bb;

        chess::Bitboard attackers =
            chess::attacks::attackers(board, chess::Color::WHITE, to, occ) |
            chess::attacks::attackers(board, chess::Color::BLACK, to, occ);

        stm = ~stm;

        from_bb =
            GetLeastValuablePieceSQ(
                board,
                stm,
                attackers,
                attacker
            );

    } while (from_bb);

    while (--depth) gain[depth - 1] = -std::max(-gain[depth - 1], gain[depth]);

    return gain[0];
}

inline bool isBadCapture(const chess::Board& board, chess::Move move) {
    chess::Square from = move.from();
    chess::Square to = move.to();
    if (search_piece_values[static_cast<int>(board.at<chess::PieceType>(from))] <= search_piece_values[static_cast<int>(board.at<chess::PieceType>(to))]) return false;
    return SEE(board, move) < 0;
}

template <bool inQs>
inline void ScoreMoves(const chess::Board& board, chess::Movelist& moves, uint8_t ply, chess::Move tt_move) {
    for (chess::Move& move : moves) {
        if constexpr (inQs) {
            chess::PieceType victim = board.at<chess::PieceType>(move.to());
            chess::PieceType attacker = board.at<chess::PieceType>(move.from());

            if (isBadCapture(board, move)) {
                move.setScore(-100);
                continue;
            }

            if (move.typeOf() == chess::Move::PROMOTION) {
                move.setScore(70'000);
                continue;
            }

            move.setScore(mvv_lva(attacker, victim) + 100'000);
            continue;
        } else {
            if (move == tt_move) {
                move.setScore(10'000'000);
                continue;
            } else if (move.typeOf() == chess::Move::PROMOTION) {
                move.setScore(70'000);
                continue;
            } else if (board.isCapture(move)) {
                chess::PieceType victim = board.at<chess::PieceType>(move.to());
                chess::PieceType attacker = board.at<chess::PieceType>(move.from());

                if (isBadCapture(board, move)) {
                    move.setScore(-100);
                    continue;
                }

                move.setScore(mvv_lva(attacker, victim) + 100'000);
                continue;
            } else if (!board.isCapture(move) && move.typeOf() != chess::Move::PROMOTION) {

                if (move == killer_moves[0][ply]) {
                    move.setScore(50'000);
                    continue;
                } else if (move == killer_moves[1][ply]) {
                    move.setScore(49'999);
                    continue;
                }

                bool color_index = static_cast<bool>(board.sideToMove());
                move.setScore(history_heuristic[color_index][move.from().index()][move.to().index()]);
            }
        }
    }
}

inline void PickMove(chess::Movelist& moves, int current_move_index) {
    int best_move_index = current_move_index;

    for (int move_index = current_move_index + 1; move_index < moves.size(); move_index++) {
        if (moves[move_index].score() > moves[best_move_index].score()) {
            best_move_index = move_index;
        }
    }

    if (best_move_index != current_move_index) {
        std::swap(moves[current_move_index], moves[best_move_index]);
    }
}