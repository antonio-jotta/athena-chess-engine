#include "evaluation.h"
#include "move_generator.h"

int Evaluation::evaluatePosition(const Board& board) {
    int whoToMove = (board.side == WHITE) ? 1 : -1;

    int material = materialScore(board);
    double pawnStructure = pawnStructureScore(board);
    int mobility = mobilityScore(board);

    // Return the evaluation relative to the side to move
    return static_cast<int>((material + pawnStructure + mobility) * whoToMove);
}

int Evaluation::materialScore(const Board& board) {
    int score = 0;

    score += KING_VALUE * (countBits(board.bitboards[WHITE_KING]) - countBits(board.bitboards[BLACK_KING]));
    score += QUEEN_VALUE * (countBits(board.bitboards[WHITE_QUEEN]) - countBits(board.bitboards[BLACK_QUEEN]));
    score += ROOK_VALUE * (countBits(board.bitboards[WHITE_ROOK]) - countBits(board.bitboards[BLACK_ROOK]));
    score += BISHOP_VALUE * (countBits(board.bitboards[WHITE_BISHOP]) - countBits(board.bitboards[BLACK_BISHOP]));
    score += KNIGHT_VALUE * (countBits(board.bitboards[WHITE_KNIGHT]) - countBits(board.bitboards[BLACK_KNIGHT]));
    score += PAWN_VALUE * (countBits(board.bitboards[WHITE_PAWN]) - countBits(board.bitboards[BLACK_PAWN]));

    return score;
}

double Evaluation::pawnStructureScore(const Board& board) {
    double score = 0.0;

    // Example of penalty for isolated pawns
    // Add more detailed pawn structure evaluation later

    return score;
}

int Evaluation::mobilityScore(const Board& board) {
    MoveGenerator moveGenerator;
    std::vector<Move> whiteMoves, blackMoves;

    // Create a copy of the board for each side's mobility
    Board whiteBoard = board;
    whiteBoard.side = WHITE;
    moveGenerator.generateAllLegalMoves(whiteBoard, whiteMoves);

    Board blackBoard = board;
    blackBoard.side = BLACK;
    moveGenerator.generateAllLegalMoves(blackBoard, blackMoves);

    int whiteMobility = whiteMoves.size();
    int blackMobility = blackMoves.size();

    return static_cast<int>(MOBILITY_WEIGHT * (whiteMobility - blackMobility));
}

