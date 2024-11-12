#include "board.h"

Board::Board() {
    resetBoard();
}

void Board::resetBoard() {
    // Clear all bitboards
    for (int i = 0; i < 12; i++)
        bitboards[i] = 0ULL;

    // Set up the starting position
    bitboards[WHITE_PAWN]   = 0x000000000000FF00ULL; // Pawns on rank 2
    bitboards[WHITE_KNIGHT] = 0x0000000000000042ULL; // Knights on b1 and g1
    bitboards[WHITE_BISHOP] = 0x0000000000000024ULL; // Bishops on c1 and f1
    bitboards[WHITE_ROOK]   = 0x0000000000000081ULL; // Rooks on a1 and h1
    bitboards[WHITE_QUUEN]  = 0x0000000000000010ULL; // Queen on d1
    bitboards[WHITE_KING]   = 0x0000000000000008ULL; // King on e1

    bitboards[BLACK_PAWN]   = 0x00FF000000000000ULL; // Pawns on rank 7
    bitboards[BLACK_KNIGHT] = 0x4200000000000000ULL; // Knights on b8 and g8
    bitboards[BLACK_BISHOP] = 0x2400000000000000ULL; // Bishops on c8 and f8
    bitboards[BLACK_ROOK]   = 0x8100000000000000ULL; // Rooks on a8 and h8
    bitboards[BLACK_QUUEN]  = 0x1000000000000000ULL; // Queen on d8
    bitboards[BLACK_KING]   = 0x0800000000000000ULL; // King on e8

    // Update occupancies
    updateOccupancies();

    // Set side to move
    side = WHITE;

    // No en passant square initially
    en_passant = -1;

    // All castling rights available
    castling_rights = CASTLE_WHITE_KING_SIDE | CASTLE_WHITE_QUEEN_SIDE |
                      CASTLE_BLACK_KING_SIDE | CASTLE_BLACK_QUEEN_SIDE;
}


void Board::updateOccupancies() {
    occupancies[WHITE] = 0ULL;
    occupancies[BLACK] = 0ULL;

    // Sum up white pieces
    for (int piece = WHITE_PAWN; piece <= WHITE_KING; piece++)
        occupancies[WHITE] |= bitboards[piece];

    // Sum up black pieces
    for (int piece = BLACK_PAWN; piece <= BLACK_KING; piece++)
        occupancies[BLACK] |= bitboards[piece];

    // All pieces
    occupancies[BOTH] = occupancies[WHITE] | occupancies[BLACK];
}


void Board::printBoard() {
    for (int rank = 7; rank >= 0; rank--) {
        std::cout << rank + 1 << "  ";
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;
            char pieceChar = '.';

            for (int piece = WHITE_PAWN; piece <= BLACK_KING; piece++) {
                if (get_bit(bitboards[piece], square)) {
                    switch (piece) {
                        case WHITE_PAWN:   pieceChar = 'P'; break;
                        case WHITE_KNIGHT: pieceChar = 'N'; break;
                        case WHITE_BISHOP: pieceChar = 'B'; break;
                        case WHITE_ROOK:   pieceChar = 'R'; break;
                        case WHITE_QUUEN:  pieceChar = 'Q'; break;
                        case WHITE_KING:   pieceChar = 'K'; break;
                        case BLACK_PAWN:   pieceChar = 'p'; break;
                        case BLACK_KNIGHT: pieceChar = 'n'; break;
                        case BLACK_BISHOP: pieceChar = 'b'; break;
                        case BLACK_ROOK:   pieceChar = 'r'; break;
                        case BLACK_QUUEN:  pieceChar = 'q'; break;
                        case BLACK_KING:   pieceChar = 'k'; break;
                    }
                    break;
                }
            }
            std::cout << pieceChar << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "\n   A B C D E F G H\n";
    std::cout << "Side to move: " << (side == WHITE ? "White\n" : "Black\n");
    std::cout << "En Passant: " << (en_passant != -1 ? std::to_string(en_passant) : "None") << "\n";
    std::cout << "Castling: " << castling_rights << "\n";
}

std::string squareToAlgebraic(int square) {
    // Ensure square is within bounds
    if (square < 0 || square > 63) {
        return "Invalid";  // Return an error string if out of bounds
    }
    
    char file = 'a' + (square % 8);       // File from 'a' to 'h'
    char rank = '1' + (square / 8);       // Rank from '1' to '8'
    
    return std::string(1, file) + std::string(1, rank);
}

// Find the index of the least significant 1 bit in b (returns 0-based index)
// Used for iteration in the bitboard
int bitscanForward(U64 b) {
    // Use GCC/Clang built-in intrinsic if available
    #if defined(__GNUC__) || defined(__clang__)
        return __builtin_ctzll(b);
    #else
        // Fallback method if no intrinsics are available
        int index = 0;
        while ((b & 1) == 0) {
            b >>= 1;
            index++;
        }
        return index;
    #endif
}

// Count the number of 1 bits in b (population count)
int countBits(U64 b) {
    // Use GCC/Clang built-in intrinsic if available
    #if defined(__GNUC__) || defined(__clang__)
        return __builtin_popcountll(b);
    #else
        // Fallback method if no intrinsics are available
        int count = 0;
        while (b) {
            count += b & 1;
            b >>= 1;
        }
        return count;
    #endif
}

