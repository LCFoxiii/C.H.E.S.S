#include "evaluate.hpp"

static constexpr int32_t piece_values[5] = {S(232, 257), S(820, 627), S(915, 743), S(1389, 1236), S(4132, 3308)};
static constexpr int32_t PSQT_PAWN[48] = {S(8, 400), S(160, 364), S(99, 373), S(92, 370), S(125, 381), S(326, 295), S(358, 293), S(103, 279), S(58, 345), S(151, 364), S(183, 286), S(160, 336), S(220, 314), S(235, 277), S(346, 274), S(154, 270), S(42, 393), S(158, 372), S(179, 301), S(271, 279), S(290, 271), S(245, 258), S(234, 310), S(86, 311), S(97, 484), S(236, 445), S(231, 381), S(315, 333), S(320, 294), S(265, 326), S(270, 381), S(96, 402), S(134, 783), S(197, 834), S(331, 714), S(345, 637), S(542, 547), S(540, 512), S(284, 709), S(111, 725), S(643, 1234), S(906, 1135), S(511, 1100), S(798, 911), S(732, 970), S(997, 840), S(377, 1125), S(69, 1277)};
static constexpr int32_t PSQT_KNIGHT[64] = {S(588, 887), S(1090, 698), S(890, 878), S(1011, 900), S(1115, 853), S(1051, 875), S(1108, 705), S(1070, 655), S(1039, 779), S(907, 890), S(1128, 921), S(1173, 933), S(1181, 950), S(1273, 862), S(1102, 861), S(1102, 744), S(1078, 848), S(1144, 952), S(1250, 946), S(1237, 1024), S(1281, 1005), S(1279, 936), S(1307, 858), S(1115, 852), S(1124, 878), S(1211, 935), S(1255, 1041), S(1245, 1083), S(1316, 1035), S(1283, 1040), S(1281, 982), S(1150, 869), S(1127, 891), S(1268, 975), S(1274, 1070), S(1445, 1051), S(1371, 1054), S(1533, 991), S(1278, 986), S(1285, 874), S(961, 854), S(1477, 848), S(1360, 1005), S(1499, 984), S(1632, 902), S(1810, 879), S(1557, 838), S(1412, 739), S(802, 864), S(935, 949), S(1504, 833), S(1312, 955), S(1291, 916), S(1478, 820), S(1170, 858), S(1104, 726), S(214, 788), S(665, 827), S(947, 942), S(955, 832), S(1565, 772), S(679, 862), S(1066, 678), S(595, 557)};
static constexpr int32_t PSQT_BISHOP[64] = {S(1016, 850), S(1179, 907), S(1142, 821), S(1089, 926), S(1126, 907), S(1150, 855), S(1005, 933), S(1074, 880), S(1222, 873), S(1282, 848), S(1278, 910), S(1203, 940), S(1245, 955), S(1292, 896), S(1364, 855), S(1223, 804), S(1203, 886), S(1276, 925), S(1267, 982), S(1279, 985), S(1266, 1004), S(1336, 945), S(1284, 913), S(1241, 870), S(1163, 918), S(1271, 946), S(1266, 1002), S(1316, 1029), S(1362, 973), S(1263, 983), S(1244, 930), S(1213, 902), S(1180, 932), S(1237, 982), S(1287, 998), S(1435, 981), S(1381, 1003), S(1382, 976), S(1242, 947), S(1183, 957), S(1124, 952), S(1361, 899), S(1400, 935), S(1386, 926), S(1360, 926), S(1479, 942), S(1356, 942), S(1198, 960), S(1062, 912), S(1251, 922), S(1094, 981), S(1060, 918), S(1340, 920), S(1453, 879), S(1265, 922), S(970, 893), S(1034, 882), S(1195, 847), S(668, 949), S(891, 938), S(963, 950), S(962, 921), S(1132, 898), S(1147, 836)};
static constexpr int32_t PSQT_ROOK[64] = {S(1313, 1669), S(1346, 1710), S(1412, 1715), S(1457, 1700), S(1468, 1676), S(1397, 1666), S(1271, 1709), S(1312, 1586), S(1186, 1689), S(1345, 1663), S(1317, 1700), S(1360, 1709), S(1405, 1651), S(1449, 1647), S(1396, 1630), S(1082, 1711), S(1186, 1697), S(1304, 1700), S(1350, 1668), S(1324, 1696), S(1417, 1653), S(1402, 1635), S(1405, 1644), S(1267, 1629), S(1219, 1726), S(1286, 1728), S(1367, 1730), S(1410, 1708), S(1439, 1674), S(1374, 1669), S(1481, 1632), S(1300, 1653), S(1301, 1720), S(1373, 1708), S(1453, 1748), S(1552, 1681), S(1522, 1691), S(1591, 1673), S(1419, 1673), S(1329, 1713), S(1395, 1724), S(1520, 1713), S(1563, 1699), S(1590, 1701), S(1487, 1702), S(1710, 1624), S(1794, 1609), S(1510, 1659), S(1604, 1705), S(1595, 1722), S(1793, 1682), S(1797, 1683), S(1901, 1600), S(1858, 1630), S(1549, 1706), S(1671, 1669), S(1620, 1721), S(1708, 1689), S(1590, 1751), S(1820, 1685), S(1811, 1686), S(1435, 1748), S(1536, 1715), S(1600, 1698)};
static constexpr int32_t PSQT_QUEEN[64] = {S(1798, 1774), S(1740, 1776), S(1775, 1821), S(1872, 1666), S(1733, 1914), S(1671, 1808), S(1654, 1850), S(1569, 1743), S(1644, 1843), S(1770, 1832), S(1878, 1765), S(1825, 1851), S(1854, 1857), S(1888, 1806), S(1810, 1742), S(1831, 1754), S(1723, 1909), S(1839, 1761), S(1760, 2019), S(1805, 1961), S(1784, 2002), S(1818, 2029), S(1871, 1986), S(1822, 1985), S(1777, 1828), S(1662, 2120), S(1762, 2053), S(1757, 2192), S(1783, 2118), S(1796, 2106), S(1812, 2140), S(1802, 2046), S(1655, 2008), S(1679, 2078), S(1730, 2071), S(1732, 2180), S(1783, 2253), S(1859, 2163), S(1788, 2269), S(1799, 2140), S(1766, 1836), S(1743, 1966), S(1846, 1965), S(1801, 2223), S(1922, 2189), S(2153, 2016), S(2055, 2006), S(2096, 1931), S(1710, 1850), S(1600, 2070), S(1768, 2115), S(1799, 2166), S(1659, 2296), S(2136, 1981), S(1926, 2073), S(2081, 1887), S(1641, 1927), S(1771, 2075), S(1865, 2078), S(1869, 2077), S(2330, 1866), S(2415, 1754), S(2105, 1883), S(2024, 2016)};
static constexpr int32_t PSQT_KING[64] = {S(-24, -288), S(238, -232), S(84, -135), S(-335, -23), S(25, -155), S(-171, -45), S(177, -171), S(163, -286), S(74, -166), S(84, -82), S(-69, 30), S(-359, 95), S(-264, 98), S(-131, 45), S(85, -43), S(120, -126), S(62, -125), S(-37, -23), S(-162, 70), S(-331, 136), S(-318, 151), S(-275, 121), S(-63, 36), S(-151, -26), S(-313, -66), S(83, -44), S(-242, 127), S(-467, 181), S(-479, 196), S(-316, 150), S(-290, 88), S(-367, -7), S(35, -71), S(-41, 90), S(76, 92), S(-161, 142), S(-131, 127), S(-157, 171), S(-1, 118), S(-336, 58), S(321, -31), S(322, 32), S(400, 31), S(131, 35), S(196, 38), S(529, 112), S(543, 111), S(-6, 46), S(870, -231), S(277, 17), S(182, 13), S(508, -30), S(233, 21), S(168, 135), S(-126, 121), S(-588, 158), S(-255, -318), S(816, -306), S(698, -216), S(416, -181), S(-556, 38), S(-301, 114), S(238, -21), S(292, -108)};
static constexpr int32_t ISOLATED_PAWN = S(-28, -8);

