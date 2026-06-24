#include "evaluate.hpp"

static constexpr int32_t piece_values[5] = {S(332, 251), S(1078, 786), S(1174, 932), S(1821, 1830), S(4138, 3333)};
static constexpr int32_t PSQT_PAWN[48] = {S(-87, 297), S(37, 234), S(12, 282), S(6, 239), S(41, 284), S(287, 202), S(282, 164), S(35, 174), S(-30, 236), S(21, 229), S(109, 181), S(74, 201), S(148, 212), S(179, 190), S(266, 143), S(91, 164), S(-42, 299), S(39, 264), S(108, 198), S(219, 150), S(238, 164), S(193, 171), S(138, 200), S(18, 211), S(91, 383), S(168, 320), S(216, 250), S(317, 145), S(335, 179), S(256, 212), S(219, 277), S(101, 309), S(304, 714), S(338, 662), S(434, 538), S(456, 351), S(639, 259), S(811, 367), S(469, 539), S(303, 611), S(303, 682), S(530, 582), S(188, 514), S(432, 335), S(237, 423), S(571, 318), S(-85, 598), S(-384, 784)};
static constexpr int32_t PSQT_KNIGHT[64] = {S(218, 689), S(821, 446), S(588, 656), S(725, 689), S(855, 627), S(785, 647), S(842, 449), S(811, 369), S(761, 558), S(602, 682), S(870, 712), S(921, 728), S(933, 743), S(1040, 651), S(839, 631), S(840, 497), S(809, 627), S(887, 746), S(1010, 746), S(1002, 834), S(1054, 812), S(1045, 732), S(1085, 633), S(852, 632), S(863, 671), S(968, 727), S(1019, 850), S(1007, 904), S(1090, 846), S(1051, 854), S(1049, 784), S(893, 655), S(861, 664), S(1035, 775), S(1037, 886), S(1248, 862), S(1159, 869), S(1349, 785), S(1045, 788), S(1056, 647), S(665, 632), S(1285, 627), S(1144, 802), S(1300, 788), S(1454, 688), S(1673, 658), S(1387, 607), S(1216, 488), S(472, 637), S(654, 732), S(1321, 591), S(1087, 752), S(1055, 705), S(1298, 576), S(903, 627), S(845, 470), S(-168, 510), S(328, 578), S(677, 709), S(682, 602), S(1358, 536), S(316, 646), S(797, 419), S(248, 282)};
static constexpr int32_t PSQT_BISHOP[64] = {S(638, 586), S(838, 660), S(794, 543), S(741, 671), S(780, 646), S(805, 586), S(631, 700), S(704, 625), S(884, 624), S(962, 590), S(964, 652), S(870, 687), S(920, 701), S(972, 648), S(1059, 588), S(896, 530), S(868, 640), S(964, 681), S(947, 748), S(961, 752), S(946, 769), S(1028, 694), S(968, 665), S(916, 618), S(823, 666), S(947, 706), S(944, 769), S(1012, 802), S(1062, 734), S(942, 745), S(918, 677), S(879, 646), S(852, 675), S(912, 738), S(982, 755), S(1142, 750), S(1084, 767), S(1079, 729), S(918, 696), S(847, 701), S(779, 700), S(1065, 641), S(1110, 684), S(1079, 677), S(1069, 669), S(1224, 683), S(1056, 695), S(868, 710), S(701, 660), S(901, 678), S(754, 735), S(697, 672), S(1044, 670), S(1170, 635), S(943, 674), S(597, 638), S(645, 628), S(840, 588), S(203, 701), S(471, 690), S(600, 702), S(578, 679), S(818, 642), S(798, 565)};
static constexpr int32_t PSQT_ROOK[64] = {S(815, 1070), S(859, 1116), S(934, 1121), S(986, 1103), S(1000, 1075), S(915, 1068), S(769, 1122), S(814, 967), S(668, 1103), S(861, 1066), S(818, 1104), S(868, 1115), S(927, 1043), S(971, 1037), S(925, 1017), S(546, 1121), S(666, 1114), S(817, 1109), S(856, 1074), S(826, 1101), S(943, 1051), S(929, 1030), S(938, 1039), S(759, 1031), S(709, 1143), S(791, 1141), S(866, 1147), S(929, 1118), S(968, 1074), S(898, 1067), S(1025, 1024), S(799, 1062), S(805, 1135), S(882, 1115), S(977, 1158), S(1095, 1083), S(1068, 1091), S(1135, 1070), S(950, 1066), S(841, 1120), S(912, 1134), S(1069, 1118), S(1108, 1103), S(1149, 1100), S(1028, 1101), S(1266, 1020), S(1383, 997), S(1073, 1038), S(1175, 1108), S(1160, 1128), S(1383, 1084), S(1412, 1078), S(1506, 990), S(1470, 1023), S(1117, 1114), S(1242, 1071), S(1176, 1131), S(1278, 1098), S(1137, 1170), S(1421, 1089), S(1399, 1096), S(963, 1163), S(1058, 1136), S(1161, 1107)};
static constexpr int32_t PSQT_QUEEN[64] = {S(1879, 1673), S(1817, 1682), S(1864, 1728), S(1977, 1546), S(1813, 1843), S(1749, 1688), S(1708, 1779), S(1633, 1619), S(1705, 1778), S(1859, 1735), S(1986, 1662), S(1925, 1765), S(1960, 1765), S(2005, 1691), S(1911, 1622), S(1936, 1662), S(1801, 1856), S(1938, 1664), S(1846, 1977), S(1902, 1892), S(1876, 1956), S(1913, 1973), S(1979, 1919), S(1921, 1928), S(1865, 1745), S(1734, 2084), S(1848, 1993), S(1838, 2161), S(1872, 2072), S(1894, 2058), S(1913, 2087), S(1893, 2000), S(1724, 1951), S(1751, 2036), S(1812, 2023), S(1812, 2147), S(1874, 2231), S(1955, 2133), S(1875, 2268), S(1890, 2115), S(1858, 1740), S(1836, 1885), S(1946, 1893), S(1883, 2202), S(2026, 2177), S(2308, 1972), S(2190, 1960), S(2233, 1858), S(1775, 1776), S(1653, 2028), S(1853, 2075), S(1883, 2147), S(1726, 2302), S(2271, 1926), S(2035, 2039), S(2238, 1806), S(1698, 1863), S(1835, 2059), S(1947, 2051), S(1924, 2077), S(2551, 1760), S(2545, 1698), S(2264, 1784), S(2167, 1973)};
static constexpr int32_t PSQT_KING[64] = {S(-23, -329), S(277, -250), S(94, -134), S(-408, -8), S(22, -164), S(-208, -37), S(203, -187), S(190, -326), S(92, -172), S(112, -71), S(-87, 60), S(-432, 133), S(-319, 136), S(-165, 78), S(95, -29), S(142, -132), S(70, -123), S(-23, -4), S(-196, 104), S(-387, 180), S(-376, 195), S(-313, 159), S(-65, 57), S(-173, -17), S(-370, -56), S(115, -41), S(-310, 170), S(-592, 237), S(-615, 254), S(-389, 197), S(-350, 115), S(-446, 8), S(132, -103), S(-59, 111), S(119, 111), S(-249, 186), S(-222, 172), S(-189, 208), S(14, 141), S(-371, 71), S(427, -47), S(384, 36), S(502, 32), S(110, 62), S(185, 55), S(607, 148), S(660, 119), S(-55, 55), S(1045, -287), S(358, 15), S(150, 34), S(566, -19), S(217, 54), S(204, 152), S(-163, 136), S(-640, 163), S(-350, -395), S(933, -369), S(842, -274), S(443, -205), S(-688, 47), S(-403, 146), S(276, -45), S(295, -138)};
static constexpr int32_t ISOLATED_PAWN = S(-97, -47);
static constexpr int32_t PASSED_PAWNS[6] = {S(61, 14), S(20, 54), S(-33, 166), S(-1, 264), S(-89, 381), S(383, 722)};
static constexpr int32_t BISHOP_PAIR = S(166, 261);
static constexpr int32_t DOUBLED_PAWN = S(-8, -81);
static constexpr int32_t BLOCKED_PAWNS[2] = {S(-91, -43), S(-195, -277)};

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

    chess::Bitboard our_pawns = board.pieces(chess::PieceType::PAWN, color);
    chess::Bitboard opp_pawns = board.pieces(chess::PieceType::PAWN, ~color);
    chess::Bitboard mask;

    // only check the pawns that are on the 5th and 6th rank for white,
    // and the 3rd and 4th rank for black.
    if constexpr (color == chess::Color::WHITE) {
        mask = (((chess::Bitboard(chess::Rank::RANK_5) | chess::Bitboard(chess::Rank::RANK_6)) & our_pawns) << 8) & opp_pawns;
    } else {
        mask = (((chess::Bitboard(chess::Rank::RANK_3) | chess::Bitboard(chess::Rank::RANK_4)) & our_pawns) >> 8) & opp_pawns;
    }

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
int32_t eval_all(const chess::Board& board) {
    int32_t packed_score = 0;

    packed_score += eval_material(board, color);
    packed_score += eval_psqt(board, color);
    packed_score += eval_pawn_structures<color>(board);
    packed_score += eval_bishop_pair(board, color);

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