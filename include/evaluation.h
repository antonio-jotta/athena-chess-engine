#ifndef EVALUATION_H
#define EVALUATION_H

#include "board.h"

class Evaluation {
public:
    static int evaluatePosition(const Board& board);

private:
    // Piece values
    static constexpr int KING_VALUE = 20000;
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

    static int materialScore(const Board& board);
    static double pawnStructureScore(const Board& board);
    static int mobilityScore(const Board& board);
};

#endif
