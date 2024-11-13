#include "move_generator.h"

void MoveGenerator::generateAllMoves(const Board& board, std::vector<Move>& move_list){
    // generatePawnMoves(board, move_list);
    // generateKnightMoves(board, move_list);
    // generateBishopMoves(board, move_list);
    generateRookMoves(board, move_list);
    // generateQueenMoves(board, move_list);
    generateKingMoves(board, move_list);
}

void MoveGenerator::generateAllLegalMoves(const Board& board, std::vector<Move>& move_list) {
    // Generate pseudolegal moves
    std::vector<Move> pseudolegal_moves;
    generateAllMoves(board, pseudolegal_moves);

    // For each move, check if it's legal
    for (const Move& move : pseudolegal_moves) {
        // Make a copy of the board
        Board board_copy = board;

        // Make the move on the copy, do not switch sides
        board_copy.makeMove(move, false);

        // Check if the king is in check after the move
        if (!isKingInCheck(board_copy, board.side)) {
            // Move is legal, add to move_list
            move_list.push_back(move);
        }else {
            // Debugging output
            // std::cout << "Move " << squareToAlgebraic(move.from_square)
            //           << " -> " << squareToAlgebraic(move.to_square)
            //           << " is illegal; king is in check after this move.\n";
        }
    }
}


// void MoveGenerator::generatePawnMoves(const Board& board, std::vector<Move>& move_list){
    
// }

void MoveGenerator::generateRookMoves(const Board& board, std::vector<Move>& move_list) {
    int side = board.side;
    int opponent_side = (side == WHITE) ? BLACK : WHITE;
    int rook_piece = (side == WHITE) ? WHITE_ROOK : BLACK_ROOK;
    U64 rooks = board.bitboards[rook_piece];

    // Define the direction offsets for the rook 
    const int directions[4] = {NORTH, SOUTH, EAST, WEST}; 

    // Loop through each rook
    while (rooks) {
        int rook_square = bitscanForward(rooks);
        clear_bit(rooks, rook_square);

        // Generate moves in all four directions
        for (int dir = 0; dir < 4; ++dir) {
            int direction_offset = directions[dir];
            generateSlidingMovesInDirection(
                board,
                move_list,
                rook_square,
                direction_offset,
                side,
                opponent_side,
                rook_piece
            );
        }
    }
}

void MoveGenerator::generateKingMoves(const Board& board, std::vector<Move>& move_list){
    int side = board.side;
    int opponent_side = (side == WHITE) ? BLACK : WHITE;
    int king_piece = (side == WHITE) ? WHITE_KING : BLACK_KING;
    U64 king = board.bitboards[king_piece];

    int king_square = bitscanForward(king);
    clear_bit(king, king_square);

    const int directions[8] = {NORTH, SOUTH, EAST, WEST, NORTH_EAST, NORTH_WEST, SOUTH_EAST, SOUTH_WEST}; 
            // Generate moves in all directions
        for (int dir = 0; dir < 8; ++dir) {
            int direction_offset = directions[dir];
            int to_square = king_square + direction_offset;
            if (to_square < 0 || to_square >= 64 || isBoundaryCrossed(king_square, to_square, direction_offset)) {
                std::cout << "Boundary crossed at " << squareToAlgebraic(to_square) << "\n";
            continue;
            }
            // Check if square is occupied by a friendly piece
            if (get_bit(board.occupancies[side], to_square)) {
                //std::cout << "Blocked by friendly piece at " << squareToAlgebraic(to_square) << "\n";
                continue;
            }
            int captured_piece = NO_PIECE;
            uint8_t flags = FLAG_NONE;

            // Check if occupied by an opponent's piece
            if (get_bit(board.occupancies[opponent_side], to_square)) {
                captured_piece = getPieceOnSquare(board, to_square, opponent_side);
                flags |= FLAG_CAPTURE;
                move_list.emplace_back(king_square, to_square, king_piece, captured_piece, NO_PIECE, flags);
                //std::cout << "Captured opponent piece at " << squareToAlgebraic(to_square) << "\n";
                continue;
            } else {
                move_list.emplace_back(king_square, to_square, king_piece, NO_PIECE, NO_PIECE, flags);
            }
        }
}


void MoveGenerator::generateSlidingMovesInDirection(
    const Board& board,
    std::vector<Move>& move_list,
    int start_square,
    int direction_offset,
    int side,
    int opponent_side,
    int piece_type)
{
    int to_square = start_square + direction_offset;

    while (to_square >= 0 && to_square < 64) {
        // Debug output for each evaluated square
        //std::cout << "Evaluating move from " << squareToAlgebraic(start_square) << " to " << squareToAlgebraic(to_square) << "\n";

        if (isBoundaryCrossed(start_square, to_square, direction_offset)) {
            //std::cout << "Boundary crossed at " << squareToAlgebraic(to_square) << "\n";
            break;
        }

        // Check if square is occupied by a friendly piece
        if (get_bit(board.occupancies[side], to_square)) {
            //std::cout << "Blocked by friendly piece at " << squareToAlgebraic(to_square) << "\n";
            break;
        }

        int captured_piece = NO_PIECE;
        uint8_t flags = FLAG_NONE;

        // Check if occupied by an opponent's piece
        if (get_bit(board.occupancies[opponent_side], to_square)) {
            captured_piece = getPieceOnSquare(board, to_square, opponent_side);
            flags |= FLAG_CAPTURE;
            move_list.emplace_back(start_square, to_square, piece_type, captured_piece, NO_PIECE, flags);
            //std::cout << "Captured opponent piece at " << squareToAlgebraic(to_square) << "\n";
            break;
        } else {
            move_list.emplace_back(start_square, to_square, piece_type, NO_PIECE, NO_PIECE, flags);
        }

        to_square += direction_offset;
    }
}

