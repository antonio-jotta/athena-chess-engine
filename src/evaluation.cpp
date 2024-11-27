#include "evaluation.h"


// ALL OF THESE ARE SUBJECT TO ADJUSTMENTS
const int PAWN_CENTER_CONTROL_BONUS = 10; 
const int PASSED_PAWN_BONUS[8] = {0, 5, 10, 20, 40, 80, 200, 0}; // Index by rank
const int CONNECTED_PASSED_PAWN_BONUS = 30; 
const int DOUBLED_PAWN_PENALTY = -20; 
const int ISOLATED_PAWN_PENALTY = -15; 

// Piece-square tables (values for white; values for black will be mirrored)
const int Evaluation::pawnTable[64] = {
     0,   0,   0,   0,   0,   0,   0,   0,
    50,  50,  50,  50,  50,  50,  50,  50,
    10,  10,  20,  30,  30,  20,  10,  10,
     5,   5,  10,  25,  25,  10,   5,   5,
     0,   0,   0,  20,  20,   0,   0,   0,
     5,  -5, -10,   0,   0, -10,  -5,   5,
     5,  10,  10, -20, -20,  10,  10,   5,
     0,   0,   0,   0,   0,   0,   0,   0
};

// Similarly define knightTable, bishopTable, rookTable, queenTable

const int Evaluation::knightTable[64] = {
    -50, -40, -30, -30, -30, -30, -40, -50,
    -40, -20,   0,   0,   0,   0, -20, -40,
    -30,   0,  10,  15,  15,  10,   0, -30,
    -30,   5,  15,  20,  20,  15,   5, -30,
    -30,   0,  15,  20,  20,  15,   0, -30,
    -30,   5,  10,  15,  15,  10,   5, -30,
    -40, -20,   0,   5,   5,   0, -20, -40,
    -50, -40, -30, -30, -30, -30, -40, -50
};

// Continue for bishopTable, rookTable, queenTable

const int Evaluation::bishopTable[64] = {
    -20, -10, -10, -10, -10, -10, -10, -20,
    -10,   5,   0,   0,   0,   0,   5, -10,
    -10,  10,  10,  10,  10,  10,  10, -10,
    -10,   0,  10,  10,  10,  10,   0, -10,
    -10,   5,   5,  10,  10,   5,   5, -10,
    -10,   0,   5,  10,  10,   5,   0, -10,
    -10,   0,   0,   0,   0,   0,   0, -10,
    -20, -10, -10, -10, -10, -10, -10, -20
};

// Define rookTable

const int Evaluation::rookTable[64] = {
     0,   0,   0,   5,   5,   0,   0,   0,
    -5,   0,   0,   0,   0,   0,   0,  -5,
    -5,   0,   0,   0,   0,   0,   0,  -5,
    -5,   0,   0,   0,   0,   0,   0,  -5,
    -5,   0,   0,   0,   0,   0,   0,  -5,
    -5,   0,   0,   0,   0,   0,   0,  -5,
     5,  10,  10,  10,  10,  10,  10,   5,
     0,   0,   0,   0,   0,   0,   0,   0
};

// Define queenTable

const int Evaluation::queenTable[64] = {
    -20, -10, -10,  -5,  -5, -10, -10, -20,
    -10,   0,   0,   0,   0,   0,   0, -10,
    -10,   0,   5,   5,   5,   5,   0, -10,
     -5,   0,   5,   5,   5,   5,   0,  -5,
      0,   0,   5,   5,   5,   5,   0,  -5,
    -10,   5,   5,   5,   5,   5,   0, -10,
    -10,   0,   5,   0,   0,   0,   0, -10,
    -20, -10, -10,  -5,  -5, -10, -10, -20
};

// King tables for middle game and end game
const int Evaluation::kingMiddleGameTable[64] = {
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -20, -30, -30, -40, -40, -30, -30, -20,
    -10, -20, -20, -20, -20, -20, -20, -10,
     20,  20,   0,   0,   0,   0,  20,  20,
     20,  30,  10,   0,   0,  10,  30,  20
};

const int Evaluation::kingEndGameTable[64] = {
    -50, -40, -30, -20, -20, -30, -40, -50,
    -30, -20, -10,   0,   0, -10, -20, -30,
    -30, -10,  20,  30,  30,  20, -10, -30,
    -30, -10,  30,  40,  40,  30, -10, -30,
    -30, -10,  30,  40,  40,  30, -10, -30,
    -30, -10,  20,  30,  30,  20, -10, -30,
    -30, -30,   0,   0,   0,   0, -30, -30,
    -50, -30, -30, -30, -30, -30, -30, -50
};


