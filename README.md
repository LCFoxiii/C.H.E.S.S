
# C.H.E.S.S - *C*learly *HE*'*S* *S*tupid

A UCI-compliant chess engine made in under **10MB** as a personal challenge of mine.

## Features
### Search
- [AlphaBeta pruning](https://www.chessprogramming.org/Alpha-Beta)
	- [Mate Distance Pruning](https://www.chessprogramming.org/Mate_Distance_Pruning)
	- [Transposition Table](https://www.chessprogramming.org/Transposition_Table)
	- [Principal Variation Search](https://www.chessprogramming.org/Principal_Variation_Search)
	- [Null Move Pruning](https://www.chessprogramming.org/Null_Move_Pruning)
- [Quiescence](https://www.chessprogramming.org/Quiescence_Search)
- [Iterative Deepening](https://www.chessprogramming.org/Iterative_Deepening)
	- [Aspiration Windows](https://www.chessprogramming.org/Aspiration_Windows)

### Evaluation
- [Tapered eval](https://www.chessprogramming.org/Tapered_Eval)
- [Piece eval](https://www.chessprogramming.org/Point_Value)
- [PSQT eval](https://www.chessprogramming.org/Piece-Square_Tables)

### Move Ordering
- TT Move
- MVV-LVA (with bad capture filtering)
- Promotions
- Killer moves
- History heuristic

that's it :3