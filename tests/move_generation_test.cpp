#include "board.h"
#include "move_generator.h"
#include "move.h"
#include <vector>
#include <iostream>
#include <cassert>

Move fromUCI(const std::string& moveStr, const Board& board) {
    if (moveStr.length() < 4) return Move(-1, -1, NO_PIECE, NO_PIECE, NO_PIECE, 0); // Invalid move

    int fromSquare = algebraicToSquare(moveStr.substr(0, 2));
    int toSquare = algebraicToSquare(moveStr.substr(2, 2));

    // Handle promotion if applicable (e.g., "e7e8q")
    int promotedPiece = NO_PIECE;
    if (moveStr.length() == 5) {
        char promo = moveStr[4];
        promotedPiece = (board.side == WHITE)
                            ? (promo == 'q' ? WHITE_QUEEN : promo == 'r' ? WHITE_ROOK
                                           : promo == 'b' ? WHITE_BISHOP
                                                          : WHITE_KNIGHT)
                            : (promo == 'q' ? BLACK_QUEEN : promo == 'r' ? BLACK_ROOK
                                           : promo == 'b' ? BLACK_BISHOP
                                                          : BLACK_KNIGHT);
    }

    // Determine the moving piece
    int movingPiece = NO_PIECE;
    for (int piece = WHITE_PAWN; piece <= BLACK_KING; ++piece) {
        if (get_bit(board.bitboards[piece], fromSquare)) {
            movingPiece = piece;
            break;
        }
    }

    // Determine the captured piece, if any
    int capturedPiece = NO_PIECE;
    for (int piece = WHITE_PAWN; piece <= BLACK_KING; ++piece) {
        if (get_bit(board.bitboards[piece], toSquare)) {
            capturedPiece = piece;
            break;
        }
    }

    // Determine flags
    uint8_t flags = 0;
    if (capturedPiece != NO_PIECE) flags |= FLAG_CAPTURE;
    if (promotedPiece != NO_PIECE) flags |= FLAG_PROMOTION;

    return Move(fromSquare, toSquare, movingPiece, capturedPiece, promotedPiece, flags);
}

std::string toUCI(const Move& move) {
    std::string uci = squareToAlgebraic(move.from_square) + squareToAlgebraic(move.to_square);
    if (move.promoted_piece != NO_PIECE) {
        char promo = (move.promoted_piece == WHITE_QUEEN || move.promoted_piece == BLACK_QUEEN)
                         ? 'q'
                         : (move.promoted_piece == WHITE_ROOK || move.promoted_piece == BLACK_ROOK)
                               ? 'r'
                               : (move.promoted_piece == WHITE_BISHOP || move.promoted_piece == BLACK_BISHOP)
                                     ? 'b'
                                     : 'n';
        uci += promo;
    }
    return uci;
}

void testKingMoveIntoCheck() {
    // Create a board and load the FEN
    Board board;
    board.loadFEN("4r1k1/4r1p1/6K1/p2R1P2/5PqP/1QP5/1P6/3R4 w - - 3 3");
    //board.printBoard();

    // Generate all legal moves for White
    MoveGenerator moveGenerator;
    std::vector<Move> move_list;
    moveGenerator.generateAllLegalMoves(board, move_list);

    // Print all generated moves
    std::cout << "Generated moves for White:\n";
    for (const Move& move : move_list) {
        std::cout << toUCI(move) << "\n";
    }
}


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


void testPawnCapture();
void testKnightCapture();
void testMultipleCaptures();
void testPromotionCapture();
void testEnPassantCapture();
void testNoCaptureMoves();
void testGenerateCaptureMoves(){
    testPawnCapture();
    testKnightCapture();
    testMultipleCaptures();
    testPromotionCapture();
    testEnPassantCapture();
    testNoCaptureMoves();
}

void testThreefoldRepetition();
void testFiftyMoveRule();
void testCombinationRules();
void testNoRepetitionNoFiftyMove();

void run3fold50moveTests(){
    testThreefoldRepetition();
    testFiftyMoveRule();
    testCombinationRules();
    testNoRepetitionNoFiftyMove();
}


