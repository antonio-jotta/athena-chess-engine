#include "board.h"
#include "evaluation.h"
#include <vector>
#include <iostream>
#include <cassert>
#include <chrono>


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



void testEvaluationWithFEN(const std::string& fen, const std::string& description) {
    Board board;
    board.loadFEN(fen);
    std::cout << "Test: " << description << "\n";
    board.printBoard();
    auto start = std::chrono::high_resolution_clock::now();

    int score = Evaluation::evaluatePosition(board);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "Evaluation duration in microseconds: " << duration.count() << "\n";
    std::cout << "Board Evaluation: " << score << "\n\n";
}

void testPassedPawn();
void testConnectedPawn();
void testIsolatedPawn();
void testDoubledPawn();
void testIsolatedPawns();

void testPawnEvaluations(){
    testPassedPawn();
    testConnectedPawn();
    testIsolatedPawn();
    testDoubledPawn();
    testIsolatedPawns();
}

int main() {

    // testEvaluationOnlyKings();
    // testEvaluationMaterialImbalance();
    // testEvaluationPawnStructure();
    // testEvaluationMobilityAdvantage();

    std::cout << "All evaluation tests completed.\n";
    

    testEvaluationWithFEN(
        "r1bqkbnr/pppppppp/n7/8/8/N7/PPPPPPPP/R1BQKBNR w KQkq - 0 1",
        "Knight on a3 vs. knight on a6"
    );

    testEvaluationWithFEN(
        "rnbqkb1r/pppppppp/5n2/8/8/5N2/PPPPPPPP/RNBQKB1R w KQkq - 0 1",
        "Knights developed on f3 and f6"
    );

    testEvaluationWithFEN(
        "7n/3pK1p1/3P1PRn/7Q/1P1k1r1B/P7/6P1/6R1 w - - 0 1", 
        "Mate in 5 for white"
    );

    testEvaluationWithFEN(
        "4B3/2p2P2/N4P1q/p1P5/8/6Pk/1K3p1P/r3r1R1 w - - 0 1",
        "Mate in 6 for black"
    );

    testEvaluationWithFEN(
        "7k/8/5RK1/8/8/8/8/8 b - - 0 1",
        "Mate in one - White to play"
    );

    Board board;
    board.loadFEN("r1bqkbnr/pppppppp/n7/8/8/N7/PPPPPPPP/R1BQKBNR w KQkq - 0 1");
    std::string fen = board.generateFEN();
    std::cout << "Generated FEN: " << fen << std::endl;

    // Add more test cases as needed...
    // testPawnEvaluations();

    std::cout << "All FEN evaluation tests completed.\n";
    return 0;
}


void testPassedPawn() {
    Board board;
    board.resetBoard();

    // Clear all pieces
    for (int i = 0; i < 12; ++i) {
        board.bitboards[i] = 0ULL;
    }

    // Place a white pawn at d5 and a black pawn at d7
    set_bit(board.bitboards[WHITE_PAWN], D5);
    set_bit(board.bitboards[BLACK_PAWN], D7);
    board.updateOccupancies();

    std::cout << "Test: Passed Pawn\n";
    board.printBoard();

    // Check if d5 is a passed pawn (should not be due to black pawn on d7)
    assert(!Evaluation::isPassedPawn(board, D5, WHITE));

    // Move black pawn to b7 and test if d5 becomes a passed pawn
    clear_bit(board.bitboards[BLACK_PAWN], D7);
    set_bit(board.bitboards[BLACK_PAWN], B7);
    board.updateOccupancies();

    // Now d5 should be a passed pawn
    assert(Evaluation::isPassedPawn(board, D5, WHITE));
    std::cout << "Passed pawn test passed.\n\n";
}

void testConnectedPawn() {
    Board board;
    board.resetBoard();

    // Clear all pieces
    for (int i = 0; i < 12; ++i) {
        board.bitboards[i] = 0ULL;
    }

    // Place white pawns at d4 and e4, making them connected
    set_bit(board.bitboards[WHITE_PAWN], D4);
    set_bit(board.bitboards[WHITE_PAWN], E4);
    board.updateOccupancies();

    std::cout << "Test: Connected Pawn\n";
    board.printBoard();

    // Check if d4 and e4 are connected pawns
    assert(Evaluation::isConnectedPawn(board, D4, WHITE));
    assert(Evaluation::isConnectedPawn(board, E4, WHITE));

    // Remove the e4 pawn and check if d4 is still connected (it should not be)
    clear_bit(board.bitboards[WHITE_PAWN], E4);
    board.updateOccupancies();
    assert(!Evaluation::isConnectedPawn(board, D4, WHITE));
    std::cout << "Connected pawn test passed.\n\n";
}

void testIsolatedPawn() {
    Board board;
    board.resetBoard();

    // Clear all pieces
    for (int i = 0; i < 12; ++i) {
        board.bitboards[i] = 0ULL;
    }

    // Place a white pawn at d4 and ensure no pawns on adjacent files
    set_bit(board.bitboards[WHITE_PAWN], D4);
    board.updateOccupancies();

    std::cout << "Test: Isolated Pawn\n";
    board.printBoard();

    // d4 should be an isolated pawn
    assert(Evaluation::isIsolatedPawn(board, D4, WHITE));

    // Place another white pawn on c4 to make d4 no longer isolated
    set_bit(board.bitboards[WHITE_PAWN], C4);
    board.updateOccupancies();
    assert(!Evaluation::isIsolatedPawn(board, D4, WHITE));

    std::cout << "Isolated pawn test passed.\n\n";
}

void testDoubledPawn() {
    Board board;
    board.resetBoard();

    // Clear all pieces
    for (int i = 0; i < 12; ++i) {
        board.bitboards[i] = 0ULL;
    }

    // Place white pawns at d4 and d5, making them doubled
    set_bit(board.bitboards[WHITE_PAWN], D4);
    set_bit(board.bitboards[WHITE_PAWN], D5);
    board.updateOccupancies();

    std::cout << "Test: Doubled Pawn\n";
    board.printBoard();

    // Check if d4 and d5 are doubled pawns
    assert(Evaluation::isDoubledPawn(board, D4, WHITE));
    assert(Evaluation::isDoubledPawn(board, D5, WHITE));

    // Remove one of the pawns and check again
    clear_bit(board.bitboards[WHITE_PAWN], D5);
    board.updateOccupancies();
    assert(!Evaluation::isDoubledPawn(board, D4, WHITE));

    std::cout << "Doubled pawn test passed.\n\n";
}


void testIsolatedPawns(){

    Board board;
    board.resetBoard();

    // Clear the board and place kings and pawns
    for (int i = 0; i < 12; ++i) {
        board.bitboards[i] = 0ULL;
    }

    set_bit(board.bitboards[WHITE_PAWN], A7);
    set_bit(board.bitboards[BLACK_PAWN], D7);
    set_bit(board.bitboards[BLACK_PAWN], E7);

    board.side = WHITE;
    board.updateOccupancies();

    std::cout << "Test: Adversary Isolated Pawns\n";
    displayBoardAndEvaluation(board);

}
