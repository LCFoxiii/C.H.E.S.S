#include "evaluate.hpp"

static constexpr int32_t piece_values[5] = {S(352, 244), S(1132, 720), S(1236, 853), S(1887, 1720), S(4577, 3070)};
static constexpr int32_t PSQT_PAWN[48] = {S(35, 273), S(112, 218), S(60, 270), S(183, 199), S(202, 256), S(341, 195), S(348, 149), S(122, 153), S(39, 229), S(38, 230), S(115, 195), S(139, 189), S(219, 197), S(227, 186), S(266, 148), S(122, 165), S(-25, 296), S(-3, 272), S(102, 194), S(178, 151), S(207, 162), S(208, 163), S(107, 206), S(19, 213), S(78, 383), S(93, 327), S(143, 259), S(258, 146), S(257, 185), S(202, 216), S(135, 290), S(86, 308), S(283, 720), S(273, 678), S(334, 559), S(367, 368), S(542, 281), S(724, 385), S(418, 549), S(284, 615), S(261, 689), S(510, 585), S(163, 515), S(360, 351), S(212, 426), S(613, 314), S(-88, 597), S(-449, 793)};
static constexpr int32_t PSQT_KNIGHT[64] = {S(349, 765), S(941, 570), S(734, 648), S(876, 707), S(1022, 623), S(850, 666), S(957, 542), S(911, 503), S(846, 583), S(707, 669), S(869, 685), S(958, 689), S(985, 699), S(1030, 622), S(918, 641), S(934, 548), S(846, 631), S(846, 715), S(977, 697), S(958, 801), S(1043, 777), S(1030, 681), S(1042, 612), S(905, 658), S(914, 671), S(964, 683), S(991, 810), S(997, 861), S(1067, 808), S(1043, 816), S(1095, 737), S(966, 694), S(907, 663), S(1003, 732), S(939, 866), S(1184, 830), S(1099, 833), S(1254, 768), S(1024, 752), S(1100, 645), S(618, 637), S(1167, 610), S(973, 812), S(1134, 785), S(1300, 691), S(1539, 660), S(1312, 574), S(1192, 494), S(475, 667), S(619, 740), S(1278, 552), S(1002, 722), S(964, 683), S(1219, 549), S(902, 633), S(820, 510), S(-8, 581), S(360, 588), S(558, 725), S(656, 602), S(1250, 555), S(420, 624), S(756, 431), S(348, 349)};
static constexpr int32_t PSQT_BISHOP[64] = {S(763, 662), S(980, 690), S(966, 659), S(901, 675), S(942, 662), S(929, 664), S(772, 715), S(831, 677), S(971, 651), S(1010, 595), S(1000, 638), S(927, 663), S(965, 668), S(1026, 631), S(1100, 602), S(966, 591), S(913, 649), S(997, 658), S(966, 711), S(962, 716), S(988, 730), S(1063, 651), S(995, 653), S(966, 653), S(880, 651), S(936, 669), S(944, 720), S(990, 760), S(1036, 695), S(921, 702), S(917, 653), S(982, 643), S(816, 677), S(926, 687), S(893, 729), S(1078, 719), S(1002, 730), S(991, 705), S(942, 643), S(861, 702), S(731, 710), S(993, 627), S(990, 666), S(944, 662), S(955, 653), S(1090, 671), S(988, 671), S(844, 709), S(663, 700), S(813, 671), S(691, 715), S(618, 653), S(939, 655), S(1073, 626), S(853, 662), S(559, 677), S(716, 636), S(738, 610), S(93, 734), S(344, 706), S(553, 702), S(525, 677), S(690, 678), S(792, 590)};
static constexpr int32_t PSQT_ROOK[64] = {S(888, 1059), S(901, 1085), S(945, 1084), S(998, 1057), S(1036, 1031), S(990, 1047), S(927, 1059), S(1012, 937), S(785, 1105), S(930, 1061), S(911, 1085), S(983, 1089), S(1055, 1020), S(1053, 1026), S(1089, 988), S(761, 1102), S(754, 1123), S(875, 1107), S(921, 1063), S(912, 1085), S(1011, 1043), S(1024, 1028), S(1049, 1038), S(875, 1037), S(742, 1154), S(844, 1136), S(882, 1139), S(917, 1110), S(984, 1069), S(954, 1072), S(1105, 1027), S(869, 1074), S(834, 1136), S(887, 1115), S(984, 1151), S(1023, 1087), S(1063, 1091), S(1149, 1075), S(1033, 1062), S(904, 1124), S(873, 1143), S(1044, 1120), S(1028, 1113), S(1127, 1094), S(975, 1101), S(1269, 1021), S(1427, 987), S(1103, 1038), S(1021, 1133), S(1025, 1146), S(1231, 1096), S(1230, 1093), S(1355, 1007), S(1357, 1035), S(1069, 1121), S(1159, 1085), S(1031, 1142), S(1079, 1121), S(887, 1197), S(1156, 1120), S(1169, 1123), S(823, 1185), S(890, 1166), S(1045, 1112)};
static constexpr int32_t PSQT_QUEEN[64] = {S(2016, 1626), S(1953, 1652), S(1977, 1669), S(2057, 1557), S(1940, 1802), S(1927, 1721), S(1867, 1813), S(1779, 1687), S(1813, 1810), S(1909, 1705), S(2012, 1659), S(2000, 1703), S(2046, 1697), S(2053, 1711), S(1960, 1665), S(2018, 1744), S(1847, 1877), S(1972, 1650), S(1880, 1885), S(1930, 1825), S(1888, 1922), S(1940, 1969), S(1985, 2003), S(1964, 2044), S(1908, 1755), S(1711, 2040), S(1863, 1899), S(1823, 2048), S(1861, 1979), S(1882, 2028), S(1901, 2150), S(1908, 2130), S(1709, 2000), S(1768, 1957), S(1743, 1983), S(1758, 2050), S(1834, 2153), S(1904, 2088), S(1879, 2284), S(1890, 2226), S(1860, 1783), S(1783, 1892), S(1894, 1821), S(1785, 2163), S(1974, 2110), S(2276, 1955), S(2164, 1974), S(2213, 1958), S(1743, 1847), S(1564, 2029), S(1775, 2047), S(1812, 2118), S(1593, 2302), S(2165, 1930), S(1938, 2059), S(2168, 1935), S(1832, 1788), S(1783, 2049), S(1893, 2023), S(1841, 2055), S(2476, 1752), S(2426, 1755), S(2282, 1773), S(2162, 1978)};
static constexpr int32_t PSQT_KING[64] = {S(-101, -326), S(258, -250), S(108, -131), S(-391, -1), S(8, -119), S(-138, -44), S(271, -224), S(157, -331), S(17, -164), S(18, -55), S(-199, 77), S(-422, 125), S(-320, 129), S(-235, 89), S(30, -21), S(82, -128), S(35, -120), S(-71, 2), S(-263, 115), S(-419, 183), S(-438, 204), S(-367, 165), S(-139, 68), S(-213, -15), S(-403, -48), S(139, -42), S(-316, 171), S(-585, 234), S(-623, 256), S(-414, 199), S(-366, 115), S(-470, 8), S(102, -100), S(22, 100), S(121, 113), S(-199, 182), S(-215, 175), S(-199, 211), S(-10, 144), S(-387, 67), S(460, -56), S(434, 30), S(529, 29), S(107, 66), S(235, 52), S(651, 146), S(670, 116), S(-19, 47), S(1095, -289), S(452, 0), S(252, 23), S(573, -17), S(237, 52), S(234, 148), S(-71, 125), S(-664, 166), S(-390, -402), S(961, -368), S(910, -283), S(465, -208), S(-628, 46), S(-328, 132), S(340, -55), S(275, -142)};
static constexpr int32_t ISOLATED_PAWN = S(-102, -44);
static constexpr int32_t PASSED_PAWNS[6] = {S(27, 18), S(15, 52), S(-49, 173), S(-17, 274), S(-138, 393), S(216, 754)};
static constexpr int32_t BISHOP_PAIR = S(151, 280);
static constexpr int32_t DOUBLED_PAWN = S(-25, -76);
static constexpr int32_t BLOCKED_PAWNS[2] = {S(-67, -37), S(-160, -282)};
static constexpr int32_t MOBILITY_KNIGHT[9] = {S(-170, -432), S(-114, -123), S(-69, 15), S(-52, 60), S(21, 66), S(46, 112), S(80, 99), S(110, 98), S(164, 34)};
static constexpr int32_t MOBILITY_BISHOP[14] = {S(-279, -273), S(-183, -206), S(-90, -127), S(-74, -7), S(-16, 25), S(32, 58), S(62, 84), S(78, 96), S(97, 118), S(112, 111), S(218, 58), S(336, 53), S(279, 109), S(396, 33)};
static constexpr int32_t MOBILITY_ROOK[15] = {S(-591, -475), S(-231, -288), S(-164, -148), S(-142, -82), S(-131, 4), S(-104, 33), S(-76, 73), S(-20, 58), S(30, 85), S(105, 79), S(191, 74), S(258, 87), S(305, 103), S(338, 94), S(433, 57)};
static constexpr int32_t MOBILITY_QUEEN[28] = {0, S(-146, -996), S(-210, -698), S(-205, -461), S(-186, -434), S(-194, -476), S(-166, -321), S(-147, -266), S(-135, -152), S(-120, -85), S(-117, 2), S(-106, 50), S(-78, 61), S(-76, 135), S(-52, 159), S(-37, 201), S(-44, 260), S(-27, 264), S(-46, 328), S(78, 293), S(113, 277), S(96, 339), S(367, 200), S(-162, 557), S(883, 4), S(405, 191), S(425, 348), S(474, 175)};

