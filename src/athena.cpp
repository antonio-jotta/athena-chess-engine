#include "board.h"
#include "move_generator.h"
#include "move.h"
#include "search.h"
#include <iostream>
#include <string>
#include <chrono>

Move fromUCI(const std::string& moveStr, const Board& board);
std::string toUCI(const Move& move);
bool isGameOver(Board& board, MoveGenerator moveGenerator, std::vector<Move> move_list);

int main() {
    Board board;
    board.resetBoard();
    //board.loadFEN("3b4/8/7p/5K1k/8/6p1/6P1/2R5 w - - 0 1");
    board.printBoard();

    int depth = 6; // Set the search depth

    // Choose sides
    char humanSideInput;
    Side humanSide;
    std::cout << "Choose your side (w for White, b for Black): ";
    std::cin >> humanSideInput;

    if (humanSideInput == 'w' || humanSideInput == 'W') {
        humanSide = WHITE;
    } else if (humanSideInput == 'b' || humanSideInput == 'B') {
        humanSide = BLACK;
    }else if (humanSideInput == 'q' || humanSideInput == 'Q'){
        return 0;
    } else {
        std::cout << "Invalid input. Defaulting to White for human.\n";
        humanSide = WHITE;
    }

    while (true) {
        // Generate every move available for the current position
        MoveGenerator moveGenerator;
        std::vector<Move> move_list;
        moveGenerator.generateAllLegalMoves(board, move_list);

        // Check for game over conditions
        if (isGameOver(board, moveGenerator, move_list)) {
            std::cout << "Game over!\n";
            break;
        }

        if (board.side == humanSide) {
            // Human move
            std::string userMoveStr;
            std::cout << "Enter your move (e.g., e2e4 or 'q' to quit): ";
            std::cin >> userMoveStr;

            if (userMoveStr == "q" || userMoveStr == "Q") {
                std::cout << "You quit the game.\n";
                break;
            }

            Move userMove = fromUCI(userMoveStr, board);

            // Check if the user's move is in the list of legal moves
            bool isValidMove = false;
            for (const Move& move : move_list) {
                if (toUCI(move) == userMoveStr) {
                    isValidMove = true;
                    userMove = move; // Use the full move object
                    break;
                }
            }

            if (!isValidMove) {
                std::cout << "Invalid move. Try again.\n";
                continue;
            }

            board.makeMove(userMove);
            board.printBoard();
        } else {
            // Engine move
            auto start = std::chrono::high_resolution_clock::now();
            Move engineMove = Search::findBestMove(board, depth);
            auto stop = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(stop - start);

            std::cout << "Engine plays: " << toUCI(engineMove) << "\n";
            std::cout << "Search Duration: " << duration.count() << "s\n";

            board.makeMove(engineMove);
            board.printBoard();
        }
    }

    return 0;
}

bool isGameOver(Board& board, MoveGenerator moveGenerator, std::vector<Move> move_list) {

    if (move_list.empty()) {
        if (moveGenerator.isKingInCheck(board, board.side)) {
            std::cout << (board.side == WHITE ? "White" : "Black") << " is checkmated!\n";
        } else {
            std::cout << "Stalemate!\n";
        }
        return true;
    }
    return false;
}

Move fromUCI(const std::string& moveStr, const Board& board) {
    if (moveStr.length() < 4) return Move(-1, -1, NO_PIECE, NO_PIECE, NO_PIECE, 0); // Invalid move

    int fromSquare = algebraicToSquare(moveStr.substr(0, 2));
    int toSquare = algebraicToSquare(moveStr.substr(2, 2));

    // Handle promotion if applicable (e.g., "e7e8q")
    int promotedPiece = NO_PIECE;
    if (moveStr.length() == 5) {
        char promo = moveStr[4];
        promotedPiece = (board.side == WHITE)
                            ? (promo == 'q' ? WHITE_QUEEN : promo == 'r' ? WHITE_ROOK
                                           : promo == 'b' ? WHITE_BISHOP
                                                          : WHITE_KNIGHT)
                            : (promo == 'q' ? BLACK_QUEEN : promo == 'r' ? BLACK_ROOK
                                           : promo == 'b' ? BLACK_BISHOP
                                                          : BLACK_KNIGHT);
    }

    // Determine the moving piece
    int movingPiece = NO_PIECE;
    for (int piece = WHITE_PAWN; piece <= BLACK_KING; ++piece) {
        if (get_bit(board.bitboards[piece], fromSquare)) {
            movingPiece = piece;
            break;
        }
    }

    // Determine the captured piece, if any
    int capturedPiece = NO_PIECE;
    for (int piece = WHITE_PAWN; piece <= BLACK_KING; ++piece) {
        if (get_bit(board.bitboards[piece], toSquare)) {
            capturedPiece = piece;
            break;
        }
    }

    // Determine flags
    uint8_t flags = 0;
    if (capturedPiece != NO_PIECE) flags |= FLAG_CAPTURE;
    if (promotedPiece != NO_PIECE) flags |= FLAG_PROMOTION;

    return Move(fromSquare, toSquare, movingPiece, capturedPiece, promotedPiece, flags);
}

std::string toUCI(const Move& move) {
    std::string uci = squareToAlgebraic(move.from_square) + squareToAlgebraic(move.to_square);
    if (move.promoted_piece != NO_PIECE) {
        char promo = (move.promoted_piece == WHITE_QUEEN || move.promoted_piece == BLACK_QUEEN)
                         ? 'q'
                         : (move.promoted_piece == WHITE_ROOK || move.promoted_piece == BLACK_ROOK)
                               ? 'r'
                               : (move.promoted_piece == WHITE_BISHOP || move.promoted_piece == BLACK_BISHOP)
                                     ? 'b'
                                     : 'n';
        uci += promo;
    }
    return uci;
}
