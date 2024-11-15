#ifndef MOVE_H
#define MOVE_H

#include <cstdint>

// Move flags
enum MoveFlags {
    FLAG_NONE               = 0,      // normal move without any special characteristics
    FLAG_CAPTURE            = 1 << 0, // the move is a capture
    FLAG_EN_PASSANT         = 1 << 1, // en passant captures
    FLAG_CASTLING           = 1 << 2, // castling moves
    FLAG_PROMOTION          = 1 << 3, // the move is a pawn promotion
    FLAG_PAWN_DOUBLE_PUSH   = 1 << 4  // pawn's initial two-square advance
};

// Enum for pieces
enum Piece {
    WHITE_PAWN,   // 0
    WHITE_KNIGHT, // 1
    WHITE_BISHOP, // 2
    WHITE_ROOK,   // 3
    WHITE_QUEEN,  // 4
    WHITE_KING,   // 5
    BLACK_PAWN,   // 6
    BLACK_KNIGHT, // 7
    BLACK_BISHOP, // 8
    BLACK_ROOK,   // 9
    BLACK_QUEEN,  // 10
    BLACK_KING,   // 11
    NO_PIECE      // Represents an empty square or the absence of a piece
};

struct Move{
    int from_square; // 0-63
    int to_square; // 0-63
    int piece; //Moving piece
    int captured_piece; 
    int promoted_piece;
    uint8_t flags; // Move flags
    
    // Constructor
    Move(int from, int to, int p, int captured, int promoted, uint8_t f)
        : from_square(from), to_square(to), piece(p),
          captured_piece(captured), promoted_piece(promoted), flags(f) {}

    // Default Constructor
    Move()
        : from_square(-1), to_square(-1), piece(NO_PIECE),
          captured_piece(NO_PIECE), promoted_piece(NO_PIECE), flags(FLAG_NONE) {}
    // Equality Operator
    bool operator==(const Move& other) const {
        return from_square == other.from_square &&
               to_square == other.to_square &&
               piece == other.piece &&
               captured_piece == other.captured_piece &&
               promoted_piece == other.promoted_piece &&
               flags == other.flags;
    }

};  

#endif