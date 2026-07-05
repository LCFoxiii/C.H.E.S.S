#include "uci.hpp"
#include "tt.hpp"

uint64_t perft(chess::Board& board, int depth) {
    chess::Movelist moves;
    chess::movegen::legalmoves(moves, board);

    if (depth == 1) return moves.size();

    uint64_t nodes = 0;
    for (const chess::Move& move : moves) {
        board.makeMove(move);
        nodes += perft(board, depth - 1);
        board.unmakeMove(move);
    }

    return nodes;
}

void UCILoop() {
    std::string input;
    chess::Board board;
    chess::Board startpos_board;

    while (std::getline(std::cin, input) && input != "quit") {
        std::vector<std::string_view> tokens = chess::utils::splitString(input, ' ');

        if (tokens.empty()) continue;

        if (tokens[0] == "uci") {
            std::cout << "id name Chess\nid author LCFoxiii\nuciok" << std::endl;
        } else if (tokens[0] == "isready") {
            std::cout << "readyok" << std::endl;
        } else if (tokens[0] == "position") {
            if (tokens.size() < 2) continue;

            int start_of_moves = 0;

            if (tokens[1] == "startpos") {
                board.backtrackTo(startpos_board);
                start_of_moves = 2;
            } else if (tokens[1] == "fen") {

                // construct a fen, assuming the uci command is well formed
                std::string fen = tokens[2].data() + std::string(" ") + tokens[3].data() + std::string(" ") + tokens[4].data() + std::string(" ") + tokens[5].data() + std::string(" ") + tokens[6].data() + std::string(" ") + tokens[7].data();
                board.setFen(fen);
                start_of_moves = 8;
            }

            if (tokens[start_of_moves] == "moves") {
                for (size_t index = start_of_moves + 1; index < tokens.size(); index++) {
                    chess::Move move = chess::uci::uciToMove(board, tokens[index]);

                    if (board.isLegal(move)) board.makeMove(move);
                    else {
                        std::cerr << "Illegal move: " << tokens[index] << std::endl;
                        break;
                    }
                }
            }
        } else if (tokens[0] == "go") {
            int time = -1;
            int increment = 0;

            bool is_white = board.sideToMove() == chess::Color::WHITE;
            bool move_time_specified = false;

            max_depth = MAX_PLY;
            for (size_t index = 1; index < tokens.size(); index++) {
                if (tokens[index] == "wtime" && is_white) {
                    if (index + 1 < tokens.size()) time = is_white ? std::stoi(tokens[index + 1].data()) : 0;
                    index++;
                } else if (tokens[index] == "btime" && !is_white) {
                    if (index + 1 < tokens.size()) time = !is_white ? std::stoi(tokens[index + 1].data()) : 0;
                    index++;
                } else if (tokens[index] == "winc" && is_white) {
                    if (index + 1 < tokens.size()) increment = is_white ? std::stoi(tokens[index + 1].data()) : 0;
                    index++;
                } else if (tokens[index] == "binc" && !is_white) {
                    if (index + 1 < tokens.size()) increment = !is_white ? std::stoi(tokens[index + 1].data()) : 0;
                    index++;
                } else if (tokens[index] == "movetime") {
                    if (index + 1 < tokens.size()) {
                        time = std::stoi(tokens[index + 1].data());
                        move_time_specified = true;
                    }
                    index++;
                } else if (tokens[index] == "infinite") {
                    time = -1;
                    index++;
                } else if (tokens[index] == "depth") {
                    if (tokens[index] == "depth") {
                        if (index + 1 < tokens.size()) max_depth = std::stoi(tokens[index + 1].data());
                        index++;
                    }
                }
            }

            if (move_time_specified) time_limit = std::chrono::milliseconds(time);
            if (!move_time_specified && time > 0) time_limit = std::chrono::milliseconds(time / 20);
            if (!move_time_specified && increment > 0) time_limit += std::chrono::milliseconds(increment / 2);
            if (time == -1) time_limit = std::chrono::hours(24);

            time_over = false;
            if (search_thread.joinable()) search_thread.join();

            search_thread = std::thread(IterativeDeepening, std::ref(board));
        } else if (tokens[0] == "stop") {
            time_over = true;
            if (search_thread.joinable()) search_thread.join();
        } else if (tokens[0] == "ucinewgame") {
            board.backtrackTo(startpos_board);
            time_over = false;
            clearTT();
        } else if (tokens[0] == "p") {
            std::cout << board << std::endl;
        } else if (tokens[0] == "perft") {
            if (tokens.size() < 2) continue;

            int depth = std::stoi(tokens[1].data());

            auto start = std::chrono::high_resolution_clock::now();
            uint64_t nodes = perft(board, depth);
            auto end = std::chrono::high_resolution_clock::now();

            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            uint64_t nps = duration.count() > 0 ? (nodes * 1000) / duration.count() : 0;

            std::cout << "Perft(" << depth << ") = " << nodes << " nps " << nps << std::endl;
        } else {
            std::cerr << "Unknown command: " << tokens[0] << std::endl;
        }
    }
}
