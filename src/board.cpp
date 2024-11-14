#include "board.h"

///////////////////////
/// Board functions ///
///////////////////////

Board::Board() {
    resetBoard();
}

void Board::makeMove(const Move& move, bool switch_side) {
    // Remove the piece from the from_square
    clear_bit(bitboards[move.piece], move.from_square);

    // If it's a capture, remove the captured piece
    if (move.flags & FLAG_CAPTURE) {
        clear_bit(bitboards[move.captured_piece], move.to_square);
    }

    // Handle promotions
    if (move.flags & FLAG_PROMOTION) {
        set_bit(bitboards[move.promoted_piece], move.to_square);
    }
    // Handle castling
    else if (move.flags & FLAG_CASTLING) {
        set_bit(bitboards[move.piece], move.to_square);

        // Move the rook as well
        if (side == WHITE) {
            if (move.to_square == G1) {
                // King-side castling
                clear_bit(bitboards[WHITE_ROOK], H1);
                set_bit(bitboards[WHITE_ROOK], F1);
            } else if (move.to_square == C1) {
                // Queen-side castling
                clear_bit(bitboards[WHITE_ROOK], A1);
                set_bit(bitboards[WHITE_ROOK], D1);
            }
        } else {
            if (move.to_square == G8) {
                // King-side castling
                clear_bit(bitboards[BLACK_ROOK], H8);
                set_bit(bitboards[BLACK_ROOK], F8);
            } else if (move.to_square == C8) {
                // Queen-side castling
                clear_bit(bitboards[BLACK_ROOK], A8);
                set_bit(bitboards[BLACK_ROOK], D8);
            }
        }
    }
    // Normal move
    else {
        set_bit(bitboards[move.piece], move.to_square);
    }

    // Update occupancies
    updateOccupancies();

    // Update castling rights
    updateCastlingRights(move);

    // Increment the move number if Black has just moved
    if (side == BLACK) {
        move_number++;
    }

    // Update side to move
    if (switch_side) {
        side = (side == WHITE) ? BLACK : WHITE;
    }
}


void Board::resetBoard() {
    // Clear all bitboards
    for (int i = 0; i < 12; i++)
        bitboards[i] = 0ULL;

    // Set up the starting position
    bitboards[WHITE_PAWN]   = 0x000000000000FF00ULL; // Pawns on rank 2
    bitboards[WHITE_KNIGHT] = 0x0000000000000042ULL; // Knights on b1 and g1
    bitboards[WHITE_BISHOP] = 0x0000000000000024ULL; // Bishops on c1 and f1
    bitboards[WHITE_ROOK]   = 0x0000000000000081ULL; // Rooks on a1 and h1
    bitboards[WHITE_QUEEN]  = 0x0000000000000010ULL; // Queen on d1
    bitboards[WHITE_KING]   = 0x0000000000000008ULL; // King on e1

    bitboards[BLACK_PAWN]   = 0x00FF000000000000ULL; // Pawns on rank 7
    bitboards[BLACK_KNIGHT] = 0x4200000000000000ULL; // Knights on b8 and g8
    bitboards[BLACK_BISHOP] = 0x2400000000000000ULL; // Bishops on c8 and f8
    bitboards[BLACK_ROOK]   = 0x8100000000000000ULL; // Rooks on a8 and h8
    bitboards[BLACK_QUEEN]  = 0x1000000000000000ULL; // Queen on d8
    bitboards[BLACK_KING]   = 0x0800000000000000ULL; // King on e8

    // Update occupancies
    updateOccupancies();

    // Set side to move
    side = WHITE;

    // No en passant square initially
    en_passant = -1;

    // All castling rights available
    castling_rights = CASTLE_WHITE_KING_SIDE | CASTLE_WHITE_QUEEN_SIDE |
                      CASTLE_BLACK_KING_SIDE | CASTLE_BLACK_QUEEN_SIDE;
}


