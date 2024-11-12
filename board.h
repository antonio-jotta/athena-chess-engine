#ifndef BOARD_H
#define BOARD_H
#include <string>
#include <iostream>
typedef unsigned long long U64;

// Enum for pieces

enum Piece {
    WHITE_PAWN,   // 0
    WHITE_KNIGHT, // 1
    WHITE_BISHOP, // 2
    WHITE_ROOK,   // 3
    WHITE_QUUEN,  // 4
    WHITE_KING,   // 5
    BLACK_PAWN,   // 6
    BLACK_KNIGHT, // 7
    BLACK_BISHOP, // 8
    BLACK_ROOK,   // 9
    BLACK_QUUEN,  // 10
    BLACK_KING    // 11
};

enum Square {
    A1, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8
};


// Enum for sides
enum Side {
    WHITE,
    BLACK,
    BOTH
};

// Castling rights using bit flags
enum CastlingRights {
    CASTLE_WHITE_KING_SIDE  = 1, // 0001
    CASTLE_WHITE_QUEEN_SIDE = 2, // 0010
    CASTLE_BLACK_KING_SIDE  = 4, // 0100
    CASTLE_BLACK_QUEEN_SIDE = 8  // 1000
};

class Board{
    public:
        // Bitboards for each piece
        U64 bitboards[12];

        // Occupancy bitboards
        U64 occupancies[3];

        // Side to move
        Side side;

        // En passant square (-1 if not available)
        int en_passant;

        // Castling rights represented as a bitmask
        int castling_rights;
        
        Board();

        void resetBoard();
        void printBoard();
        void updateOccupancies();
};

// Bit manipulation functions
inline void set_bit(U64& bitboard, int square) {
    bitboard |= (1ULL << square);
}

inline bool get_bit(U64 bitboard, int square) {
    return bitboard & (1ULL << square);
}

inline void clear_bit(U64& bitboard, int square) {
    bitboard &= ~(1ULL << square);
}
// Additional utility functions
int bitscanForward(U64 bb);
int countBits(U64 bb);

#endif