int32_t eval_material(const chess::Board& board, const chess::Color color) {
    int32_t score = 0;

    chess::Bitboard our_pawns   = board.pieces(chess::PieceType::PAWN, color);
    chess::Bitboard our_knights = board.pieces(chess::PieceType::KNIGHT, color);
    chess::Bitboard our_bishops = board.pieces(chess::PieceType::BISHOP, color);
    chess::Bitboard our_rooks   = board.pieces(chess::PieceType::ROOK, color);
    chess::Bitboard our_queens  = board.pieces(chess::PieceType::QUEEN, color);

    // material score
    score += piece_values[0] * our_pawns.count();
    score += piece_values[1] * our_knights.count();
    score += piece_values[2] * our_bishops.count();
    score += piece_values[3] * our_rooks.count();
    score += piece_values[4] * our_queens.count();

    return score;
}

int32_t eval_psqt(const chess::Board& board, const chess::Color color) {
    int32_t score = 0;

    chess::Bitboard our_pawns   = board.pieces(chess::PieceType::PAWN, color);
    chess::Bitboard our_knights = board.pieces(chess::PieceType::KNIGHT, color);
    chess::Bitboard our_bishops = board.pieces(chess::PieceType::BISHOP, color);
    chess::Bitboard our_rooks   = board.pieces(chess::PieceType::ROOK, color);
    chess::Bitboard our_queens  = board.pieces(chess::PieceType::QUEEN, color);
    chess::Bitboard our_king    = board.pieces(chess::PieceType::KING, color);

    while (our_pawns) {
        chess::Square sq = our_pawns.pop();
        score += PSQT_PAWN[sq.relative_square(color).index() - 8];
    }

    while (our_knights) {
        chess::Square sq = our_knights.pop();
        score += PSQT_KNIGHT[sq.relative_square(color).index()];
    }

    while (our_bishops) {
        chess::Square sq = our_bishops.pop();
        score += PSQT_BISHOP[sq.relative_square(color).index()];
    }

    while (our_rooks) {
        chess::Square sq = our_rooks.pop();
        score += PSQT_ROOK[sq.relative_square(color).index()];
    }

    while (our_queens) {
        chess::Square sq = our_queens.pop();
        score += PSQT_QUEEN[sq.relative_square(color).index()];
    }

    while (our_king) {
        chess::Square sq = our_king.pop();
        score += PSQT_KING[sq.relative_square(color).index()];
    }

    return score;
}

