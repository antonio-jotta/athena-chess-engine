#include "evaluation.h"
#include "move_generator.h"

const int PAWN_CENTER_CONTROL_BONUS = 10; // Adjust the factor as needed
const int PASSED_PAWN_BONUS[8] = {0, 5, 10, 20, 40, 80, 200, 0}; // Index by rank
const int CONNECTED_PASSED_PAWN_BONUS = 30; // Adjust as needed
const int DOUBLED_PAWN_PENALTY = -20; // Adjust the factor as needed
const int ISOLATED_PAWN_PENALTY = -15; // Adjust as needed



int Evaluation::evaluatePosition(const Board& board) {
    int whoToMove = (board.side == WHITE) ? 1 : -1;

    int material = materialScore(board);
    int pawnStructure = scorePawnStructure(board, whoToMove);
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

int Evaluation::scorePawnStructure(const Board& board, int side) {
    U64 pawns = board.bitboards[side == WHITE ? WHITE_PAWN : BLACK_PAWN];
    int score = 0;

    U64 tempPawns = pawns; // Copy of pawns to use in pop_lsb
    while (tempPawns) {
        int square = bitscanForward(tempPawns); // Remove LSB and get index
        int rank = square / 8;

        // Central Pawn Bonus
        if (isCentralSquare(square)) {
            score += PAWN_CENTER_CONTROL_BONUS;
        }

        // Passed Pawn
        if (isPassedPawn(board, square, side)) {
            int bonus = PASSED_PAWN_BONUS[rank];
            // Connected Passed Pawn
            if (isConnectedPawn(board, square, side)) {
                bonus += CONNECTED_PASSED_PAWN_BONUS;
            }
            // Isolated Passed Pawn
            if (isIsolatedPawn(board, square, side)) {
                bonus = static_cast<int>(bonus * 0.85); // Apply isolation factor
            }
            score += bonus;
        } else {
            // Isolated Pawn Penalty
            if (isIsolatedPawn(board, square, side)) {
                score += ISOLATED_PAWN_PENALTY;
            }
            // Doubled Pawn Penalty
            if (isDoubledPawn(board, square, side)) {
                score += DOUBLED_PAWN_PENALTY;
            }
        }
        clear_bit(tempPawns, square);
    }

    return score;
}


bool Evaluation::isCentralSquare(int square) {
    // Central squares: d4, d5, e4, e5 (for now)
    return (square == D4 || square == D5 || square == E4 || square == E5);
}

bool Evaluation::isPassedPawn(const Board& board, int square, int side) {
    int file = square % 8;
    int rank = square / 8;
    U64 opponentPawns = board.bitboards[side == WHITE ? BLACK_PAWN : WHITE_PAWN];

    // Create a bitboard mask for the files adjacent to the pawn
    U64 mask = FILE_MASKS[file];
    if (file > 0) mask |= FILE_MASKS[file - 1];
    if (file < 7) mask |= FILE_MASKS[file + 1];

    // For white pawns, check squares ahead
    if (side == WHITE) {
        U64 squaresAhead = 0ULL;
        for (int r = rank + 1; r <= 7; ++r) {
            squaresAhead |= mask & (0xFFULL << (r * 8));
        }
        // If there are no opponent pawns ahead in the mask, it's a passed pawn
        return (opponentPawns & squaresAhead) == 0;
    }
    // For black pawns, check squares behind
    else {
        U64 squaresBehind = 0ULL;
        for (int r = 0; r < rank; ++r) {
            squaresBehind |= mask & (0xFFULL << (r * 8));
        }
        return (opponentPawns & squaresBehind) == 0;
    }
}


bool Evaluation::isConnectedPawn(const Board& board, int square, int side) {
    int rank = square / 8;
    int file = square % 8;
    U64 ownPawns = board.bitboards[side == WHITE ? WHITE_PAWN : BLACK_PAWN];

    // Pawns on adjacent files on the same rank
    U64 connectedPawns = 0ULL;
    if (file > 0) connectedPawns |= ownPawns & (1ULL << (rank * 8 + (file - 1)));
    if (file < 7) connectedPawns |= ownPawns & (1ULL << (rank * 8 + (file + 1)));

    return connectedPawns != 0;
}


bool Evaluation::isIsolatedPawn(const Board& board, int square, int side) {
    int file = square % 8;
    U64 pawns = board.bitboards[side == WHITE ? WHITE_PAWN : BLACK_PAWN];

    U64 adjacentFiles = 0ULL;
    if (file > 0) adjacentFiles |= FILE_MASKS[file - 1];
    if (file < 7) adjacentFiles |= FILE_MASKS[file + 1];

    return !(pawns & adjacentFiles);
}

bool Evaluation::isDoubledPawn(const Board& board, int square, int side) {
    int file = square % 8;
    U64 pawns = board.bitboards[side == WHITE ? WHITE_PAWN : BLACK_PAWN] & FILE_MASKS[file];

    // Count number of pawns on the same file
    return countBits(pawns) > 1;
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

