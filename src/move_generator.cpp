#include "move_generator.h"

void MoveGenerator::generateAllMoves(const Board& board, std::vector<Move>& move_list){
    generatePawnMoves(board, move_list);
    generateKnightMoves(board, move_list);
    generateBishopMoves(board, move_list);
    generateRookMoves(board, move_list);
    generateQueenMoves(board, move_list);
    generateKingMoves(board, move_list);
}

void MoveGenerator::generateAllLegalMoves(const Board& board, std::vector<Move>& move_list) {
    // Generate all pseudolegal moves
    std::vector<Move> pseudolegal_moves;
    generateAllMoves(board, pseudolegal_moves);
    
    // For each move, check if it's legal
    for (const Move& move : pseudolegal_moves) {
        // Make a copy of the board
        Board board_copy = board;

        // Make the move on the copy, do not switch sides
        board_copy.makeMove(move, false);
        // std::cout << "Move " << squareToAlgebraic(move.from_square)<< " -> " << squareToAlgebraic(move.to_square) << "\n";
        
        // Check if the king is in check after the move
        if (!isKingInCheck(board_copy, board.side)) {
            // Move is legal, add to move_list
            move_list.push_back(move);
        }
        // else {
        //     // Debugging output
        //     // std::cout << "Move " << squareToAlgebraic(move.from_square)
        //     //           << " -> " << squareToAlgebraic(move.to_square)
        //     //           << " is illegal; king is in check after this move.\n";
        // }
    }
}

void MoveGenerator::generateAllCaptureMoves(const Board& board, std::vector<Move>& move_list) {
    // Generate all legal moves first
    std::vector<Move> all_moves;
    generateAllLegalMoves(board, all_moves);
    
    // Iterate through all moves and select only the capture moves
    for (const Move& move : all_moves) {
        if (move.captured_piece != NO_PIECE) {
            move_list.push_back(move);
        }
    }
}