template <chess::Color::underlying color>
int32_t eval_all(const chess::Board& board, int& phase) {
    int32_t packed_score = 0;

    chess::Bitboard our_pawns   = board.pieces(chess::PieceType::PAWN, color);
    chess::Bitboard our_knights = board.pieces(chess::PieceType::KNIGHT, color);
    chess::Bitboard our_bishops = board.pieces(chess::PieceType::BISHOP, color);
    chess::Bitboard our_rooks   = board.pieces(chess::PieceType::ROOK, color);
    chess::Bitboard our_queens  = board.pieces(chess::PieceType::QUEEN, color);
    chess::Bitboard our_king    = board.pieces(chess::PieceType::KING, color);

    // isolated pawns
    chess::Bitboard isolated_pawns = ~FillBackward(FillForward(PawnAttacks<color>(our_pawns))) & our_pawns;
    while (isolated_pawns) {
        packed_score += ISOLATED_PAWN;
        (void)isolated_pawns.pop(); // silence
    }
    

    while (our_pawns) {
        packed_score += piece_values[0];
        chess::Square sq = our_pawns.pop();
        packed_score += PSQT_PAWN[sq.relative_square(color).index() - 8];
    }

    while (our_knights) {
        packed_score += piece_values[1];
        phase += 1;
        chess::Square sq = our_knights.pop();
        packed_score += PSQT_KNIGHT[sq.relative_square(color).index()];
    }

    while (our_bishops) {
        packed_score += piece_values[2];
        phase += 1;
        chess::Square sq = our_bishops.pop();
        packed_score += PSQT_BISHOP[sq.relative_square(color).index()];
    }

    while (our_rooks) {
        packed_score += piece_values[3];
        phase += 2;
        chess::Square sq = our_rooks.pop();
        packed_score += PSQT_ROOK[sq.relative_square(color).index()];
    }

    while (our_queens) {
        packed_score += piece_values[4];
        phase += 4;
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