#include "board.h"
#include "move_generator.h"
#include "move.h"
#include <iostream>

int main() {
    Board board;
    board.resetBoard();
    board.printBoard();

    std::vector<Move> move_list;
    MoveGenerator moveGenerator;
    moveGenerator.generateAllMoves(board, move_list);

    // Print the moves
    for (const Move& move : move_list) {
        // Convert move to notation (e.g., "e2e4") and print
    }

    return 0;
}
