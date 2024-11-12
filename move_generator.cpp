#include "move_generator.h"

void MoveGenerator::generateAllMoves(const Board& board, std::vector<Move>& move_list){
    // generatePawnMoves(board, move_list);
    // generateKnightMoves(board, move_list);
    // generateBishopMoves(board, move_list);
    generateRookMoves(board, move_list);
    // generateQueenMoves(board, move_list);
    // generateKingMoves(board, move_list);
}

// void MoveGenerator::generatePawnMoves(const Board& board, std::vector<Move>& move_list){
    
// }

void MoveGenerator::generateRookMoves(const Board& board, std::vector<Move>& move_list) {
    int side = board.side;
    int opponent_side = (side == WHITE) ? BLACK : WHITE;
    int rook_piece = (side == WHITE) ? WHITE_ROOK : BLACK_ROOK;
    U64 rooks = board.bitboards[rook_piece];

    // Define the direction offsets for the rook (right, left, up, down)
    const int directions[4] = { +1, -1, +8, -8 };

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
        // Boundary check to prevent wrapping around the board
        if (isBoundaryCrossed(start_square, to_square, direction_offset)) {
            break;
        }

        // Check if the square is occupied by a friendly piece
        if (get_bit(board.occupancies[side], to_square)) {
            break; // Friendly piece blocks the way
        }

        // Prepare move details
        int captured_piece = NO_PIECE;
        uint8_t flags = FLAG_NONE;

        // Check if the square is occupied by an opponent's piece
        if (get_bit(board.occupancies[opponent_side], to_square)) {
            // Capture move
            captured_piece = getPieceOnSquare(board, to_square, opponent_side);
            flags |= FLAG_CAPTURE;

            // Add the capture move
            move_list.emplace_back(
                start_square,
                to_square,
                piece_type,
                captured_piece,
                NO_PIECE, // promoted_piece
                flags
            );

            break; // Stop after capturing
        } else {
            // Regular move
            move_list.emplace_back(
                start_square,
                to_square,
                piece_type,
                NO_PIECE,   // captured_piece
                NO_PIECE,   // promoted_piece
                flags
            );
        }

        // Move to the next square in the direction
        to_square += direction_offset;
    }
}


bool MoveGenerator::isBoundaryCrossed(int from_square, int to_square, int direction_offset) {
    // For horizontal moves (left and right), check if we stay on the same rank
    if (direction_offset == -1 || direction_offset == 1) {
        return (from_square / 8) != (to_square / 8);
    }
    // For diagonal moves, ensure we don't wrap around horizontally
    if (direction_offset == 9 || direction_offset == -9 ||
        direction_offset == 7 || direction_offset == -7) {
        int from_file = from_square % 8;
        int to_file = to_square % 8;
        return std::abs(from_file - to_file) != 1;
    }
    // For vertical moves (up and down), no boundary crossing occurs
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