void Board::updateOccupancies() {
    occupancies[WHITE] = 0ULL;
    occupancies[BLACK] = 0ULL;

    // Sum up white pieces
    for (int piece = WHITE_PAWN; piece <= WHITE_KING; piece++)
        occupancies[WHITE] |= bitboards[piece];

    // Sum up black pieces
    for (int piece = BLACK_PAWN; piece <= BLACK_KING; piece++)
        occupancies[BLACK] |= bitboards[piece];

    // All pieces
    occupancies[BOTH] = occupancies[WHITE] | occupancies[BLACK];
}

void Board::updateCastlingRights(const Move& move) {
    // If king or rook moves, or rook is captured, update castling rights
    if (move.piece == WHITE_KING) {
        castling_rights &= ~(CASTLE_WHITE_KING_SIDE | CASTLE_WHITE_QUEEN_SIDE);
    }
    if (move.piece == BLACK_KING) {
        castling_rights &= ~(CASTLE_BLACK_KING_SIDE | CASTLE_BLACK_QUEEN_SIDE);
    }
    if (move.piece == WHITE_ROOK) {
        if (move.from_square == H1) castling_rights &= ~CASTLE_WHITE_KING_SIDE;
        if (move.from_square == A1) castling_rights &= ~CASTLE_WHITE_QUEEN_SIDE;
    }
    if (move.piece == BLACK_ROOK) {
        if (move.from_square == H8) castling_rights &= ~CASTLE_BLACK_KING_SIDE;
        if (move.from_square == A8) castling_rights &= ~CASTLE_BLACK_QUEEN_SIDE;
    }
    if (move.flags & FLAG_CAPTURE) {
        if (move.captured_piece == WHITE_ROOK) {
            if (move.to_square == H1) castling_rights &= ~CASTLE_WHITE_KING_SIDE;
            if (move.to_square == A1) castling_rights &= ~CASTLE_WHITE_QUEEN_SIDE;
        }
        if (move.captured_piece == BLACK_ROOK) {
            if (move.to_square == H8) castling_rights &= ~CASTLE_BLACK_KING_SIDE;
            if (move.to_square == A8) castling_rights &= ~CASTLE_BLACK_QUEEN_SIDE;
        }
    }
}


