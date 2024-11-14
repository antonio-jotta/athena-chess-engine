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

};

#endif