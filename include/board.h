#ifndef BOARD_H
#define BOARD_H

#include <string>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <random>
#include "move.h"

typedef unsigned long long U64;
const int NO_SQUARE = -1;  // Represents "no en passant square"

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

constexpr U64 FILE_A = 0x0101010101010101ULL;
constexpr U64 FILE_B = 0x0202020202020202ULL;
constexpr U64 FILE_C = 0x0404040404040404ULL;
constexpr U64 FILE_D = 0x0808080808080808ULL;
constexpr U64 FILE_E = 0x1010101010101010ULL;
constexpr U64 FILE_F = 0x2020202020202020ULL;
constexpr U64 FILE_G = 0x4040404040404040ULL;
constexpr U64 FILE_H = 0x8080808080808080ULL;
constexpr U64 FILE_MASKS[8] = {FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H};

class Board {
public:
    // Bitboards for each piece
    U64 bitboards[12];

    // Occupancy bitboards
    U64 occupancies[3];

    // Side to move
    Side side;

    // En passant square (-1 if not available)
    int en_passant; // Square index or NO_SQUARE

    // Castling rights flags
    int castling_rights;

    // Halfmove clock for 50-move rule
    int halfmove_clock;

    // Move number
    int move_number;
    
    // Draw Flag
    int DRAW = -1;

    // Zobrist hash of the current position
    U64 hash_key;

    // History of positions and their occurrence counts
    std::unordered_map<U64, int> repetition_counts;

    // List of non-pawn move positions (only non-pawn moves are tracked)
    std::vector<U64> non_pawn_move_history;

    // Zobrist hashing keys
    static U64 piece_keys[12][64];    // Random keys for piece positions
    static U64 side_key;              // Random key for side to move
    static U64 enpassant_keys[64];    // Random keys for en passant squares
    static U64 castling_keys[16];     // Random keys for castling rights

    // Constructor
    Board();

    // Board setup methods
    void resetBoard();
    void setInitialPosition();
    void loadFEN(const std::string& fen);
    std::string generateFEN() const;

    // Board state methods
    void printBoard();
    void updateOccupancies();
    void updateCastlingRights(const Move& move);

    // Move handling
    void makeMove(const Move& move, bool switch_side = true);

    // Zobrist hashing methods
    void initZobristKeys();
    void computeHash();
    void updateHash(const Move& move);

    // Repetition and 50-move rule methods
    void updateRepetitionHistory(const Move& move);
    bool isThreefoldRepetition() const;
    bool isFiftyMoveRule() const;

    // Utility methods
    std::string getCastlingRightsString() const;

private:
    // Helper method to check if a piece is a pawn
    bool isPawnMove(int piece) const;
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

int bitscanForward(U64 bb);
int countBits(U64 bb);

// Additional utility functions for strings
std::string squareToAlgebraic(int square);
char pieceToChar(Piece piece);
std::string pieceToString(int piece);
int algebraicToSquare(const std::string& algebraic);

#endif 