void MoveGenerator::generatePawnMoves(const Board& board, std::vector<Move>& move_list) {
    int side = board.side;
    int opponent_side = (side == WHITE) ? BLACK : WHITE;
    int pawn_piece = (side == WHITE) ? WHITE_PAWN : BLACK_PAWN;
    U64 pawns = board.bitboards[pawn_piece];
    int pawn_push = (side == WHITE) ? NORTH : SOUTH;
    int start_rank = (side == WHITE) ? 1 : 6;
    int promotion_rank = (side == WHITE) ? 7 : 0;
    int en_passant_square = board.en_passant; // -1 if not available

    // Direction offsets for pawn captures
    int capture_offsets[2];
    if (side == WHITE) {
        capture_offsets[0] = NORTH_WEST;
        capture_offsets[1] = NORTH_EAST;
    } else {
        capture_offsets[0] = SOUTH_WEST;
        capture_offsets[1] = SOUTH_EAST;
    }

    while (pawns) {
        int pawn_square = bitscanForward(pawns);
        clear_bit(pawns, pawn_square);

        int pawn_rank = pawn_square / 8;

        // **Single Forward Move**
        int to_square = pawn_square + pawn_push;
        if (to_square >= 0 && to_square < 64 && !get_bit(board.occupancies[BOTH], to_square)) {
            int to_rank = to_square / 8;

            // **Promotion Handling**
            if (to_rank == promotion_rank) {
                // Promote to Queen, Rook, Bishop, Knight
                int promotion_pieces[4] = {
                    (side == WHITE) ? WHITE_QUEEN : BLACK_QUEEN,
                    (side == WHITE) ? WHITE_ROOK : BLACK_ROOK,
                    (side == WHITE) ? WHITE_BISHOP : BLACK_BISHOP,
                    (side == WHITE) ? WHITE_KNIGHT : BLACK_KNIGHT
                };

                for (int i = 0; i < 4; ++i) {
                    move_list.emplace_back(
                        pawn_square,
                        to_square,
                        pawn_piece,
                        NO_PIECE,
                        promotion_pieces[i],
                        FLAG_PROMOTION
                    );
                }
            } else {
                // **Normal Single Move**
                move_list.emplace_back(
                    pawn_square,
                    to_square,
                    pawn_piece,
                    NO_PIECE,
                    NO_PIECE,
                    FLAG_NONE
                );

                // **Double Forward Move**
                if (pawn_rank == start_rank) {
                    int to_square2 = to_square + pawn_push;
                    if (!get_bit(board.occupancies[BOTH], to_square2)) {
                        move_list.emplace_back(
                            pawn_square,
                            to_square2,
                            pawn_piece,
                            NO_PIECE,
                            NO_PIECE,
                            FLAG_PAWN_DOUBLE_PUSH
                        );
                    }
                }
            }
        }

        // **Captures**
        for (int i = 0; i < 2; ++i) {
            int capture_offset = capture_offsets[i];
            int capture_square = pawn_square + capture_offset;

            if (capture_square >= 0 && capture_square < 64) {
                int from_file = pawn_square % 8;
                int to_file = capture_square % 8;

                // Ensure no wrap-around
                if (abs(from_file - to_file) == 1) {
                    // **Normal Capture**
                    if (get_bit(board.occupancies[opponent_side], capture_square)) {
                        int captured_piece = getPieceOnSquare(board, capture_square, opponent_side);
                        int to_rank = capture_square / 8;

                        // **Promotion Capture**
                        if (to_rank == promotion_rank) {
                            int promotion_pieces[4] = {
                                (side == WHITE) ? WHITE_QUEEN : BLACK_QUEEN,
                                (side == WHITE) ? WHITE_ROOK : BLACK_ROOK,
                                (side == WHITE) ? WHITE_BISHOP : BLACK_BISHOP,
                                (side == WHITE) ? WHITE_KNIGHT : BLACK_KNIGHT
                            };

                            for (int j = 0; j < 4; ++j) {
                                move_list.emplace_back(
                                    pawn_square,
                                    capture_square,
                                    pawn_piece,
                                    captured_piece,
                                    promotion_pieces[j],
                                    FLAG_PROMOTION | FLAG_CAPTURE
                                );
                            }
                        } else {
                            // **Normal Capture Move**
                            move_list.emplace_back(
                                pawn_square,
                                capture_square,
                                pawn_piece,
                                captured_piece,
                                NO_PIECE,
                                FLAG_CAPTURE
                            );
                        }
                    }

                    // **En Passant Capture**
                    else if (capture_square == en_passant_square) {
                        move_list.emplace_back(
                            pawn_square,
                            capture_square,
                            pawn_piece,
                            (opponent_side == WHITE) ? WHITE_PAWN : BLACK_PAWN,
                            NO_PIECE,
                            FLAG_EN_PASSANT
                        );
                    }
                }
            }
        }
    }
}


void MoveGenerator::generateKnightMoves(const Board& board, std::vector<Move>& move_list) {
    int side = board.side;
    int opponent_side = (side == WHITE) ? BLACK : WHITE;
    int knight_piece = (side == WHITE) ? WHITE_KNIGHT : BLACK_KNIGHT;
    U64 knights = board.bitboards[knight_piece];

    // Loop through each knight
    while (knights) {
        int knight_square = bitscanForward(knights);
        clear_bit(knights, knight_square);

        // Possible knight move offsets
        const int knight_offsets[8] = {
            17, 15, 10, 6, -6, -10, -15, -17
        };

        // Generate moves for the knight
        for (int i = 0; i < 8; ++i) {
            int to_square = knight_square + knight_offsets[i];

            // Check if the destination square is within the board bounds
            if (to_square >= 0 && to_square < 64) {
                // Check for boundary crossing to prevent wrap-around
                if (!isKnightMoveBoundaryCrossed(knight_square, to_square)) {
                    // Check if the destination square is occupied by a friendly piece
                    if (!get_bit(board.occupancies[side], to_square)) {
                        int captured_piece = NO_PIECE;
                        uint8_t flags = FLAG_NONE;

                        // Check if the destination square is occupied by an opponent piece
                        if (get_bit(board.occupancies[opponent_side], to_square)) {
                            captured_piece = getPieceOnSquare(board, to_square, opponent_side);
                            flags |= FLAG_CAPTURE;
                        }

                        // Add the move to the move list
                        move_list.emplace_back(
                            knight_square,
                            to_square,
                            knight_piece,
                            captured_piece,
                            NO_PIECE, // promoted_piece (not applicable for knights)
                            flags
                        );
                    }
                }
            }
        }
    }
}


