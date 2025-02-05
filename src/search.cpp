#include "search.h"

long long Search::nodes_searched = 0;


Move Search::findBestMove(Board& board, int depth) {
    nodes_searched = 0;
    MoveGenerator moveGenerator;
    std::vector<Move> move_list;
    moveGenerator.generateAllLegalMoves(board, move_list);
    orderMoves(move_list, board); // Move ordering for better pruning fo the search tree
    Move bestMove;
    int bestValue = INT_MIN;
    int alpha = INT_MIN;
    int beta = INT_MAX;
    auto start = std::chrono::high_resolution_clock::now();
    for (const Move& move : move_list) {
        Board tempBoard = board;
        tempBoard.makeMove(move);

        int score = -negamax(tempBoard, depth - 1, -beta, -alpha);

        if (score > bestValue) {
            bestValue = score;
            bestMove = move;
        }
        if (bestValue > alpha) {
            alpha = bestValue;
        }
    }
    auto end = std::chrono::high_resolution_clock::now();   
    std::chrono::duration<double> duration = end - start;
    std::cout << "Time taken for the search: " << duration.count() << " seconds\n";

    std::cout << "Leaf nodes evaluated: " << nodes_searched << std::endl;

    return bestMove;
}


int Search::negamax(Board& board, int depth, int alpha, int beta) {
    if (depth == 0) {
        return quiescence(board, alpha, beta);
    }

    MoveGenerator moveGenerator;
    std::vector<Move> move_list;
    moveGenerator.generateAllLegalMoves(board, move_list);
    orderMoves(move_list, board); 

    if (move_list.empty()) {
        if (moveGenerator.isKingInCheck(board, board.side)) {
            return -999999 + depth; // Checkmate
        } else {
            return 0; // Stalemate
        }
    }

    int bestValue = -999999;

    for (const Move& move : move_list) {
        Board tempBoard = board;
        tempBoard.makeMove(move);

        int score = -negamax(tempBoard, depth - 1, -beta, -alpha);

        if (score > bestValue) {
            bestValue = score;
        }
        if (bestValue > alpha) {
            alpha = bestValue;
        }
        if (alpha >= beta) {
            break; // Beta cutoff
        }
    }
    return bestValue;
}


int Search::quiescence(Board& board, int alpha, int beta) {

    nodes_searched++;

    int stand_pat = Evaluation::evaluatePosition(board);
    if (stand_pat >= beta) {
        return beta;
    }
    if (alpha < stand_pat) {
        alpha = stand_pat;
    }

    MoveGenerator moveGenerator;
    std::vector<Move> capture_moves;
    moveGenerator.generateAllCaptureMoves(board, capture_moves);
    orderMoves(capture_moves, board); 


    for (const Move& move : capture_moves) {
        Board tempBoard = board;
        tempBoard.makeMove(move);

        int score = -quiescence(tempBoard, -beta, -alpha);

        if (score >= beta) {
            return beta;
        }
        if (score > alpha) {
            alpha = score;
        }
    }
    return alpha;
}

int Search::scoreMove(const Move& move, const Board& board) {
    int score = 0;
    if (move.captured_piece != NO_PIECE) {
        int victimValue = Evaluation::getPieceValue(move.captured_piece);
        int attackerValue = Evaluation::getPieceValue(move.piece);
        score += 1000 + (victimValue - attackerValue);
    }
    if (move.promoted_piece != NO_PIECE) {
        score += 800;
    }
    if (MoveGenerator::isKingInCheck(board, board.side)) {
        score += 500;
    }
    // For the future: can add more heruistics to improve move scoring
    return score;
}

void Search::orderMoves(std::vector<Move>& move_list, Board& board) {
    std::vector<std::pair<int, Move>> scoredMoves;
    for (const Move& move : move_list) {
        int score = scoreMove(move, board);
        scoredMoves.emplace_back(score, move);
    }
    std::sort(scoredMoves.begin(), scoredMoves.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });
    move_list.clear();
    for (const auto& pair : scoredMoves) {
        move_list.push_back(pair.second);
    }
}
