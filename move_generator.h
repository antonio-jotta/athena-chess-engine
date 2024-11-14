#ifndef MOVE_GENERATOR_H
#define MOVE_GENERATOR_H

#include "board.h"
#include "move.h"
#include <vector>


class MoveGenerator{
    public:
        void generateAllMoves(const Board& board, std::vector<Move>& move_list);
        // Direction offsets
        static constexpr int NORTH = +8;
        static constexpr int SOUTH = -8;
        static constexpr int EAST  = +1;
        static constexpr int WEST  = -1;
        static constexpr int NORTH_EAST = +9;
        static constexpr int NORTH_WEST = +7;
        static constexpr int SOUTH_EAST = -7;
        static constexpr int SOUTH_WEST = -9;
        void generatePawnMoves(const Board& board, std::vector<Move>& move_list);
        void generateKnightMoves(const Board& board, std::vector<Move>& move_list);
        void generateBishopMoves(const Board& board, std::vector<Move>& move_list);
        void generateRookMoves(const Board& board, std::vector<Move>& move_list);
        void generateQueenMoves(const Board& board, std::vector<Move>& move_list);
        void generateKingMoves(const Board& board, std::vector<Move>& move_list);
        void generateCastlingMoves(const Board& board, std::vector<Move>& move_list, int king_square, int side);
        bool canCastleKingSide(const Board& board, int side);
        bool canCastleQueenSide(const Board& board, int side);
        bool isSafeToCastle(const Board& board, int king_square, int side, const std::string& castling_type);

        void generateSlidingMovesInDirection(const Board& board, 
            std::vector<Move>& move_list,
            int start_square,
            int direction_offset,
            int side,
            int opponent_side,
            int piece_type);
        void generateAllLegalMoves(const Board& board, std::vector<Move>& move_list);
        bool isKingInCheck(const Board& board, int side);
        bool isSquareAttackedByPawn(const Board& board, int square, int opponent_side);
        bool isSquareAttackedByKnight(const Board& board, int square, int opponent_side);
        bool isSquareAttackedByBishop(const Board& board, int square, int opponent_side);
        bool isSquareAttackedByQueen(const Board& board, int square, int opponent_side);
        bool isSquareAttackedByRook(const Board& board, int square, int opponent_side);
        bool isSquareAttackedByKing(const Board& board, int square, int opponent_side);
        bool isBoundaryCrossed(int from_square, int to_square, int direction_offset);
        bool isKnightMoveBoundaryCrossed(int from_square, int to_square);
        int getPieceOnSquare(const Board& board, int square, int opponent_side);

};

#endif