void MoveGenerator::generateBishopMoves(const Board& board, std::vector<Move>& move_list) {
    int side = board.side;
    int opponent_side = (side == WHITE) ? BLACK : WHITE;
    int bishop_piece = (side == WHITE) ? WHITE_BISHOP : BLACK_BISHOP;
    U64 bishops = board.bitboards[bishop_piece];

    // Loop through each bishop
    while (bishops) {
        int bishop_square = bitscanForward(bishops);
        clear_bit(bishops, bishop_square);

        // Direction offsets for the bishop 
        const int directions[4] = {NORTH_EAST, NORTH_WEST, SOUTH_EAST, SOUTH_WEST}; 

        // Generate moves in all four directions
        for (int dir = 0; dir < 4; ++dir) {
            int direction_offset = directions[dir];
            generateSlidingMovesInDirection(
                board,
                move_list,
                bishop_square,
                direction_offset,
                side,
                opponent_side,
                bishop_piece
            );
        }
    }
}

void MoveGenerator::generateRookMoves(const Board& board, std::vector<Move>& move_list) {
    int side = board.side;
    int opponent_side = (side == WHITE) ? BLACK : WHITE;
    int rook_piece = (side == WHITE) ? WHITE_ROOK : BLACK_ROOK;
    U64 rooks = board.bitboards[rook_piece];

    // Loop through each rook
    while (rooks) {
        int rook_square = bitscanForward(rooks);
        clear_bit(rooks, rook_square);
        // Direction offsets for the rook 
        const int directions[4] = {NORTH, SOUTH, EAST, WEST}; 
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

void MoveGenerator::generateQueenMoves(const Board& board, std::vector<Move>& move_list){
    int side = board.side;
    int opponent_side = (side == WHITE) ? BLACK : WHITE;
    int queen_piece = (side == WHITE) ? WHITE_QUEEN : BLACK_QUEEN;
    U64 queens = board.bitboards[queen_piece];

    // Loop through each queen
    while(queens){
        // Direction offsets for the queen
        const int directions[8] = {NORTH, SOUTH, EAST, WEST, NORTH_EAST, NORTH_WEST, SOUTH_EAST, SOUTH_WEST}; 
        int queen_square = bitscanForward(queens);
        clear_bit(queens, queen_square);
        for (int dir = 0; dir < 8; ++dir) {
            int direction_offset = directions[dir];
            generateSlidingMovesInDirection(
                board,
                move_list,
                queen_square,
                direction_offset,
                side,
                opponent_side,
                queen_piece
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
    
    while(king){
        const int directions[8] = {NORTH, SOUTH, EAST, WEST, NORTH_EAST, NORTH_WEST, SOUTH_EAST, SOUTH_WEST}; 
            // Generate moves in all directions
        for (int dir = 0; dir < 8; ++dir) {
            int direction_offset = directions[dir];
            int to_square = king_square + direction_offset;
            if (to_square < 0 || to_square >= 64 || isBoundaryCrossed(king_square, to_square, direction_offset)) {
                //std::cout << "Boundary crossed at " << squareToAlgebraic(to_square) << "\n";
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
        // Generate castling moves
        if (!isKingInCheck(board, side)) {
            generateCastlingMoves(board, move_list, king_square, side);
        }        
        clear_bit(king, king_square);
    }
}

bool MoveGenerator::isKingInCheck(const Board& board, int side) {
    int king_square = bitscanForward(board.bitboards[(side == WHITE) ? WHITE_KING : BLACK_KING]);

    // Generate attack bitboards for opponent
    int opponent_side = (side == WHITE) ? BLACK : WHITE;

    // Check for attacks from pawns, knights, bishops, rooks, queens, and the opponent's king
    if (isSquareAttackedByPawn(board, king_square, opponent_side)) return true;
    if (isSquareAttackedByKnight(board, king_square, opponent_side)) return true;
    if (isSquareAttackedByBishop(board, king_square, opponent_side)) return true;
    if (isSquareAttackedByRook(board, king_square, opponent_side)) return true;
    if (isSquareAttackedByQueen(board, king_square, opponent_side)) return true;
    if (isSquareAttackedByKing(board, king_square, opponent_side)) return true;

    // No attacks on the king
    return false;
}


void MoveGenerator::generateCastlingMoves(const Board& board, std::vector<Move>& move_list, int king_square, int side) {

    // King-side castling
    if (canCastleKingSide(board, side)) {
        if (isSafeToCastle(board, king_square, side, "king")) {
            int to_square = (side == WHITE) ? G1 : G8;
            move_list.emplace_back(king_square, to_square, (side == WHITE) ? WHITE_KING : BLACK_KING, NO_PIECE, NO_PIECE, FLAG_CASTLING);
        }
    }

    // Queen-side castling
    if (canCastleQueenSide(board, side)) {
        if (isSafeToCastle(board, king_square, side, "queen")) {
            int to_square = (side == WHITE) ? C1 : C8;
            move_list.emplace_back(king_square, to_square, (side == WHITE) ? WHITE_KING : BLACK_KING, NO_PIECE, NO_PIECE, FLAG_CASTLING);
        }
    }
}


bool MoveGenerator::canCastleKingSide(const Board& board, int side) {
    if (side == WHITE) {
        // Check castling rights
        if (!(board.castling_rights & CASTLE_WHITE_KING_SIDE)) return false;
        // Check for empty squares between king and rook
        if (get_bit(board.occupancies[BOTH], F1) || get_bit(board.occupancies[BOTH], G1)) return false;
        // **Check if rook is on H1**
        if (!get_bit(board.bitboards[WHITE_ROOK], H1)) return false;
        return true;
    } else {
        if (!(board.castling_rights & CASTLE_BLACK_KING_SIDE)) return false;
        if (get_bit(board.occupancies[BOTH], F8) || get_bit(board.occupancies[BOTH], G8)) return false;
        // **Check if rook is on H8**
        if (!get_bit(board.bitboards[BLACK_ROOK], H8)) return false;
        return true;
    }
}

bool MoveGenerator::canCastleQueenSide(const Board& board, int side) {
    if (side == WHITE) {
        if (!(board.castling_rights & CASTLE_WHITE_QUEEN_SIDE)) return false;
        if (get_bit(board.occupancies[BOTH], B1) || get_bit(board.occupancies[BOTH], C1) || get_bit(board.occupancies[BOTH], D1)) return false;
        // **Check if rook is on A1**
        if (!get_bit(board.bitboards[WHITE_ROOK], A1)) return false;
        return true;
    } else {
        if (!(board.castling_rights & CASTLE_BLACK_QUEEN_SIDE)) return false;
        if (get_bit(board.occupancies[BOTH], B8) || get_bit(board.occupancies[BOTH], C8) || get_bit(board.occupancies[BOTH], D8)) return false;
        // **Check if rook is on A8**
        if (!get_bit(board.bitboards[BLACK_ROOK], A8)) return false;
        return true;
    }
}


// Check if any of the squares between the king and the rook are attacked
bool MoveGenerator::isSafeToCastle(const Board& board, int king_square, int side, const std::string& castling_type) {
    int intermediate_square1, intermediate_square2;
    
    if (castling_type == "king") {
        // King-side castling
        intermediate_square1 = king_square + 1;  // F1 or F8
        intermediate_square2 = king_square + 2;  // G1 or G8
    } else {
        // Queen-side castling
        intermediate_square1 = king_square - 1;  // D1 or D8
        intermediate_square2 = king_square - 2;  // C1 or C8
    }

    // Create a temporary board for testing each square without making permanent changes
    Board temp_board = board;
    
    // Check if moving to intermediate_square1 or intermediate_square2 would put the king in check
    for (int test_square : {king_square, intermediate_square1, intermediate_square2}) {
        clear_bit(temp_board.bitboards[(side == WHITE) ? WHITE_KING : BLACK_KING], king_square);
        set_bit(temp_board.bitboards[(side == WHITE) ? WHITE_KING : BLACK_KING], test_square);
        temp_board.updateOccupancies();
        
        if (isKingInCheck(temp_board, side)) {
            return false;
        }
        
        // Restore the original board position after each test
        temp_board = board;
    }

    return true;
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
            // std::cout << "Boundary crossed at " << squareToAlgebraic(to_square) << "\n";
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
            // std::cout << "Captured opponent piece at " << squareToAlgebraic(to_square) << "\n";
            break;
        } else {
            // O problema dos bispos não é aqui
            move_list.emplace_back(start_square, to_square, piece_type, NO_PIECE, NO_PIECE, flags);
        }

        to_square += direction_offset;
    }
}


bool MoveGenerator::isSquareAttackedByPawn(const Board& board, int square, int opponent_side) {
    int pawn_piece = (opponent_side == WHITE) ? WHITE_PAWN : BLACK_PAWN;
    U64 pawns = board.bitboards[pawn_piece];
    U64 pieces = pawns;

    int attack_offsets[2];
    if (opponent_side == WHITE) {
        attack_offsets[0] = NORTH_EAST; // +9
        attack_offsets[1] = NORTH_WEST; // +7
    } else {
        attack_offsets[0] = SOUTH_EAST; // -7
        attack_offsets[1] = SOUTH_WEST; // -9
    }

    while (pieces) {
        int pawn_square = bitscanForward(pieces);
        clear_bit(pieces, pawn_square);

        for (int i = 0; i < 2; ++i) {
            int attack_square = pawn_square + attack_offsets[i];

            // Ensure the attack square is on the board
            if (attack_square < 0 || attack_square >= 64) {
                continue;
            }

            int from_file = pawn_square % 8;
            int to_file = attack_square % 8;

            // Ensure no wrap-around
            if (abs(from_file - to_file) == 1) {
                if (attack_square == square) {
                    return true; // Target square is attacked by a pawn
                }
            }
        }
    }

    // No attacks found
    return false;
}



bool MoveGenerator::isSquareAttackedByKnight(const Board& board, int square, int opponent_side) {
    // Get the bitboard of opponent's knights
    U64 knights = board.bitboards[(opponent_side == WHITE) ? WHITE_KNIGHT : BLACK_KNIGHT];
    U64 pieces = knights;

    // Loop through each knight
    while (pieces) {
        int knight_square = bitscanForward(pieces);
        clear_bit(pieces, knight_square);

        // Knight move offsets
        const int knight_offsets[8] = {17, 15, 10, 6, -6, -10, -15, -17};

        // Generate moves for the knight
        for (int i = 0; i < 8; ++i) {
            int to_square = knight_square + knight_offsets[i];

            // Check if the destination square is within the board bounds
            if (to_square >= 0 && to_square < 64) {
                // Check for boundary crossing to prevent wrap-around
                if (!isKnightMoveBoundaryCrossed(knight_square, to_square)) {
                    // Check if the knight attacks the target square
                    if (to_square == square) {
                        return true; // Target square is attacked by a knight
                    }
                }
            }
        }
    }

    // No attacks found
    return false;
}


bool MoveGenerator::isSquareAttackedByBishop(const Board& board, int square, int opponent_side) {
    U64 bishops = board.bitboards[(opponent_side == WHITE) ? WHITE_BISHOP : BLACK_BISHOP];
    U64 pieces = bishops;

    while (pieces) {
        int piece_square = bitscanForward(pieces);
        clear_bit(pieces, piece_square);

        // Generate the bishop's attacks in the diagonal
        const int directions[4] = {NORTH_EAST, NORTH_WEST, SOUTH_EAST, SOUTH_WEST}; 
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

bool MoveGenerator::isSquareAttackedByQueen(const Board& board, int square, int opponent_side) {
    U64 queens = board.bitboards[(opponent_side == WHITE) ? WHITE_QUEEN : BLACK_QUEEN];
    U64 pieces = queens;

    while (pieces) {
        int piece_square = bitscanForward(pieces);
        clear_bit(pieces, piece_square);

        // Generate rook attacks inline
        const int directions[8] = {NORTH, SOUTH, EAST, WEST, NORTH_EAST, NORTH_WEST, SOUTH_EAST, SOUTH_WEST}; 
        for (int dir = 0; dir < 8; ++dir) {
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

bool MoveGenerator::isKnightMoveBoundaryCrossed(int from_square, int to_square) {
    int from_file = from_square % 8;
    int to_file = to_square % 8;
    int file_diff = std::abs(from_file - to_file);
    int rank_diff = std::abs((from_square / 8) - (to_square / 8));

    // A valid knight move should have file and rank differences of (1, 2) or (2, 1)
    if ((file_diff == 1 && rank_diff == 2) || (file_diff == 2 && rank_diff == 1)) {
        return false;
    }

    // If not, the move crosses the boundary
    return true;
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
