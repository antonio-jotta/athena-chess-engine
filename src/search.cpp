#include "search.h"

Move Search::findBestMove(Board& board, int depth) {
    MoveGenerator moveGenerator;
    std::vector<Move> move_list;
    moveGenerator.generateAllLegalMoves(board, move_list);
    Move bestMove;
    int bestValue = -999999;
    
    for (const Move& move : move_list) {
        Board tempBoard = board; // Copy the board
        tempBoard.makeMove(move);
        int boardValue = minimax(tempBoard, depth - 1, false);
        if (boardValue > bestValue) {
            bestValue = boardValue;
            bestMove = move;
        }
    }
    return bestMove;
}

int Search::minimax(Board& board, int depth, bool maximizingPlayer) {
    if (depth == 0) {
        return Evaluation::evaluatePosition(board);
    }

    MoveGenerator moveGenerator;
    std::vector<Move> move_list;
    moveGenerator.generateAllLegalMoves(board, move_list);

    if (move_list.empty()) {
        // Checkmate or stalemate
        if(board.side == WHITE) {
            return maximizingPlayer ? -999999 : 999999;
        } else {
            return maximizingPlayer ? 999999 : -999999;
        }
    }

    if (maximizingPlayer) {
        int maxEval = -999999;
        for (const Move& move : move_list) {
            Board tempBoard = board;
            tempBoard.makeMove(move);
            int eval = minimax(tempBoard, depth - 1, false);
            maxEval = std::max(maxEval, eval);
        }
        return maxEval;
    } else {
        int minEval = 999999;
        for (const Move& move : move_list) {
            Board tempBoard = board;
            tempBoard.makeMove(move);
            int eval = minimax(tempBoard, depth - 1, true);
            minEval = std::min(minEval, eval);
        }
        return minEval;
    }
}
