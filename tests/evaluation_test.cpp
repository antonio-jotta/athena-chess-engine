#include <iostream>
#include <cassert>
#include "board.h"
#include "evaluation.h"

// Function to display the board and its evaluation
void displayBoardAndEvaluation(Board& board) {
    board.printBoard();
    int score = Evaluation::evaluatePosition(board);
    std::cout << "Board Evaluation: " << score << "\n\n";
}

// Test 1: Simple material evaluation with only kings on the board
void testEvaluationOnlyKings() {
    Board board;
    board.resetBoard();

    // Clear the board
    for (int i = 0; i < 12; ++i) {
        board.bitboards[i] = 0ULL;
    }
    set_bit(board.bitboards[WHITE_KING], E1);
    set_bit(board.bitboards[BLACK_KING], E8);
    board.side = WHITE;
    board.updateOccupancies();

    std::cout << "Test: Evaluation with Only Kings on the Board\n";
    displayBoardAndEvaluation(board);
}

// Test 2: Evaluation with a material imbalance (White has an extra queen)
void testEvaluationMaterialImbalance() {
    Board board;
    board.resetBoard();

    // Clear the board and place kings and a queen
    for (int i = 0; i < 12; ++i) {
        board.bitboards[i] = 0ULL;
    }
    set_bit(board.bitboards[WHITE_KING], E1);
    set_bit(board.bitboards[BLACK_KING], E8);
    set_bit(board.bitboards[WHITE_QUEEN], D1);
    board.side = WHITE;
    board.updateOccupancies();

    std::cout << "Test: Evaluation with Material Imbalance (White Queen)\n";
    displayBoardAndEvaluation(board);
}

// Test 3: Evaluation with pawns in a basic structure
void testEvaluationPawnStructure() {
    Board board;
    board.resetBoard();

    // Clear the board and place kings and pawns
    for (int i = 0; i < 12; ++i) {
        board.bitboards[i] = 0ULL;
    }
    set_bit(board.bitboards[WHITE_KING], E1);
    set_bit(board.bitboards[BLACK_KING], E8);
    set_bit(board.bitboards[WHITE_PAWN], D2);
    set_bit(board.bitboards[WHITE_PAWN], E2);
    set_bit(board.bitboards[BLACK_PAWN], D7);
    set_bit(board.bitboards[BLACK_PAWN], E7);
    board.side = WHITE;
    board.updateOccupancies();

    std::cout << "Test: Evaluation with Basic Pawn Structure\n";
    displayBoardAndEvaluation(board);
}

// Test 4: Evaluation with mobility advantage for White
void testEvaluationMobilityAdvantage() {
    Board board;
    board.resetBoard();

    // Clear the board and place kings and a few pieces for mobility testing
    for (int i = 0; i < 12; ++i) {
        board.bitboards[i] = 0ULL;
    }
    set_bit(board.bitboards[WHITE_KING], E1);
    set_bit(board.bitboards[BLACK_KING], E8);
    set_bit(board.bitboards[WHITE_ROOK], D4);
    set_bit(board.bitboards[BLACK_ROOK], H8);
    board.side = WHITE;
    board.updateOccupancies();

    std::cout << "Test: Evaluation with Mobility Advantage for White\n";
    displayBoardAndEvaluation(board);
}

// Main function to run all evaluation tests
int main() {
    testEvaluationOnlyKings();
    testEvaluationMaterialImbalance();
    testEvaluationPawnStructure();
    testEvaluationMobilityAdvantage();

    std::cout << "All evaluation tests completed.\n";
    return 0;
}
