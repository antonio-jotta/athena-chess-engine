#include "board.h"
#include "move_generator.h"
#include "move.h"
#include <vector>
#include <iostream>
#include <cassert>

std::string squareToAlgebraic(int square) {
    char file = 'a' + (square % 8);
    char rank = '1' + (square / 8);
    return std::string(1, file) + std::string(1, rank);
}


void testRookMovesEmptyBoard() {
    // Initialize board
    Board board;
    board.resetBoard();
    
    // Clear the board
    for (int i = 0; i < 12; ++i) {
        board.bitboards[i] = 0ULL;
    }
    board.updateOccupancies();
    
    // Place a white rook at D4 (square 27)
    set_bit(board.bitboards[WHITE_ROOK], 27);
    board.side = WHITE;
    board.updateOccupancies();
    
    // Generate moves
    MoveGenerator moveGenerator;
    std::vector<Move> move_list;
    moveGenerator.generateRookMoves(board, move_list);
    
    // Expected number of moves: 14
    size_t expected_move_count = 14;
    assert(move_list.size() == expected_move_count);

    // Optionally, print the moves
    std::cout << "Test 1: Empty Board Test\n";
    std::cout << "Generated moves (" << move_list.size() << "):\n";
    for (const Move& move : move_list) {
        // Convert move to notation and print
        std::cout << squareToAlgebraic(move.from_square) << " -> " << squareToAlgebraic(move.to_square) << "\n";
    }
    std::cout << "Test 1 passed.\n\n";
}

void testRookMovesBlockedByFriendly() {
    // Initialize board
    Board board;
    board.resetBoard();
    
    // Clear the board
    for (int i = 0; i < 12; ++i) {
        board.bitboards[i] = 0ULL;
    }
    board.updateOccupancies();
    
    // Place a white rook at D4
    set_bit(board.bitboards[WHITE_ROOK], D4);
    // Place friendly pieces at D6 and F4
    set_bit(board.bitboards[WHITE_PAWN], D6); // D6
    set_bit(board.bitboards[WHITE_PAWN], F4); // F4
    board.side = WHITE;
    board.updateOccupancies();
    
    // Generate moves
    MoveGenerator moveGenerator;
    std::vector<Move> move_list;
    moveGenerator.generateRookMoves(board, move_list);
    
    // Expected moves: 8 (as calculated above)
    size_t expected_move_count = 8;
    assert(move_list.size() == expected_move_count);

    // Print the moves for debugging
    std::cout << "Test 2: Blocked by Friendly Pieces\n";
    std::cout << "Generated moves (" << move_list.size() << "):\n";
    for (const Move& move : move_list) {
        std::cout << squareToAlgebraic(move.from_square) << " -> " << squareToAlgebraic(move.to_square) << "\n";
    }
    std::cout << "Test 2 passed.\n\n";
}


void testRookMovesCaptureOpponent() {
    // Initialize board
    Board board;
    board.resetBoard();
    
    // Clear the board
    for (int i = 0; i < 12; ++i) {
        board.bitboards[i] = 0ULL;
    }
    board.updateOccupancies();
    
    // Place a white rook at D4
    set_bit(board.bitboards[WHITE_ROOK], D4);
    // Place black pieces at D6 and F4
    set_bit(board.bitboards[BLACK_PAWN], D6);
    set_bit(board.bitboards[BLACK_PAWN], F4);
    board.side = WHITE;
    board.updateOccupancies();
    
    // Generate moves
    MoveGenerator moveGenerator;
    std::vector<Move> move_list;
    moveGenerator.generateRookMoves(board, move_list);
    
    // Expected moves: Up to and including D6
    size_t expected_move_count = 12;
    std::cout << "Generated moves (" << move_list.size() << "):\n";
    for (const Move& move : move_list) {
        std::cout << squareToAlgebraic(move.from_square) << " -> " << squareToAlgebraic(move.to_square) << "\n";
    }
    assert(move_list.size() == expected_move_count);

    std::cout << "Test 3: Capture Opponent Pieces\n";
    std::cout << "Generated moves (" << move_list.size() << "):\n";
    for (const Move& move : move_list) {
        std::string move_str = squareToAlgebraic(move.from_square) + " -> " + squareToAlgebraic(move.to_square);
        if (move.flags & FLAG_CAPTURE) {
            move_str += " x"; // Indicate capture
        }
        std::cout << move_str << "\n";
    }
    std::cout << "Test 3 passed.\n\n";
}

void testRookMovesEdgeOfBoard() {
    // Initialize board
    Board board;
    board.resetBoard();
    
    // Clear the board
    for (int i = 0; i < 12; ++i) {
        board.bitboards[i] = 0ULL;
    }
    board.updateOccupancies();
    
    // Place a white rook at A1 (square 0)
    set_bit(board.bitboards[WHITE_ROOK], A1);
    board.side = WHITE;
    board.updateOccupancies();
    
    // Generate moves
    MoveGenerator moveGenerator;
    std::vector<Move> move_list;
    moveGenerator.generateRookMoves(board, move_list);
    
    // Expected moves: Up and right from A1
    size_t expected_move_count = 14;
    assert(move_list.size() == expected_move_count);

    std::cout << "Test 4: Edge of Board Test\n";
    std::cout << "Generated moves (" << move_list.size() << "):\n";
    for (const Move& move : move_list) {
        std::cout << squareToAlgebraic(move.from_square) << " -> " << squareToAlgebraic(move.to_square) << "\n";
    }
    std::cout << "Test 4 passed.\n\n";
}

void runAllTests() {
    testRookMovesEmptyBoard();
    testRookMovesBlockedByFriendly();
    testRookMovesCaptureOpponent();
    testRookMovesEdgeOfBoard();
    // Add more tests as needed
}

int main() {
    runAllTests();
    return 0;
}
