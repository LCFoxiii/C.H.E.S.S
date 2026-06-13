#include "evaluate.hpp"

static constexpr int32_t piece_values[5] = {S(232, 260), S(820, 627), S(915, 743), S(1390, 1235), S(4131, 3339)};
static constexpr int32_t PSQT_PAWN[48] = {S(-2, 351), S(150, 312), S(92, 330), S(85, 317), S(119, 339), S(320, 256), S(351, 247), S(97, 238), S(48, 301), S(140, 316), S(174, 247), S(153, 288), S(213, 278), S(228, 243), S(339, 231), S(148, 233), S(32, 348), S(147, 329), S(172, 260), S(265, 236), S(284, 234), S(239, 225), S(226, 270), S(80, 273), S(88, 423), S(227, 384), S(226, 320), S(309, 258), S(316, 244), S(265, 274), S(266, 329), S(92, 351), S(135, 648), S(205, 678), S(325, 551), S(332, 454), S(535, 368), S(568, 369), S(307, 555), S(126, 583), S(164, 581), S(429, 475), S(36, 439), S(325, 248), S(245, 312), S(516, 180), S(-96, 465), S(-409, 624)};
static constexpr int32_t PSQT_KNIGHT[64] = {S(588, 905), S(1090, 712), S(892, 891), S(1011, 915), S(1116, 868), S(1053, 887), S(1108, 716), S(1076, 665), S(1039, 799), S(907, 906), S(1128, 936), S(1174, 948), S(1182, 964), S(1273, 880), S(1103, 874), S(1103, 758), S(1078, 863), S(1144, 966), S(1250, 962), S(1237, 1040), S(1282, 1020), S(1279, 952), S(1307, 871), S(1115, 867), S(1125, 895), S(1212, 949), S(1256, 1055), S(1245, 1097), S(1317, 1049), S(1283, 1055), S(1281, 997), S(1150, 883), S(1125, 902), S(1269, 989), S(1275, 1083), S(1446, 1064), S(1372, 1069), S(1534, 1004), S(1278, 1001), S(1285, 887), S(959, 869), S(1477, 863), S(1360, 1017), S(1498, 999), S(1632, 916), S(1811, 892), S(1558, 851), S(1414, 754), S(802, 877), S(941, 961), S(1507, 842), S(1315, 969), S(1292, 930), S(1481, 832), S(1169, 869), S(1105, 740), S(221, 790), S(667, 836), S(952, 951), S(955, 847), S(1567, 785), S(675, 876), S(1065, 693), S(592, 571)};
static constexpr int32_t PSQT_BISHOP[64] = {S(1015, 867), S(1178, 923), S(1142, 834), S(1090, 941), S(1124, 921), S(1150, 868), S(1004, 952), S(1072, 895), S(1220, 891), S(1282, 864), S(1279, 925), S(1203, 953), S(1245, 968), S(1291, 912), S(1364, 869), S(1224, 820), S(1203, 903), S(1276, 942), S(1266, 997), S(1278, 1000), S(1266, 1018), S(1335, 958), S(1284, 929), S(1240, 888), S(1165, 933), S(1270, 962), S(1266, 1017), S(1316, 1045), S(1362, 988), S(1263, 997), S(1243, 943), S(1213, 918), S(1180, 945), S(1237, 995), S(1289, 1010), S(1433, 996), S(1381, 1017), S(1381, 989), S(1242, 961), S(1185, 968), S(1124, 964), S(1363, 911), S(1400, 948), S(1384, 942), S(1362, 938), S(1483, 955), S(1357, 956), S(1200, 972), S(1064, 926), S(1251, 936), S(1096, 995), S(1059, 933), S(1342, 935), S(1451, 896), S(1268, 935), S(972, 907), S(1027, 899), S(1187, 862), S(656, 965), S(883, 953), S(965, 964), S(966, 936), S(1132, 913), S(1148, 848)};
static constexpr int32_t PSQT_ROOK[64] = {S(1311, 1695), S(1344, 1736), S(1409, 1740), S(1455, 1726), S(1466, 1703), S(1395, 1692), S(1268, 1736), S(1309, 1611), S(1184, 1718), S(1344, 1690), S(1316, 1725), S(1358, 1735), S(1403, 1676), S(1446, 1672), S(1396, 1655), S(1080, 1738), S(1183, 1726), S(1302, 1727), S(1346, 1696), S(1322, 1722), S(1415, 1680), S(1400, 1661), S(1403, 1671), S(1263, 1658), S(1216, 1754), S(1285, 1754), S(1363, 1757), S(1405, 1735), S(1439, 1699), S(1373, 1695), S(1480, 1658), S(1298, 1682), S(1296, 1748), S(1370, 1734), S(1451, 1772), S(1552, 1706), S(1521, 1716), S(1589, 1698), S(1419, 1698), S(1327, 1739), S(1390, 1751), S(1518, 1738), S(1560, 1724), S(1587, 1726), S(1487, 1726), S(1705, 1650), S(1785, 1636), S(1510, 1681), S(1604, 1730), S(1593, 1747), S(1789, 1708), S(1800, 1707), S(1897, 1627), S(1858, 1657), S(1549, 1732), S(1669, 1695), S(1617, 1746), S(1706, 1715), S(1587, 1777), S(1820, 1710), S(1808, 1713), S(1435, 1773), S(1529, 1744), S(1597, 1724)};
static constexpr int32_t PSQT_QUEEN[64] = {S(1798, 1783), S(1739, 1789), S(1775, 1833), S(1871, 1678), S(1733, 1927), S(1673, 1815), S(1653, 1865), S(1571, 1751), S(1643, 1862), S(1771, 1844), S(1878, 1778), S(1825, 1864), S(1854, 1868), S(1890, 1813), S(1811, 1750), S(1832, 1770), S(1722, 1927), S(1838, 1775), S(1759, 2034), S(1805, 1973), S(1783, 2018), S(1818, 2041), S(1871, 1997), S(1821, 1997), S(1777, 1843), S(1663, 2132), S(1762, 2064), S(1757, 2201), S(1783, 2129), S(1796, 2117), S(1812, 2148), S(1801, 2061), S(1655, 2020), S(1680, 2090), S(1731, 2082), S(1733, 2189), S(1783, 2263), S(1858, 2174), S(1786, 2284), S(1798, 2156), S(1766, 1845), S(1743, 1975), S(1848, 1974), S(1800, 2232), S(1921, 2202), S(2155, 2030), S(2052, 2022), S(2095, 1941), S(1706, 1868), S(1603, 2080), S(1768, 2125), S(1798, 2178), S(1661, 2308), S(2135, 1992), S(1924, 2086), S(2077, 1901), S(1640, 1942), S(1769, 2091), S(1862, 2091), S(1858, 2099), S(2337, 1872), S(2406, 1772), S(2102, 1894), S(2023, 2032)};
static constexpr int32_t PSQT_KING[64] = {S(-24, -274), S(238, -216), S(85, -120), S(-335, -9), S(26, -142), S(-170, -33), S(178, -158), S(164, -274), S(77, -151), S(87, -67), S(-68, 44), S(-359, 109), S(-264, 111), S(-132, 59), S(86, -29), S(122, -113), S(60, -109), S(-31, -8), S(-162, 84), S(-329, 149), S(-318, 164), S(-272, 133), S(-62, 49), S(-149, -14), S(-315, -50), S(92, -33), S(-247, 141), S(-478, 196), S(-493, 211), S(-322, 165), S(-289, 99), S(-370, 6), S(55, -67), S(-45, 100), S(83, 102), S(-177, 156), S(-144, 141), S(-155, 182), S(1, 128), S(-329, 68), S(329, -25), S(323, 40), S(406, 38), S(117, 50), S(184, 49), S(513, 127), S(540, 117), S(-20, 55), S(868, -225), S(277, 26), S(161, 28), S(489, -16), S(215, 38), S(164, 142), S(-134, 128), S(-570, 160), S(-270, -314), S(807, -298), S(698, -212), S(397, -167), S(-557, 46), S(-312, 125), S(238, -17), S(283, -103)};
static constexpr int32_t ISOLATED_PAWN = S(-28, -17);
static constexpr int32_t PASSED_PAWNS[6] = {S(29, 8), S(18, 17), S(0, 50), S(16, 86), S(19, 185), S(463, 663)};