void Board::printBoard() {
    for (int rank = 7; rank >= 0; rank--) {
        std::cout << rank + 1 << "  ";
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;
            char pieceChar = '.';

            for (int piece = WHITE_PAWN; piece <= BLACK_KING; piece++) {
                if (get_bit(bitboards[piece], square)) {
                    switch (piece) {
                        case WHITE_PAWN:   pieceChar = 'P'; break;
                        case WHITE_KNIGHT: pieceChar = 'N'; break;
                        case WHITE_BISHOP: pieceChar = 'B'; break;
                        case WHITE_ROOK:   pieceChar = 'R'; break;
                        case WHITE_QUEEN:  pieceChar = 'Q'; break;
                        case WHITE_KING:   pieceChar = 'K'; break;
                        case BLACK_PAWN:   pieceChar = 'p'; break;
                        case BLACK_KNIGHT: pieceChar = 'n'; break;
                        case BLACK_BISHOP: pieceChar = 'b'; break;
                        case BLACK_ROOK:   pieceChar = 'r'; break;
                        case BLACK_QUEEN:  pieceChar = 'q'; break;
                        case BLACK_KING:   pieceChar = 'k'; break;
                    }
                    break;
                }
            }
            std::cout << pieceChar << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "\n   A B C D E F G H\n";
    std::cout << "Side to move: " << (side == WHITE ? "White\n" : "Black\n");
    std::cout << "En Passant: " << (en_passant != -1 ? std::to_string(en_passant) : "None") << "\n";
    std::cout << "Castling: " << getCastlingRightsString() << "\n";}


////////////////////////////////////////////////////
// Helper functions for dealing with FEN notation //
////////////////////////////////////////////////////

void Board::loadFEN(const std::string& fen) {
    // Clear the board
    for (int i = 0; i < 12; ++i) {
        bitboards[i] = 0ULL;
    }
    occupancies[WHITE] = occupancies[BLACK] = occupancies[BOTH] = 0ULL;

    std::istringstream fenStream(fen);
    std::string boardPart, activeColor, castling, enPassant, halfmoveClock, fullmoveNumber;

    // Read each part of the FEN string
    fenStream >> boardPart >> activeColor >> castling >> enPassant >> halfmoveClock >> fullmoveNumber;

    // 1. Piece placement
    int rank = 7;
    int file = 0;

    for (size_t i = 0; i < boardPart.length(); ++i) {
        char c = boardPart[i];

        if (c == '/') {
            rank--;
            file = 0;
        } else if (isdigit(c)) {
            file += c - '0'; // Skip empty squares
        } else {
            Piece piece = NO_PIECE;
            switch (c) {
                case 'P': piece = WHITE_PAWN; break;
                case 'N': piece = WHITE_KNIGHT; break;
                case 'B': piece = WHITE_BISHOP; break;
                case 'R': piece = WHITE_ROOK; break;
                case 'Q': piece = WHITE_QUEEN; break;
                case 'K': piece = WHITE_KING; break;
                case 'p': piece = BLACK_PAWN; break;
                case 'n': piece = BLACK_KNIGHT; break;
                case 'b': piece = BLACK_BISHOP; break;
                case 'r': piece = BLACK_ROOK; break;
                case 'q': piece = BLACK_QUEEN; break;
                case 'k': piece = BLACK_KING; break;
                default: break;
            }
            if (piece != NO_PIECE) {
                int square = rank * 8 + file;
                set_bit(bitboards[piece], square);
                file++;
            }
        }
    }

    // 2. Active color
    side = (activeColor == "w") ? WHITE : BLACK;

    // 3. Castling availability
    castling_rights = 0;
    if (castling.find('K') != std::string::npos) castling_rights |= CASTLE_WHITE_KING_SIDE;
    if (castling.find('Q') != std::string::npos) castling_rights |= CASTLE_WHITE_QUEEN_SIDE;
    if (castling.find('k') != std::string::npos) castling_rights |= CASTLE_BLACK_KING_SIDE;
    if (castling.find('q') != std::string::npos) castling_rights |= CASTLE_BLACK_QUEEN_SIDE;

    // 4. En passant target square
    if (enPassant != "-") {
        en_passant = algebraicToSquare(enPassant);
    } else {
        en_passant = NO_SQUARE;
    }

    // 5. Halfmove clock
    halfmove_clock = std::stoi(halfmoveClock);

    // 6. Fullmove number
    move_number = std::stoi(fullmoveNumber);

    // Update occupancies
    updateOccupancies();
}

std::string Board::generateFEN() const {
    std::string fen;

    // 1. Piece placement
    for (int rank = 7; rank >= 0; --rank) {
        int emptySquares = 0;
        for (int file = 0; file < 8; ++file) {
            int square = rank * 8 + file;
            bool pieceFound = false;

            for (int pieceType = WHITE_PAWN; pieceType <= BLACK_KING; ++pieceType) {
                if (get_bit(bitboards[pieceType], square)) {
                    if (emptySquares > 0) {
                        fen += std::to_string(emptySquares);
                        emptySquares = 0;
                    }
                    fen += pieceToChar(static_cast<Piece>(pieceType));
                    pieceFound = true;
                    break;
                }
            }
            if (!pieceFound) {
                emptySquares++;
            }
        }
        if (emptySquares > 0) {
            fen += std::to_string(emptySquares);
        }
        if (rank > 0) {
            fen += '/';
        }
    }

    // 2. Active color
    fen += ' ';
    fen += (side == WHITE) ? 'w' : 'b';

    // 3. Castling availability
    fen += ' ';
    std::string castling = "";
    if (castling_rights & CASTLE_WHITE_KING_SIDE)  castling += 'K';
    if (castling_rights & CASTLE_WHITE_QUEEN_SIDE) castling += 'Q';
    if (castling_rights & CASTLE_BLACK_KING_SIDE)  castling += 'k';
    if (castling_rights & CASTLE_BLACK_QUEEN_SIDE) castling += 'q';
    fen += (castling.empty()) ? "-" : castling;

    // 4. En passant target square
    fen += ' ';
    if (en_passant != NO_SQUARE) {
        fen += squareToAlgebraic(en_passant);
    } else {
        fen += '-';
    }

    // 5. Halfmove clock
    fen += ' ';
    fen += std::to_string(halfmove_clock);

    // 6. Fullmove number
    fen += ' ';
    fen += std::to_string(move_number);

    return fen;
}

//////////////////////////////////////
// Bit manipulations for the pieces //
//////////////////////////////////////


// Find the index of the least significant 1 bit in b (returns 0-based index)
// Used for iteration in the bitboard
int bitscanForward(U64 b) {
    // Use GCC/Clang built-in intrinsic if available
    #if defined(__GNUC__) || defined(__clang__)
        return __builtin_ctzll(b);
    #else
        // Fallback method if no intrinsics are available
        int index = 0;
        while ((b & 1) == 0) {
            b >>= 1;
            index++;
        }
        return index;
    #endif
}

// Count the number of 1 bits in b (population count)
int countBits(U64 b) {
    // Use GCC/Clang built-in intrinsic if available
    #if defined(__GNUC__) || defined(__clang__)
        return __builtin_popcountll(b);
    #else
        // Fallback method if no intrinsics are available
        int count = 0;
        while (b) {
            count += b & 1;
            b >>= 1;
        }
        return count;
    #endif
}


/////////////////////////////////
// Helper functions for prints //
/////////////////////////////////


std::string squareToAlgebraic(int square) {
    // Ensure square is within bounds
    if (square < 0 || square > 63) {
        return "Invalid";  // Return an error string if out of bounds
    }
    
    char file = 'a' + (square % 8);       // File from 'a' to 'h'
    char rank = '1' + (square / 8);       // Rank from '1' to '8'
    
    return std::string(1, file) + std::string(1, rank);
}

// In board.h or a utility header
int algebraicToSquare(const std::string& algebraic) {
    int file = algebraic[0] - 'a';
    int rank = algebraic[1] - '1';
    return rank * 8 + file;
}

char pieceToChar(Piece piece){
    switch (piece) {
        case WHITE_PAWN:   return 'P';
        case WHITE_KNIGHT: return 'N';
        case WHITE_BISHOP: return 'B';
        case WHITE_ROOK:   return 'R';
        case WHITE_QUEEN:  return 'Q';
        case WHITE_KING:   return 'K';
        case BLACK_PAWN:   return 'p';
        case BLACK_KNIGHT: return 'n';
        case BLACK_BISHOP: return 'b';
        case BLACK_ROOK:   return 'r';
        case BLACK_QUEEN:  return 'q';
        case BLACK_KING:   return 'k';
        default:           return '?';
    }
}


std::string pieceToString(int piece) {
    switch (piece) {
        case WHITE_PAWN:   return "White Pawn";
        case WHITE_KNIGHT: return "White Knight";
        case WHITE_BISHOP: return "White Bishop";
        case WHITE_ROOK:   return "White Rook";
        case WHITE_QUEEN:  return "White Queen";
        case WHITE_KING:   return "White King";
        case BLACK_PAWN:   return "Black Pawn";
        case BLACK_KNIGHT: return "Black Knight";
        case BLACK_BISHOP: return "Black Bishop";
        case BLACK_ROOK:   return "Black Rook";
        case BLACK_QUEEN:  return "Black Queen";
        case BLACK_KING:   return "Black King";
        case NO_PIECE:     return "No Piece";
        default:           return "Unknown Piece";
    }
}

std::string Board::getCastlingRightsString() const {
    std::string rights;
    
    if (castling_rights & CASTLE_WHITE_KING_SIDE) rights += "K";
    if (castling_rights & CASTLE_WHITE_QUEEN_SIDE) rights += "Q";
    if (castling_rights & CASTLE_BLACK_KING_SIDE) rights += "k";
    if (castling_rights & CASTLE_BLACK_QUEEN_SIDE) rights += "q";
    
    return rights.empty() ? "None" : rights;
}


