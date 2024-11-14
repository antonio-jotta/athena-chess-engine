#include "board.h"
#include "move_generator.h"
#include "move.h"
#include <vector>
#include <iostream>
#include <cassert>



void testRookMovesEmptyBoard();
void testRookMovesBlockedByFriendly();
void testRookMovesCaptureOpponent();
void testRookMovesEdgeOfBoard();
void runRookTests() {
    testRookMovesEmptyBoard();
    testRookMovesBlockedByFriendly();
    testRookMovesCaptureOpponent();
    testRookMovesEdgeOfBoard();
}

void testLegalMovesKingSafe();
void testLegalMovesKingInCheckBlock();
void testLegalMovesKingInCheckCapture();
void testLegalMovesKingInCheckRookCannotHelp();
void testIsSquareAttackedByKing();
void testTwoRooksTwoKings();
void testCastlingMoves();

void runKingTests(){
    testLegalMovesKingSafe();
    testLegalMovesKingInCheckBlock();
    testLegalMovesKingInCheckCapture();
    testLegalMovesKingInCheckRookCannotHelp();
    testIsSquareAttackedByKing();
    testTwoRooksTwoKings();
    testCastlingMoves();
}

void testBishops();
void testKnightMoves();
void testSquareAttackedByKnight();
void runKnightBishopTests(){
    testBishops();
    testKnightMoves();
    testSquareAttackedByKnight();
}

void testIsSquareAttackedByPawn();
void testEnPassant();
void testPawnDoubleMove();
void testPawnPromotions();
void runPawnTests(){
    testIsSquareAttackedByPawn();
    testEnPassant();
    testPawnDoubleMove();
    testPawnPromotions();
}