bool MoveGenerator::isKingInCheck(const Board& board, int side) {
    int king_square = bitscanForward(board.bitboards[(side == WHITE) ? WHITE_KING : BLACK_KING]);

    // Generate attack bitboards for opponent
    int opponent_side = (side == WHITE) ? BLACK : WHITE;

    // Check for attacks from pawns, knights, bishops, rooks, queens, and the opponent's king
    //if (isSquareAttackedByPawn(board, king_square, opponent_side)) return true;
    //if (isSquareAttackedByKnight(board, king_square, opponent_side)) return true;
    //if (isSquareAttackedByBishopOrQueen(board, king_square, opponent_side)) return true;
    if (isSquareAttackedByRook(board, king_square, opponent_side)) return true;
    if (isSquareAttackedByKing(board, king_square, opponent_side)) return true;

    // No attacks on the king
    return false;
}

bool MoveGenerator::isSquareAttackedByRook(const Board& board, int square, int opponent_side) {
    U64 rooks = board.bitboards[(opponent_side == WHITE) ? WHITE_ROOK : BLACK_ROOK];
    U64 pieces = rooks;

    while (pieces) {
        int piece_square = bitscanForward(pieces);
        clear_bit(pieces, piece_square);

        // Generate rook attacks inline
        const int directions[4] = {NORTH, SOUTH, EAST, WEST}; 
        for (int dir = 0; dir < 4; ++dir) {
            int to_square = piece_square;

            while (true) {
                to_square += directions[dir];
                if (to_square < 0 || to_square >= 64 || isBoundaryCrossed(piece_square, to_square, directions[dir])) {
                    break;
                }

                if (get_bit(board.occupancies[BOTH], to_square)) {
                    if (to_square == square) {
                        return true; // Target square is attacked
                    }
                    break; // Blocked by other piece
                }

                if (to_square == square) {
                    return true; // Target square is attacked
                }
            }
        }
    }

    // No attacks found
    return false;
}

bool MoveGenerator::isSquareAttackedByKing(const Board& board, int square, int opponent_side){
    U64 kings = board.bitboards[(opponent_side == WHITE) ? WHITE_KING : BLACK_KING];

    while(kings){
        int piece_square = bitscanForward(kings);
        clear_bit(kings, piece_square);
        const int directions[8] = {NORTH, SOUTH, EAST, WEST, NORTH_EAST, NORTH_WEST, SOUTH_EAST, SOUTH_WEST}; 
        for(int dir = 0; dir < 8; ++dir){
            int to_square = piece_square;
            to_square += directions[dir];
            // std::cout << "Checking square: " << squareToAlgebraic(to_square) << "\n";
            if (to_square < 0 || to_square >= 64 || isBoundaryCrossed(piece_square, to_square, directions[dir])) {
                break;
            }
            if (get_bit(board.occupancies[BOTH], to_square)) {
                    if (to_square == square) {
                        return true; // Target square is attacked
                    }
                    break; // Blocked by other piece
                }

                if (to_square == square) {
                    return true; // Target square is attacked
                }
        }
    }
    return false;
}


bool MoveGenerator::isBoundaryCrossed(int from_square, int to_square, int direction_offset) {
    if (direction_offset == EAST || direction_offset == WEST) {
        // Horizontal moves (E/W)
        return (from_square / 8) != (to_square / 8);
    } else if (direction_offset == NORTH_EAST || direction_offset == SOUTH_EAST) {
        // Northeast/Southwest moves
        return ((from_square % 8) == NORTH_WEST) || ((to_square % 8) == 0);
    } else if (direction_offset == NORTH_WEST || direction_offset == SOUTH_WEST) {
        // Northwest/Southeast moves
        return ((from_square % 8) == 0) || ((to_square % 8) == NORTH_WEST);
    }
    // Vertical moves (N/S) don't cross boundaries
    return false;
}


int MoveGenerator::getPieceOnSquare(const Board& board, int square, int opponent_side) {
    int opponent_start = (opponent_side == WHITE) ? WHITE_PAWN : BLACK_PAWN;
    int opponent_end = (opponent_side == WHITE) ? WHITE_KING : BLACK_KING;

    for (int piece = opponent_start; piece <= opponent_end; ++piece) {
        if (get_bit(board.bitboards[piece], square)) {
            return piece;
        }
    }
    return NO_PIECE;
}
