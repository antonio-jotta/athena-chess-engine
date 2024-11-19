#ifndef SEARCH_H
#define SEARCH_H

#include "board.h"
#include "move.h"
#include "move_generator.h"
#include "evaluation.h"
#include <vector>
#include <algorithm>
#include <chrono>

class Search {
public:
    static Move findBestMove(Board& board, int depth);
    static long long nodes_searched; // Counter for leaf nodes
private:
    static int negamax(Board& board, int depth, int alpha, int beta);
    static int quiescence(Board& board, int alpha, int beta);
    static int scoreMove(const Move& move, const Board& board);
    static void orderMoves(std::vector<Move>& move_list, Board& board);
};


#endif