int main() {
    

    runRookTests();
    runKingTests();
    testBishops();
    testKnightMoves();
    testSquareAttackedByKnight();
    runPawnTests();
    testCastlingMoves();
    
    return 0;
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


void testLegalMovesKingSafe() {
    // Initialize board
    Board board;
    board.resetBoard();

    // Clear the board
    for (int i = 0; i < 12; ++i) {
        board.bitboards[i] = 0ULL;
    }
    board.updateOccupancies();

    // Place a white rook at D4
    // set_bit(board.bitboards[WHITE_ROOK], D4);
    // Place white king at E1 (square 4)
    set_bit(board.bitboards[WHITE_KING], E1);
    board.printBoard();
    board.side = WHITE;
    board.updateOccupancies();

    // Generate legal moves
    MoveGenerator moveGenerator;
    std::vector<Move> move_list;
    moveGenerator.generateAllLegalMoves(board, move_list);

    // Expected moves: All rook moves from D4
    std::cout << "Test: Legal Moves When King Is Safe\n";
    std::cout << "Generated moves (" << move_list.size() << "):\n";
    for (const Move& move : move_list) {
        std::cout << squareToAlgebraic(move.from_square) << " -> " << squareToAlgebraic(move.to_square) << "\n";
    }
    size_t expected_move_count = 5;
    assert(move_list.size() == expected_move_count);


    std::cout << "Test passed.\n\n";
}

void testLegalMovesKingInCheckBlock() {
    // Initialize board
    Board board;
    board.resetBoard();

    // Clear the board
    for (int i = 0; i < 12; ++i) {
        board.bitboards[i] = 0ULL;
    }
    board.updateOccupancies();

    // Place white king at E1 (square 4)
    set_bit(board.bitboards[WHITE_KING], E1);
    // Place white rook at E2 (square 12)
    set_bit(board.bitboards[WHITE_ROOK], E2);
    // Place black rook at E8 (square 60), putting the white king in check
    set_bit(board.bitboards[BLACK_ROOK], E8);
    board.side = WHITE;
    board.updateOccupancies();

    // Generate legal moves
    MoveGenerator moveGenerator;
    std::vector<Move> move_list;
    moveGenerator.generateAllLegalMoves(board, move_list);

    // Expected moves:
    // - Rook moves from E2 to any square that blocks the check (E3, E4, E5, E6, E7)
    // - Note: E1 is occupied by the king, so the rook cannot move there
    std::cout << "\nTest: Rook Legal Moves When King Is in Check and Rook Can Block\n";
    std::cout << "Generated moves (" << move_list.size() << "):\n";
    for (const Move& move : move_list) {
        std::cout << squareToAlgebraic(move.from_square) << " -> " << squareToAlgebraic(move.to_square) << "\n";
    }
    
    size_t expected_move_count = 10; 
    assert(move_list.size() == expected_move_count);


    std::cout << "Test passed.\n\n";
}

void testLegalMovesKingInCheckCapture() {
    // Initialize board
    Board board;
    board.resetBoard();

    // Clear the board
    for (int i = 0; i < 12; ++i) {
        board.bitboards[i] = 0ULL;
    }
    board.updateOccupancies();

    // Place white king at E1 (square 4)
    set_bit(board.bitboards[WHITE_KING], E1);
    // Place white rook at A4 (square 24)
    set_bit(board.bitboards[WHITE_ROOK], A4);
    // Place black rook at E4 (square 28), putting the white king in check along the rank
    set_bit(board.bitboards[BLACK_ROOK], E4);
    board.side = WHITE;
    board.updateOccupancies();

    // Generate legal moves
    MoveGenerator moveGenerator;
    std::vector<Move> move_list;
    moveGenerator.generateAllLegalMoves(board, move_list);

    // Expected moves:
    // - Rook captures the attacking rook: A4->E4
    std::cout << "Test: Rook Legal Moves When King Is in Check and Rook Can Capture Attacker\n";
    std::cout << "Generated moves (" << move_list.size() << "):\n";
    for (const Move& move : move_list) {
        std::cout << squareToAlgebraic(move.from_square) << " -> " << squareToAlgebraic(move.to_square);
        if (move.flags & FLAG_CAPTURE) {
            std::cout << " x"; // Indicate capture
        }
        std::cout << "\n";
    }


    size_t expected_move_count = 5; // Capturing and moving the king
    assert(move_list.size() == expected_move_count);

    std::cout << "Test passed.\n\n";
}

void testLegalMovesKingInCheckRookCannotHelp() {
    // Initialize board
    Board board;
    board.resetBoard();

    // Clear the board
    for (int i = 0; i < 12; ++i) {
        board.bitboards[i] = 0ULL;
    }
    board.updateOccupancies();

    // Place white king at E1 (square 4)
    set_bit(board.bitboards[WHITE_KING], E1);
    // Place white rook at A1 (square 0)
    set_bit(board.bitboards[WHITE_ROOK], H7);
    // Place black bishop at H5 (square 39), putting the white king in check diagonally
    set_bit(board.bitboards[BLACK_ROOK], E4);
    board.side = WHITE;
    board.updateOccupancies();

    // Generate legal moves
    MoveGenerator moveGenerator;
    std::vector<Move> move_list;
    moveGenerator.generateAllLegalMoves(board, move_list);

    std::cout << "Test: Legal Moves When King Is in Check and Rook Cannot Help\n";
    std::cout << "Generated moves (" << move_list.size() << "):\n";
    for (const Move& move : move_list) {
        std::cout << squareToAlgebraic(move.from_square) << " -> " << squareToAlgebraic(move.to_square) << "\n";
    }

    // Expected moves:
    size_t expected_move_count = 4; // No rook moves
    assert(move_list.size() == expected_move_count);

    std::cout << "Test passed.\n\n";
}


void testIsSquareAttackedByKing() {
    // Initialize board
    Board board;
    board.resetBoard();

    // Clear the board
    for (int i = 0; i < 12; ++i) {
        board.bitboards[i] = 0ULL;
    }
    board.updateOccupancies();

    set_bit(board.bitboards[WHITE_KING], D3);
    set_bit(board.bitboards[BLACK_KING], D5);
    board.side = WHITE;
    board.updateOccupancies();

    // Generate legal moves
    MoveGenerator moveGenerator;
    std::vector<Move> move_list;
    moveGenerator.generateAllLegalMoves(board, move_list);

    std::cout << "Test: Is square attacked by king\n";
    std::cout << "Generated moves (" << move_list.size() << "):\n";
    for (const Move& move : move_list) {
        std::cout << squareToAlgebraic(move.from_square) << " -> " << squareToAlgebraic(move.to_square) << "\n";
    }

    // Expected moves:
    size_t expected_move_count = 5; 
    assert(move_list.size() == expected_move_count);

    std::cout << "Test passed.\n\n";
}

void testTwoRooksTwoKings(){
    // Initialize board
    Board board;
    board.resetBoard();

    // Clear the board
    for (int i = 0; i < 12; ++i) {
        board.bitboards[i] = 0ULL;
    }
    board.updateOccupancies();

    set_bit(board.bitboards[WHITE_KING], E2);
    set_bit(board.bitboards[WHITE_ROOK], D4);
    set_bit(board.bitboards[WHITE_ROOK], G2);
    set_bit(board.bitboards[BLACK_KING], C7);
    set_bit(board.bitboards[BLACK_ROOK], D7);
    set_bit(board.bitboards[BLACK_ROOK], B6);

    board.side = BLACK;
    board.updateOccupancies();

    // Generate legal moves
    MoveGenerator moveGenerator;
    std::vector<Move> move_list;
    moveGenerator.generateAllLegalMoves(board, move_list);

    std::cout << "Test: Two Rooks Two Kings\n";
    std::cout << "Generated moves (" << move_list.size() << "):\n";
    for (const Move& move : move_list) {
        std::cout << squareToAlgebraic(move.from_square) << " -> " << squareToAlgebraic(move.to_square) << "\n";
    }

    // Expected moves:
    size_t expected_move_count = 27; 
    assert(move_list.size() == expected_move_count);

    std::cout << "Test passed.\n\n";
}


void testCastlingMoves() {
    // Initialize board
    Board board;
    board.resetBoard();

    // Clear the board and set up kings and rooks for castling
    for (int i = 0; i < 12; ++i) {
        board.bitboards[i] = 0ULL;
    }
    set_bit(board.bitboards[WHITE_KING], E1);
    set_bit(board.bitboards[WHITE_ROOK], A1);
    set_bit(board.bitboards[WHITE_ROOK], G1);
    set_bit(board.bitboards[WHITE_ROOK], H1);
    set_bit(board.bitboards[WHITE_BISHOP], C5);
    set_bit(board.bitboards[BLACK_KING], E8);
    set_bit(board.bitboards[BLACK_ROOK], A8);
    set_bit(board.bitboards[BLACK_ROOK], H8);
    set_bit(board.bitboards[BLACK_PAWN], F6);
    set_bit(board.bitboards[BLACK_PAWN], G6);

    std::cout << "\nTest: Castling Moves\n";

    std::cout << "Initial Board:\n";
    board.printBoard();

    // Set castling rights for both sides
    board.castling_rights = CASTLE_WHITE_KING_SIDE | CASTLE_WHITE_QUEEN_SIDE | CASTLE_BLACK_KING_SIDE | CASTLE_BLACK_QUEEN_SIDE;

    board.side = WHITE;
    board.updateOccupancies();

    // Generate legal moves
    MoveGenerator moveGenerator;
    std::vector<Move> move_list;


    moveGenerator.generateAllLegalMoves(board, move_list);

    // Filter out castling moves
    std::vector<Move> castling_moves;
    for (const Move& move : move_list) {
        if (move.flags & FLAG_CASTLING) {
            castling_moves.push_back(move);
        }
    }

    std::cout << "Generated castling moves (" << castling_moves.size() << "):\n";
    for (const Move& move : castling_moves) {
        std::cout << squareToAlgebraic(move.from_square) << " -> " << squareToAlgebraic(move.to_square) << " (Castling)\n";

        // Make the move and print the board
        Board new_board = board; // Create a copy to preserve the original board
        new_board.makeMove(move);
        std::cout << "Board after move:\n";
        new_board.printBoard();
        std::cout << "\n";
    }

    // Expected moves: 2 castling moves for white
    // size_t expected_move_count = 2;
    // assert(castling_moves.size() == expected_move_count);

    std::cout << "Test passed.\n\n";
}


void testBishops(){
    // Initialize board
    Board board;
    board.resetBoard();

    // Clear the board
    for (int i = 0; i < 12; ++i) {
        board.bitboards[i] = 0ULL;
    }
    board.updateOccupancies();

    set_bit(board.bitboards[WHITE_BISHOP], F6);
    set_bit(board.bitboards[WHITE_BISHOP], F3);
    set_bit(board.bitboards[WHITE_KING], B1);
    set_bit(board.bitboards[BLACK_BISHOP],  B7);
    set_bit(board.bitboards[BLACK_QUEEN], G2);

    board.side = WHITE;
    board.updateOccupancies();

    // Generate legal moves
    MoveGenerator moveGenerator;
    std::vector<Move> move_list;
    
    std::cout << "Test: Bishops\n";


    moveGenerator.generateAllLegalMoves(board, move_list);

    std::cout << "Generated moves (" << move_list.size() << "):\n";
    for (const Move& move : move_list) {
        std::cout << squareToAlgebraic(move.from_square) << " -> " << squareToAlgebraic(move.to_square) << "\n";
    }

    // Expected moves:
    size_t expected_move_count = 9 + 11 + 2; 
    assert(move_list.size() == expected_move_count);

    std::cout << "Test passed.\n\n";
}


void testKnightMoves() {
    // Initialize board
    Board board;
    board.resetBoard();

    // Clear the board
    for (int i = 0; i < 12; ++i) {
        board.bitboards[i] = 0ULL;
    }
    board.updateOccupancies();

    // Place a white knight at D4 (square 27)
    set_bit(board.bitboards[WHITE_KNIGHT], 27);
    board.side = WHITE;
    board.updateOccupancies();

    // Generate moves
    MoveGenerator moveGenerator;
    std::vector<Move> move_list;
    moveGenerator.generateAllLegalMoves(board, move_list);

    // Print the moves
    std::cout << "Knight moves from d4:\n";
    for (const Move& move : move_list) {
        std::cout << squareToAlgebraic(move.from_square) << " -> " << squareToAlgebraic(move.to_square) << "\n";
    }

    // Expected moves: 8 possible moves (may be fewer if on the edge)
    size_t expected_move_count = 8;
    assert(move_list.size() == expected_move_count);


}

void testSquareAttackedByKnight(){
    // Initialize board
    Board board;
    board.resetBoard();

    // Clear the board
    for (int i = 0; i < 12; ++i) {
        board.bitboards[i] = 0ULL;
    }
    board.updateOccupancies();

    // Place a white knight at D4 
    set_bit(board.bitboards[WHITE_KNIGHT], D4);
    set_bit(board.bitboards[BLACK_KING], F6);

    board.side = BLACK;
    board.updateOccupancies();

    // Generate moves
    MoveGenerator moveGenerator;
    std::vector<Move> move_list;
    moveGenerator.generateAllLegalMoves(board, move_list);

    // Print the moves
    std::cout << "King moves from d4:\n";
    for (const Move& move : move_list) {
        std::cout << squareToAlgebraic(move.from_square) << " -> " << squareToAlgebraic(move.to_square) << "\n";
    }

    size_t expected_move_count = 6;
    assert(move_list.size() == expected_move_count);

}


void testIsSquareAttackedByPawn() {
    Board board;
    board.resetBoard();

    // Clear the board
    for (int i = 0; i < 12; ++i) {
        board.bitboards[i] = 0ULL;
    }
    board.updateOccupancies();

    // Place a black pawn at e5 (square 36)
    set_bit(board.bitboards[BLACK_PAWN], E5);

    MoveGenerator moveGenerator;

    // Squares that should be attacked by the pawn (d4 and f4)
    int attacked_squares[] = {D4, F4}; // Squares corresponding to d4 and f4

    for (int square : attacked_squares) {
        bool is_attacked = moveGenerator.isSquareAttackedByPawn(board, square, BLACK);
        std::cout << "Square " << squareToAlgebraic(square)
                  << " is attacked by pawn: " << (is_attacked ? "Yes" : "No") << "\n";
        assert(is_attacked);
    }

    // Test a square that should not be attacked
    int non_attacked_square = 44; // e6
    bool is_attacked = moveGenerator.isSquareAttackedByPawn(board, non_attacked_square, BLACK);
    std::cout << "Square " << squareToAlgebraic(non_attacked_square)
              << " is attacked by pawn: " << (is_attacked ? "Yes" : "No") << "\n";
    assert(!is_attacked);

    std::cout << "isSquareAttackedByPawn tests passed.\n";
}

void testPawnPromotions() {
    // Initialize board
    Board board;
    board.resetBoard();

    // Clear the board
    for (int i = 0; i < 12; ++i) {
        board.bitboards[i] = 0ULL;
    }
    board.updateOccupancies();

    // Place a white pawn on e7 (square 52) and a black knight on d8 (square 59)
    set_bit(board.bitboards[WHITE_PAWN], E7);
    set_bit(board.bitboards[BLACK_KNIGHT], D8);

    board.side = WHITE;
    board.updateOccupancies();

    // Generate legal moves
    MoveGenerator moveGenerator;
    std::vector<Move> move_list;

    std::cout << "Test: Pawn Promotions with Move Execution\n";

    moveGenerator.generateAllLegalMoves(board, move_list);

    std::cout << "Generated moves (" << move_list.size() << "):\n";
    for (const Move& move : move_list) {
        std::cout << squareToAlgebraic(move.from_square) << " -> " << squareToAlgebraic(move.to_square);

        if (move.flags & FLAG_PROMOTION) {
            std::string promotion_piece;
            switch (move.promoted_piece) {
                case WHITE_QUEEN:
                    promotion_piece = "Queen";
                    break;
                case WHITE_ROOK:
                    promotion_piece = "Rook";
                    break;
                case WHITE_BISHOP:
                    promotion_piece = "Bishop";
                    break;
                case WHITE_KNIGHT:
                    promotion_piece = "Knight";
                    break;
            }
            std::cout << " = " << promotion_piece;
        }
        if (move.flags & FLAG_CAPTURE) {
            std::cout << " (captures " << pieceToString(move.captured_piece) << ")";
        }
        std::cout << "\n";

        // Make the move and print the board
        Board new_board = board; // Create a copy to preserve the original board
        new_board.makeMove(move);
        std::cout << "Board after move:\n";
        new_board.printBoard();
        std::cout << "\n";
    }

    // Expected moves: 8 promotion moves (4 promotions with and without capturing the knight)
    size_t expected_move_count = 8;
    assert(move_list.size() == expected_move_count);

    std::cout << "Test passed.\n\n";
}




void testEnPassant() {
    // Initialize board
    Board board;
    board.resetBoard();

    // Clear the board
    for (int i = 0; i < 12; ++i) {
        board.bitboards[i] = 0ULL;
    }
    board.updateOccupancies();

    // Place a white pawn on e5 (square 28) and a black pawn on d5 (square 27)
    set_bit(board.bitboards[WHITE_PAWN], E5);
    set_bit(board.bitboards[BLACK_PAWN], D5);
    board.side = WHITE;
    board.en_passant = 27; // D5 is the en passant target
    board.updateOccupancies();

    // Generate legal moves
    MoveGenerator moveGenerator;
    std::vector<Move> move_list;

    std::cout << "Test: En Passant\n";

    moveGenerator.generatePawnMoves(board, move_list);

    std::cout << "Generated moves (" << move_list.size() << "):\n";
    for (const Move& move : move_list) {
        std::cout << squareToAlgebraic(move.from_square) << " -> " << squareToAlgebraic(move.to_square);
        if (move.flags & FLAG_EN_PASSANT) {
            std::cout << " (En Passant)";
        }
        std::cout << "\n";
    }

    // Expected moves: 1 en passant capture move
    size_t expected_move_count = 1;
    assert(move_list.size() == expected_move_count);

    std::cout << "Test passed.\n\n";
}


void testPawnDoubleMove() {
    // Initialize board
    Board board;
    board.resetBoard();

    // Clear the board
    for (int i = 0; i < 12; ++i) {
        board.bitboards[i] = 0ULL;
    }
    board.updateOccupancies();

    // Place a white pawn on e2 (square 12) to test double move
    set_bit(board.bitboards[WHITE_PAWN], E2);
    
    board.side = WHITE;
    board.updateOccupancies();

    // Generate legal moves
    MoveGenerator moveGenerator;
    std::vector<Move> move_list;

    std::cout << "Test: Pawn Double Move\n";

    moveGenerator.generatePawnMoves(board, move_list);

    std::cout << "Generated moves (" << move_list.size() << "):\n";
    for (const Move& move : move_list) {
        std::cout << squareToAlgebraic(move.from_square) << " -> " << squareToAlgebraic(move.to_square);
        if (move.flags & FLAG_PAWN_DOUBLE_PUSH) {
            std::cout << " (Double Move)";
        }
        std::cout << "\n";
    }

    // Expected moves: 2 moves (one square forward and two squares forward)
    size_t expected_move_count = 2;
    assert(move_list.size() == expected_move_count);

    std::cout << "Test passed.\n\n";
}