int main() {
    

    // runRookTests();
    // runKingTests();
    // testBishops();
    // testKnightMoves();
    // testSquareAttackedByKnight();
    // runPawnTests();
    // testCastlingMoves();
    // testGenerateCaptureMoves();
    // run3fold50moveTests();
    testKingMoveIntoCheck();
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


// Test 1: Pawn Capture
void testPawnCapture() {
    Board board;
    board.resetBoard();
    
    // Clear the board
    for (int i = 0; i < 12; ++i) {
        board.bitboards[i] = 0ULL;
    }
    board.updateOccupancies();
    
    // Place a white pawn on e4 and a black pawn on d5
    set_bit(board.bitboards[WHITE_PAWN], algebraicToSquare("e4"));
    set_bit(board.bitboards[BLACK_PAWN], algebraicToSquare("d5"));
    board.side = WHITE;
    board.updateOccupancies();
    
    MoveGenerator moveGenerator;
    std::vector<Move> capture_moves;
    moveGenerator.generateAllCaptureMoves(board, capture_moves);
    
    std::cout << "Test 1: Pawn Capture\n";
    board.printBoard();
    
    // Expected: 1 capture move (e4xd5)
    assert(capture_moves.size() == 1);
    Move expectedMove = fromUCI("e4d5", board);
    assert(capture_moves[0] == expectedMove);
    
    std::cout << "Generated capture moves (" << capture_moves.size() << "):\n";
    for (const Move& move : capture_moves) {
        std::cout << toUCI(move) << "\n";
    }
    std::cout << "Test 1 passed.\n\n";
}

// Test 2: Knight Capture
void testKnightCapture() {
    Board board;
    board.resetBoard();
    
    // Clear the board
    for (int i = 0; i < 12; ++i) {
        board.bitboards[i] = 0ULL;
    }
    board.updateOccupancies();
    
    // Place a white knight on f3 and a black bishop on e5
    set_bit(board.bitboards[WHITE_KNIGHT], algebraicToSquare("f3"));
    set_bit(board.bitboards[BLACK_BISHOP], algebraicToSquare("e5"));
    board.side = WHITE;
    board.updateOccupancies();
    
    MoveGenerator moveGenerator;
    std::vector<Move> capture_moves;
    moveGenerator.generateAllCaptureMoves(board, capture_moves);
    
    std::cout << "Test 2: Knight Capture\n";
    board.printBoard();
    
    // Expected: 1 capture move (Nf3xe5)
    assert(capture_moves.size() == 1);
    Move expectedMove = fromUCI("f3e5", board);
    assert(capture_moves[0] == expectedMove);
    
    std::cout << "Generated capture moves (" << capture_moves.size() << "):\n";
    for (const Move& move : capture_moves) {
        std::cout << toUCI(move) << "\n";
    }
    std::cout << "Test 2 passed.\n\n";
}

// Test 3: Multiple Capture Moves
void testMultipleCaptures() {
    Board board;
    board.resetBoard();
    
    // Clear the board
    for (int i = 0; i < 12; ++i) {
        board.bitboards[i] = 0ULL;
    }
    board.updateOccupancies();
    
    // Place a white rook on a1 and black pawns on a3 and a4
    set_bit(board.bitboards[WHITE_ROOK], algebraicToSquare("a1"));
    set_bit(board.bitboards[BLACK_PAWN], algebraicToSquare("a3"));
    set_bit(board.bitboards[BLACK_PAWN], algebraicToSquare("b1"));
    board.side = WHITE;
    board.updateOccupancies();
    
    MoveGenerator moveGenerator;
    std::vector<Move> capture_moves;
    moveGenerator.generateAllCaptureMoves(board, capture_moves);
    
    std::cout << "Test 3: Multiple Captures\n";
    board.printBoard();
    
    assert(capture_moves.size() == 2);
    Move expectedMove1 = fromUCI("a1a3", board);
    Move expectedMove2 = fromUCI("a1b1", board);
    assert((capture_moves[0] == expectedMove1 || capture_moves[0] == expectedMove2));
    assert((capture_moves[1] == expectedMove1 || capture_moves[1] == expectedMove2));
    
    std::cout << "Generated capture moves (" << capture_moves.size() << "):\n";
    for (const Move& move : capture_moves) {
        std::cout << toUCI(move) << "\n";
    }
    std::cout << "Test 3 passed.\n\n";
}

// Test 4: Promotion with Capture
void testPromotionCapture() {
    Board board;
    board.resetBoard();
    
    // Clear the board
    for (int i = 0; i < 12; ++i) {
        board.bitboards[i] = 0ULL;
    }
    board.updateOccupancies();
    
    // Place a white pawn on h7 and a black rook on h8
    set_bit(board.bitboards[WHITE_PAWN], algebraicToSquare("h7"));
    set_bit(board.bitboards[BLACK_ROOK], algebraicToSquare("g8"));
    board.side = WHITE;
    board.updateOccupancies();
    
    MoveGenerator moveGenerator;
    std::vector<Move> capture_moves;
    moveGenerator.generateAllCaptureMoves(board, capture_moves);
    
    std::cout << "Test 4: Promotion with Capture\n";
    board.printBoard();
    
    std::cout << "Generated capture moves (" << capture_moves.size() << "):\n";
    for (const Move& move : capture_moves) {
        std::cout << toUCI(move) << "\n";
    }

    // Expected: 4 capture moves (h7xh8=Q, h7xh8=R, etc)
    assert(capture_moves.size() == 4);
    assert(capture_moves[0].from_square == algebraicToSquare("h7"));
    assert(capture_moves[0].to_square == algebraicToSquare("g8"));
    assert(capture_moves[0].captured_piece == BLACK_ROOK);
    assert(capture_moves[0].promoted_piece == WHITE_QUEEN); // Assuming WHITE_QUEEN is the enum for queen
    

    std::cout << "Test 4 passed.\n\n";
}

// Test 5: En Passant Capture
void testEnPassantCapture() {
    Board board;
    board.resetBoard();
    
    // Clear the board
    for (int i = 0; i < 12; ++i) {
        board.bitboards[i] = 0ULL;
    }
    board.updateOccupancies();
    
    // Place a white pawn on e5 and a black pawn on d5
    set_bit(board.bitboards[WHITE_PAWN], algebraicToSquare("e5"));
    set_bit(board.bitboards[BLACK_PAWN], algebraicToSquare("d5"));
    board.side = WHITE;
    board.en_passant = algebraicToSquare("d6"); // Black just moved pawn d7-d5
    board.updateOccupancies();
    
    MoveGenerator moveGenerator;
    std::vector<Move> capture_moves;
    moveGenerator.generateAllCaptureMoves(board, capture_moves);
    
    std::cout << "Test 5: En Passant Capture\n";
    board.printBoard();
    
    // Expected: 1 en passant capture move (e5xd6)
    assert(capture_moves.size() == 1);
    assert(capture_moves[0].from_square == algebraicToSquare("e5"));
    assert(capture_moves[0].to_square == algebraicToSquare("d6"));
    assert(capture_moves[0].captured_piece == BLACK_PAWN); // Assuming en passant capture is recorded as capturing a pawn
    assert(capture_moves[0].flags & 2); // Assuming flag 2 is for promotion, adjust accordingly if you have specific flags for en passant
    
    std::cout << "Generated capture moves (" << capture_moves.size() << "):\n";
    for (const Move& move : capture_moves) {
        std::cout << toUCI(move) << "\n";
    }
    std::cout << "Test 5 passed.\n\n";
}

// Test 6: No Capture Moves
void testNoCaptureMoves() {
    Board board;
    board.resetBoard();
    
    // Clear the board
    for (int i = 0; i < 12; ++i) {
        board.bitboards[i] = 0ULL;
    }
    board.updateOccupancies();
    
    // Place a white rook on a1 and a white pawn on a2
    set_bit(board.bitboards[WHITE_ROOK], algebraicToSquare("a1"));
    set_bit(board.bitboards[WHITE_PAWN], algebraicToSquare("a2"));
    board.side = WHITE;
    board.updateOccupancies();
    
    MoveGenerator moveGenerator;
    std::vector<Move> capture_moves;
    moveGenerator.generateAllCaptureMoves(board, capture_moves);
    
    std::cout << "Test 6: No Capture Moves\n";
    board.printBoard();
    
    // Expected: 0 capture moves
    assert(capture_moves.empty());
    
    std::cout << "Generated capture moves (" << capture_moves.size() << "): None\n";
    std::cout << "Test 6 passed.\n\n";
}



// Helper function to set a piece on the board
void setPiece(Board& board, const std::string& square, int piece) {
    int sq = algebraicToSquare(square);
    set_bit(board.bitboards[piece], sq);
    board.updateOccupancies();
    board.computeHash(); // Recompute hash after setting the piece
}

// Function to apply a move
void applyMove(Board& board, const std::string& moveStr) {
    Move move = fromUCI(moveStr, board);
    board.makeMove(move);
}

// Test 1: Threefold Repetition
void testThreefoldRepetition() {
    Board board;
    board.resetBoard();
    board.computeHash();
    board.updateRepetitionHistory(Move());
    board.repetition_counts[board.hash_key] = 1;

    // Example: Repeating a simple position
    // White King on e1, White Rook on h1
    // Black King on e8, Black Rook on a8
    // Alternate moving White Rook between h1 and h2 without changing the board
    setPiece(board, "e1", WHITE_KING);
    setPiece(board, "h1", WHITE_ROOK);
    setPiece(board, "e8", BLACK_KING);
    setPiece(board, "a8", BLACK_ROOK);
    board.side = WHITE;
    board.computeHash();
    board.updateRepetitionHistory(Move());
    board.repetition_counts[board.hash_key] = 2; // Initial position occurred twice

    // Move 1: White Rook h1-h2
    applyMove(board, "h1h2");

    // Move 2: Black Rook a8-a7 (not affecting repetition)
    applyMove(board, "a8a7");

    // Move 3: White Rook h2-h1
    applyMove(board, "h2h1");

    // Move 4: Black Rook a7-a8
    applyMove(board, "a7a8");

    // Move 5: White Rook h1-h2
    applyMove(board, "h1h2");

    // Move 6: Black Rook a8-a7
    applyMove(board, "a8a7");

    // Move 7: White Rook h2-h1
    applyMove(board, "h2h1");

    // Move 8: Black Rook a7-a8
    applyMove(board, "a7a8");

    // Move 9: White Rook h1-h2
    applyMove(board, "h1h2");

    // After these moves, the position should have occurred three times
    assert(board.isThreefoldRepetition());

    std::cout << "Test 1: Threefold Repetition Passed.\n\n";
}

// Test 2: 50-Move Rule
void testFiftyMoveRule() {
    Board board;
    board.resetBoard();
    board.computeHash();
    board.updateRepetitionHistory(Move());
    board.repetition_counts[board.hash_key] = 1;

    // Example: Making 100 non-capturing, non-pawn moves
    // For simplicity, move a knight back and forth without captures or pawn moves
    // White Knight on b1 and c3, Black Knight on b8 and c6
    setPiece(board, "b1", WHITE_KNIGHT);
    setPiece(board, "b8", BLACK_KNIGHT);
    board.side = WHITE;
    board.computeHash();
    board.updateRepetitionHistory(Move());
    board.repetition_counts[board.hash_key] = 2;
    // Define a sequence of moves that do not involve pawn moves or captures
    std::vector<std::string> moves = {
        "b1c3", "b8c6",
        "c3b1", "c6b8",
        "b1c3", "b8c6",
        "c3b1", "c6b8",
        // Repeat the above 12 times to reach 24 half-moves
    };


    // Repeat the moves 24 times (48 half-moves)
    for (int i = 0; i < 12; ++i) {
        for (const std::string& moveStr : moves) {
            applyMove(board, moveStr);
        }
    }

    assert(board.halfmove_clock == 96);

    // Make 4 more half-moves without captures or pawn moves to reach 100
    applyMove(board, "b1c3");
    applyMove(board, "b8c6");
    applyMove(board, "c3b1");
    applyMove(board, "c6b8");

    // Now, halfmove_clock should be 100
    assert(board.isFiftyMoveRule());

    std::cout << "Test 2: 50-Move Rule Passed.\n\n";
}

// Test 3: Combination of Threefold Repetition and 50-Move Rule
void testCombinationRules() {
    Board board;
    board.resetBoard();
    board.computeHash();
    board.updateRepetitionHistory(Move());
    board.repetition_counts[board.hash_key] = 1;

    // Set up a simple position
    setPiece(board, "e1", WHITE_KING);
    setPiece(board, "h1", WHITE_ROOK);
    setPiece(board, "e8", BLACK_KING);
    setPiece(board, "a8", BLACK_ROOK);
    board.side = WHITE;
    board.computeHash();
    board.updateRepetitionHistory(Move());
    board.repetition_counts[board.hash_key] = 2;

    // Define a sequence of non-pawn, non-capturing moves that repeat the position
    std::vector<std::string> moves = {
        "h1h2", "a8a7",
        "h2h1", "a7a8",
        // Repeating enough to trigger both rules
    };

    // Apply the moves
    for(int i = 0; i < 25; i++){
        for (const std::string& moveStr : moves) {
            applyMove(board, moveStr);
        }
    }

    // At this point, check for both rules
    if (board.isThreefoldRepetition()) {
        std::cout << "Draw by threefold repetition!\n";
    }

    if (board.isFiftyMoveRule()) {
        std::cout << "Draw by the 50-move rule!\n";
    }

    // Both should be true
    assert(board.isThreefoldRepetition());
    assert(board.isFiftyMoveRule());

    std::cout << "Test 3: Combination of Threefold Repetition and no 50-Move Rule Passed.\n\n";
}

// Test 4: No Repetition and No 50-Move Rule
void testNoRepetitionNoFiftyMove() {
    Board board;
    board.resetBoard();
    // Initialize the board state
    setPiece(board, "e1", WHITE_KING);
    setPiece(board, "e8", BLACK_KING);
    board.side = WHITE;

    // Compute the initial hash and add it to the repetition history
    board.computeHash();
    board.updateRepetitionHistory(Move());
    std::vector<std::string> moves = {
        "e1e2", "e8e7",
        "e2e1", "e7e8",
        // Repeating enough to trigger both rules
    };
    for (const std::string& moveStr : moves) {
        applyMove(board, moveStr);
    }


    // Check that no draw conditions are met
    assert(!board.isThreefoldRepetition());
    assert(!board.isFiftyMoveRule());

    std::cout << "Test 4: No Repetition and No 50-Move Rule Passed.\n\n";
}