template <chess::Color::underlying color>
int32_t eval_all(const chess::Board& board, int& phase) {
    int32_t packed_score = 0;

    chess::Bitboard our_pawns   = board.pieces(chess::PieceType::PAWN, color);
    chess::Bitboard our_knights = board.pieces(chess::PieceType::KNIGHT, color);
    chess::Bitboard our_bishops = board.pieces(chess::PieceType::BISHOP, color);
    chess::Bitboard our_rooks   = board.pieces(chess::PieceType::ROOK, color);
    chess::Bitboard our_queens  = board.pieces(chess::PieceType::QUEEN, color);
    chess::Bitboard our_king    = board.pieces(chess::PieceType::KING, color);

    const int pawns_count = our_pawns.count();
    const int knights_count = our_knights.count();
    const int bishops_count = our_bishops.count();
    const int rooks_count = our_rooks.count();
    const int queens_count = our_queens.count();

    chess::Bitboard opp_pawns   = board.pieces(chess::PieceType::PAWN, ~color);

    // material score
    packed_score += piece_values[0] * pawns_count;
    packed_score += piece_values[1] * knights_count;
    packed_score += piece_values[2] * bishops_count;
    packed_score += piece_values[3] * rooks_count;
    packed_score += piece_values[4] * queens_count;

    // phase calculation
    phase += knights_count;
    phase += bishops_count;
    phase += rooks_count  * 2;
    phase += queens_count * 4;


    // isolated pawns
    chess::Bitboard isolated_pawns = ~FillBackward(FillForward(PawnAttacks<color>(our_pawns))) & our_pawns;
    packed_score += ISOLATED_PAWN * isolated_pawns.count();

    chess::Bitboard passed_pawns;
    if constexpr (color == chess::Color::WHITE) {
        passed_pawns = ~FillBackward(PawnAttacks<chess::Color::BLACK>(opp_pawns) | opp_pawns >> 8) & our_pawns;
    } else {
        passed_pawns = ~FillForward(PawnAttacks<chess::Color::WHITE>(opp_pawns) | opp_pawns << 8) & our_pawns;
    }
    
    // passed pawns
    while (passed_pawns) {
        chess::Square sq = passed_pawns.pop();
        packed_score += PASSED_PAWNS[sq.relative_square(color).rank() - 1];
    }

    // PSQT
    while (our_pawns) {
        chess::Square sq = our_pawns.pop();
        packed_score += PSQT_PAWN[sq.relative_square(color).index() - 8];
    }

    while (our_knights) {
        chess::Square sq = our_knights.pop();
        packed_score += PSQT_KNIGHT[sq.relative_square(color).index()];
    }

    while (our_bishops) {
        chess::Square sq = our_bishops.pop();
        packed_score += PSQT_BISHOP[sq.relative_square(color).index()];
    }

    while (our_rooks) {
        chess::Square sq = our_rooks.pop();
        packed_score += PSQT_ROOK[sq.relative_square(color).index()];
    }

    while (our_queens) {
        chess::Square sq = our_queens.pop();
        packed_score += PSQT_QUEEN[sq.relative_square(color).index()];
    }

    while (our_king) {
        chess::Square sq = our_king.pop();
        packed_score += PSQT_KING[sq.relative_square(color).index()];
    }

    return packed_score;
}

int16_t evaluate(const chess::Board& board) {
    int phase = 0;

    int32_t packed_score = eval_all<chess::Color::WHITE>(board, phase) - eval_all<chess::Color::BLACK>(board, phase);

    int16_t mg = mg_score(packed_score);
    int16_t eg = eg_score(packed_score);

    phase = std::min(phase, 24);

    int16_t score = (mg * phase + eg * (24 - phase)) / 24;

    return board.sideToMove() == chess::Color::WHITE ? score : -score;
}