template <chess::Color::underlying color>
int32_t eval_isolated_pawns(const chess::Board& board) {
    int32_t score = 0;

    chess::Bitboard our_pawns   = board.pieces(chess::PieceType::PAWN, color);

    chess::Bitboard isolated_pawns = ~FillBackward(FillForward(PawnAttacks<color>(our_pawns))) & our_pawns;
    score += ISOLATED_PAWN * isolated_pawns.count();

    return score;
}

template <chess::Color::underlying color>
int32_t eval_passed_pawns(const chess::Board& board) {
    int32_t score = 0;

    chess::Bitboard our_pawns   = board.pieces(chess::PieceType::PAWN, color);
    chess::Bitboard opp_pawns   = board.pieces(chess::PieceType::PAWN, ~color);

    chess::Bitboard passed_pawns;
    if constexpr (color == chess::Color::WHITE) {
        passed_pawns = ~FillBackward(PawnAttacks<chess::Color::BLACK>(opp_pawns) | opp_pawns >> 8) & our_pawns;
    } else {
        passed_pawns = ~FillForward(PawnAttacks<chess::Color::WHITE>(opp_pawns) | opp_pawns << 8) & our_pawns;
    }
    
    while (passed_pawns) {
        chess::Square sq = passed_pawns.pop();
        score += PASSED_PAWNS[sq.relative_square(color).rank() - 1];
    }

    return score;
}