int Evaluation::evaluatePosition(const Board& board){
    int whoToMove = (board.side == WHITE) ? 1 : -1;
    
    MoveGenerator moveGenerator;
    std::vector<Move> kingMoves;
    moveGenerator.generateEnemyKingMoves(board, kingMoves);
    if(kingMoves.empty() && moveGenerator.isKingInCheck(board, board.side)){
        return INT_MAX;
    }


    int material = materialScore(board);
    int pawnStructure = scorePawnStructure(board, whoToMove);
    int mobility = mobilityScore(board);
    int positional = pieceSquareScore(board);

    if(board.DRAW == 1){
        return 0;
    }
    int totalScore = material + pawnStructure + mobility + positional;

    // Return the evaluation relative to the side to move
    return totalScore * whoToMove;
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

int Evaluation::getPieceValue(const int piece) {
    switch(piece) {
        case WHITE_KING:
        case BLACK_KING:
            return KING_VALUE;

        case WHITE_QUEEN:
        case BLACK_QUEEN:
            return QUEEN_VALUE;

        case WHITE_ROOK:
        case BLACK_ROOK:
            return ROOK_VALUE;

        case WHITE_BISHOP:
        case BLACK_BISHOP:
            return BISHOP_VALUE;

        case WHITE_KNIGHT:
        case BLACK_KNIGHT:
            return KNIGHT_VALUE;

        case WHITE_PAWN:
        case BLACK_PAWN:
            return PAWN_VALUE;

        default:
            return 0; // For empty squares or invalid pieces
    }
}


int Evaluation::scorePawnStructure(const Board& board, int side) {
    U64 pawns = board.bitboards[side == WHITE ? WHITE_PAWN : BLACK_PAWN];
    int score = 0;

    U64 tempPawns = pawns; 
    while (tempPawns) {
        int square = bitscanForward(tempPawns); // get the index for the LSB
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
        clear_bit(tempPawns, square); // remove the LSB and continue the loop
    }

    return score;
}


bool Evaluation::isCentralSquare(int square) {
    // (for now): maybe in the future give slight bonus to C and F file?
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


int Evaluation::pieceSquareScore(const Board& board) {
    int score = 0;
    bool endGame = isEndGame(board);

    // Score white pieces
    for (int piece = WHITE_PAWN; piece <= WHITE_KING; ++piece) {
        U64 bitboard = board.bitboards[piece];
        while (bitboard) {
            int square = bitscanForward(bitboard);
            clear_bit(bitboard, square);

            switch (piece) {
                case WHITE_PAWN:
                    score += pawnTable[square];
                    break;
                case WHITE_KNIGHT:
                    score += knightTable[square];
                    break;
                case WHITE_BISHOP:
                    score += bishopTable[square];
                    break;
                case WHITE_ROOK:
                    score += rookTable[square];
                    break;
                case WHITE_QUEEN:
                    score += queenTable[square];
                    break;
                case WHITE_KING:
                    if (endGame) {
                        score += kingEndGameTable[square];
                    } else {
                        score += kingMiddleGameTable[square];
                    }
                    break;
            }
        }
    }

    // Score black pieces
    for (int piece = BLACK_PAWN; piece <= BLACK_KING; ++piece) {
        U64 bitboard = board.bitboards[piece];
        while (bitboard) {
            int square = bitscanForward(bitboard);
            clear_bit(bitboard, square);

            int mirroredSquare = mirrorSquare(square);

            switch (piece) {
                case BLACK_PAWN:
                    score -= pawnTable[mirroredSquare];
                    break;
                case BLACK_KNIGHT:
                    score -= knightTable[mirroredSquare];
                    break;
                case BLACK_BISHOP:
                    score -= bishopTable[mirroredSquare];
                    break;
                case BLACK_ROOK:
                    score -= rookTable[mirroredSquare];
                    break;
                case BLACK_QUEEN:
                    score -= queenTable[mirroredSquare];
                    break;
                case BLACK_KING:
                    if (endGame) {
                        score -= kingEndGameTable[mirroredSquare];
                    } else {
                        score -= kingMiddleGameTable[mirroredSquare];
                    }
                    break;
            }
        }
    }

    return score;
}


bool Evaluation::isEndGame(const Board& board) {
    int totalMaterial = 0;

    // Exclude pawns and kings (piece enums 0 and 5 for white, 6 and 11 for black)
    // White minor and major pieces (knight to queen)
    for (int piece = WHITE_KNIGHT; piece <= WHITE_QUEEN; ++piece) {
        int count = countBits(board.bitboards[piece]);
        int pieceValue = getPieceValue(piece);
        totalMaterial += count * pieceValue;
    }

    // Black minor and major pieces (knight to queen)
    for (int piece = BLACK_KNIGHT; piece <= BLACK_QUEEN; ++piece) {
        int count = countBits(board.bitboards[piece]);
        int pieceValue = getPieceValue(piece);
        totalMaterial += count * pieceValue;
    }

    // If total material is below a threshold, consider it endgame
    return totalMaterial <= 2400; // Adjust this threshold as needed
}



int Evaluation::mirrorSquare(int square) {
    // Assuming square indices from 0 (a1) to 63 (h8)
    int rank = square / 8;
    int file = square % 8;
    // Mirror the rank (0 becomes 7, 1 becomes 6, ..., 7 becomes 0)
    int mirroredRank = 7 - rank;
    return mirroredRank * 8 + file;
}
