#ifndef EVALUATION_H
#define EVALUATION_H

#include "board.h"
#include "move.h"
#include "move_generator.h"
#include <limits.h>

class Evaluation {
public:
    static int evaluatePosition(const Board& board);
    static int getPieceValue(const int piece);
    static bool isIsolatedPawn(const Board& board, int square, int side);
    static bool isConnectedPawn(const Board& board, int square, int side);
    static bool isDoubledPawn(const Board& board, int square, int side);
    static bool isPassedPawn(const Board& board, int square, int side);
    static bool isCentralSquare(int square);
    
    // Piece values
    static constexpr int KING_VALUE = 2000000;
    static constexpr int QUEEN_VALUE = 900;
    static constexpr int ROOK_VALUE = 500;
    static constexpr int BISHOP_VALUE = 300;
    static constexpr int KNIGHT_VALUE = 300;
    static constexpr int PAWN_VALUE = 100;

    // Additional evaluation factors
    static constexpr double DOUBLED_PAWN_PENALTY = -0.5;
    static constexpr double ISOLATED_PAWN_PENALTY = -0.5;
    static constexpr double BLOCKED_PAWN_PENALTY = -0.5;
    static constexpr double MOBILITY_WEIGHT = 0.1;
    
    static int scorePawnStructure(const Board& board, int side);
    static int materialScore(const Board& board);
    static int mobilityScore(const Board& board);

private:
    // Piece-square tables for each piece type
    static const int pawnTable[64];
    static const int knightTable[64];
    static const int bishopTable[64];
    static const int rookTable[64];
    static const int queenTable[64];
    static const int kingMiddleGameTable[64];
    static const int kingEndGameTable[64];

    // Helper methods
    static int pieceSquareScore(const Board& board);
    static bool isEndGame(const Board& board);
    static int mirrorSquare(int square);
};



#endif
