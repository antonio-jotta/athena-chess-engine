#ifndef SEARCH_H
#define SEARCH_H

#include "board.h"
#include "move.h"
#include "move_generator.h"
#include "evaluation.h"
#include <vector>

class Search {
public:
    static Move findBestMove(Board& board, int depth);
private:
    static int minimax(Board& board, int depth, bool maximizingPlayer);
};

#endif 
