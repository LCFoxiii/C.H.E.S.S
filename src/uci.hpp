#pragma once

#include "include/chess.hpp"
#include "search.hpp"

#include <vector>
#include <thread>

inline std::thread search_thread;

void UCILoop();