template <chess::Color::underlying color>
int32_t eval_doubled_pawns(const chess::Board& board) {
    int32_t score = 0;

    chess::Bitboard our_pawns = board.pieces(chess::PieceType::PAWN, color);
    chess::Bitboard mask;
    
    if constexpr (color == chess::Color::WHITE) {
        mask = FillForward(our_pawns << 8) & our_pawns;
    } else {
        mask = FillBackward(our_pawns >> 8) & our_pawns;
    }

    score += DOUBLED_PAWN * mask.count();

    return score;
}

template <chess::Color::underlying color>
int32_t eval_blocked_pawns(const chess::Board& board) {
    int32_t score = 0;
    chess::Bitboard mask = get_blocked_pawns<color, false>(board);

    while (mask) {
        chess::Square sq = mask.pop();
        chess::Square blocked_pawn_square;

        if constexpr (color == chess::Color::WHITE) {
            blocked_pawn_square = sq - 8;
        } else {
            blocked_pawn_square = sq + 8;
        }

        score += BLOCKED_PAWNS[
            blocked_pawn_square.relative_square(color).rank() - 4
        ];
    }

    return score;
}

template <chess::Color::underlying color>
int32_t eval_pawn_structures(const chess::Board& board) {
    int32_t score = 0;

    score += eval_isolated_pawns<color>(board);
    score += eval_passed_pawns<color>(board);
    score += eval_doubled_pawns<color>(board);
    score += eval_blocked_pawns<color>(board);

    return score;
}

int32_t eval_bishop_pair(const chess::Board& board, const chess::Color color) {
    return BISHOP_PAIR * (board.pieces(chess::PieceType::BISHOP, color).count() >= 2);
}

int phase_accumulator(const chess::Board& board) {
    int phase = 0;
    
    chess::Bitboard all_bishops = board.pieces(chess::PieceType::BISHOP);
    chess::Bitboard all_knights = board.pieces(chess::PieceType::KNIGHT);
    chess::Bitboard all_rooks   = board.pieces(chess::PieceType::ROOK);
    chess::Bitboard all_queens  = board.pieces(chess::PieceType::QUEEN);

    phase += 1 * all_bishops.count();
    phase += 1 * all_knights.count();
    phase += 2 * all_rooks.count();
    phase += 4 * all_queens.count();

    return phase;
}

template <chess::Color::underlying color>
int32_t eval_mobility(const chess::Board& board) {
    int32_t score = 0;

    chess::Bitboard our_knights = board.pieces(chess::PieceType::KNIGHT, color);
    chess::Bitboard our_bishops = board.pieces(chess::PieceType::BISHOP, color);
    chess::Bitboard our_rooks   = board.pieces(chess::PieceType::ROOK, color);
    chess::Bitboard our_queens  = board.pieces(chess::PieceType::QUEEN, color);

    chess::Bitboard our_queens_copy = board.pieces(chess::PieceType::QUEEN, color);
    chess::Bitboard area = get_mobility_area<color>(board);
    const chess::Bitboard occ = board.occ();

    while (our_knights) {
        chess::Square sq = our_knights.pop();
        int mobility = ((chess::attacks::knight(sq) & area) & ~our_queens_copy).count();
        score += MOBILITY_KNIGHT[mobility];
    }

    while (our_bishops) {
        chess::Square sq = our_bishops.pop();
        int mobility = ((bishop_xray(board, sq, color) & area) & ~our_queens_copy).count();
        score += MOBILITY_BISHOP[mobility];
    }

    while (our_rooks) {
        chess::Square sq = our_rooks.pop();
        int mobility = (rook_xray(board, sq, color) & area).count();
        score += MOBILITY_ROOK[mobility];
    }

    while (our_queens) {
        chess::Square sq = our_queens.pop();
        int mobility = (chess::attacks::queen(sq, occ) & area).count();
        score += MOBILITY_QUEEN[mobility];
    }

    return score;
}

template <chess::Color::underlying color>
int32_t eval_all(const chess::Board& board) {
    int32_t packed_score = 0;

    packed_score += eval_material(board, color);
    packed_score += eval_psqt(board, color);
    packed_score += eval_pawn_structures<color>(board);
    packed_score += eval_bishop_pair(board, color);
    packed_score += eval_mobility<color>(board);

    return packed_score;
}

int16_t evaluate(const chess::Board& board) {
    int phase = phase_accumulator(board);

    int32_t packed_score = eval_all<chess::Color::WHITE>(board) - eval_all<chess::Color::BLACK>(board);

    int16_t mg = mg_score(packed_score);
    int16_t eg = eg_score(packed_score);

    phase = std::min(phase, 24);

    int16_t score = (mg * phase + eg * (24 - phase)) / 24;

    return board.sideToMove() == chess::Color::WHITE ? score : -score;
}