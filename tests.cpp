#include "board.h"
#include "move_generator.h"
#include "move.h"
#include <vector>
#include <iostream>
#include <cassert>

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
    size_t expected_move_count = 10;
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
    std::cout << "Test 4: Edge of Board Test\n";
    std::cout << "Generated moves (" << move_list.size() << "):\n";
    for (const Move& move : move_list) {
        std::cout << squareToAlgebraic(move.from_square) << " -> " << squareToAlgebraic(move.to_square) << "\n";
    }
    assert(move_list.size() == expected_move_count);
    std::cout << "Test 4 passed.\n\n";
}




void runAllTests() {
    testRookMovesEmptyBoard();
    testRookMovesBlockedByFriendly();
    testRookMovesCaptureOpponent();
    testRookMovesEdgeOfBoard();
    // Add more tests as needed
}


void testRookLegalMovesKingSafe() {
    // Initialize board
    Board board;
    board.resetBoard();

    // Clear the board
    for (int i = 0; i < 12; ++i) {
        board.bitboards[i] = 0ULL;
    }
    board.updateOccupancies();

    // Place a white rook at D4
    set_bit(board.bitboards[WHITE_ROOK], 27);
    // Place white king at E1 (square 4)
    set_bit(board.bitboards[WHITE_KING], 4);
    board.side = WHITE;
    board.updateOccupancies();

    // Generate legal moves
    MoveGenerator moveGenerator;
    std::vector<Move> move_list;
    moveGenerator.generateAllLegalMoves(board, move_list);

    // Expected moves: All rook moves from D4
    size_t expected_move_count = 14;
    assert(move_list.size() == expected_move_count);

    // Optionally, print the moves
    std::cout << "Test: Rook Legal Moves When King Is Safe\n";
    std::cout << "Generated moves (" << move_list.size() << "):\n";
    for (const Move& move : move_list) {
        std::cout << squareToAlgebraic(move.from_square) << " -> " << squareToAlgebraic(move.to_square) << "\n";
    }
    std::cout << "Test passed.\n\n";
}

void testRookLegalMovesKingInCheckBlock() {
    // Initialize board
    Board board;
    board.resetBoard();

    // Clear the board
    for (int i = 0; i < 12; ++i) {
        board.bitboards[i] = 0ULL;
    }
    board.updateOccupancies();

    // Place white king at E1 (square 4)
    set_bit(board.bitboards[WHITE_KING], 4);
    // Place white rook at E2 (square 12)
    set_bit(board.bitboards[WHITE_ROOK], 12);
    // Place black rook at E8 (square 60), putting the white king in check
    set_bit(board.bitboards[BLACK_ROOK], 60);
    board.side = WHITE;
    board.updateOccupancies();

    // Generate legal moves
    MoveGenerator moveGenerator;
    std::vector<Move> move_list;
    moveGenerator.generateAllLegalMoves(board, move_list);

    // Expected moves:
    // - Rook moves from E2 to any square that blocks the check (E3, E4, E5, E6, E7)
    // - Note: E1 is occupied by the king, so the rook cannot move there
    size_t expected_move_count = 5; // E2->E3, E2->E4, E2->E5, E2->E6, E2->E7
    assert(move_list.size() == expected_move_count);

    // Optionally, print the moves
    std::cout << "Test: Rook Legal Moves When King Is in Check and Rook Can Block\n";
    std::cout << "Generated moves (" << move_list.size() << "):\n";
    for (const Move& move : move_list) {
        std::cout << squareToAlgebraic(move.from_square) << " -> " << squareToAlgebraic(move.to_square) << "\n";
    }
    std::cout << "Test passed.\n\n";
}

void testRookLegalMovesKingInCheckCapture() {
    // Initialize board
    Board board;
    board.resetBoard();

    // Clear the board
    for (int i = 0; i < 12; ++i) {
        board.bitboards[i] = 0ULL;
    }
    board.updateOccupancies();

    // Place white king at E1 (square 4)
    set_bit(board.bitboards[WHITE_KING], 4);
    // Place white rook at A4 (square 24)
    set_bit(board.bitboards[WHITE_ROOK], 24);
    // Place black rook at E4 (square 28), putting the white king in check along the rank
    set_bit(board.bitboards[BLACK_ROOK], 28);
    board.side = WHITE;
    board.updateOccupancies();

    // Generate legal moves
    MoveGenerator moveGenerator;
    std::vector<Move> move_list;
    moveGenerator.generateAllLegalMoves(board, move_list);

    // Expected moves:
    // - Rook captures the attacking rook: A4->E4
    size_t expected_move_count = 1; // Only the capturing move
    assert(move_list.size() == expected_move_count);

    // Optionally, print the moves
    std::cout << "Test: Rook Legal Moves When King Is in Check and Rook Can Capture Attacker\n";
    std::cout << "Generated moves (" << move_list.size() << "):\n";
    for (const Move& move : move_list) {
        std::cout << squareToAlgebraic(move.from_square) << " -> " << squareToAlgebraic(move.to_square);
        if (move.flags & FLAG_CAPTURE) {
            std::cout << " x"; // Indicate capture
        }
        std::cout << "\n";
    }
    std::cout << "Test passed.\n\n";
}

void testRookLegalMovesKingInCheckRookCannotHelp() {
    // Initialize board
    Board board;
    board.resetBoard();

    // Clear the board
    for (int i = 0; i < 12; ++i) {
        board.bitboards[i] = 0ULL;
    }
    board.updateOccupancies();

    // Place white king at E1 (square 4)
    set_bit(board.bitboards[WHITE_KING], 4);
    // Place white rook at A1 (square 0)
    set_bit(board.bitboards[WHITE_ROOK], 0);
    // Place black bishop at H5 (square 39), putting the white king in check diagonally
    set_bit(board.bitboards[BLACK_BISHOP], 39);
    board.side = WHITE;
    board.updateOccupancies();

    // Generate legal moves
    MoveGenerator moveGenerator;
    std::vector<Move> move_list;
    moveGenerator.generateAllLegalMoves(board, move_list);

    // Expected moves:
    // - Since the rook cannot block a diagonal check or capture the bishop, no rook moves are legal
    size_t expected_move_count = 0; // No rook moves
    assert(move_list.size() == expected_move_count);

    // Optionally, print the moves
    std::cout << "Test: Rook Legal Moves When King Is in Check and Rook Cannot Help\n";
    std::cout << "Generated moves (" << move_list.size() << "):\n";
    for (const Move& move : move_list) {
        std::cout << squareToAlgebraic(move.from_square) << " -> " << squareToAlgebraic(move.to_square) << "\n";
    }
    std::cout << "Test passed.\n\n";
}


void runAllLegalMoveTests() {
    testRookLegalMovesKingSafe();
    testRookLegalMovesKingInCheckBlock();
    testRookLegalMovesKingInCheckCapture();
    testRookLegalMovesKingInCheckRookCannotHelp();
}

int main() {
    runAllTests();
    runAllLegalMoveTests();
    return 0;
}
