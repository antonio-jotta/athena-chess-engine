#include "board.h"
#include "move_generator.h"
#include "move.h"
#include "search.h"
#include <iostream>
#include <string>

Move fromUCI(const std::string& moveStr, const Board& board);

std::string toUCI(const Move& move);
bool isGameOver(Board& board, MoveGenerator moveGenerator, std::vector<Move> move_list);


int main() {
    Board board;
    //board.resetBoard();
    
    // board.loadFEN("4r1k1/4r1p1/8/p2R1P1K/5P1P/1QP3q1/1P6/3R4 b - - 0 1");
    board.setInitialPosition();
    board.loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    // mate in 4
    //board.loadFEN("3qr2k/1p3rbp/2p3p1/p7/P2pBNn1/1P3n2/6P1/B1Q1RR1K b - - 1 30");

    board.printBoard();

    int depth = 5; // Set the search depth

    // Choose sides
    char humanSideInput;
    Side humanSide;
    std::cout << "Choose your side (w for White, b for Black): ";
    std::cin >> humanSideInput;

    if (humanSideInput == 'w' || humanSideInput == 'W') {
        humanSide = WHITE;
    } else if (humanSideInput == 'b' || humanSideInput == 'B') {
        humanSide = BLACK;
    } else {
        std::cout << "Invalid input. Defaulting to White for human.\n";
        humanSide = WHITE;
    }

    MoveGenerator moveGenerator;

    while (true) {
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
            std::cout << "Enter your move in UCI format (e.g., e2e4 or 'q' to quit): ";
            std::cin >> userMoveStr;

            if (userMoveStr == "q") {
                std::cout << "You quit the game.\n";
                break;
            }

            // Convert user input to a Move object
            Move userMove = fromUCI(userMoveStr, board);

            // Validate the user's move
            bool isValidMove = false;
            for (const Move& move : move_list) {
                if (move.from_square == userMove.from_square && move.to_square == userMove.to_square) {
                    isValidMove = true;
                    userMove = move; // Ensure all move details are accurate
                    break;
                }
            }

            if (!isValidMove) {
                std::cout << "Invalid move. Try again.\n";
                continue;
            }

            // Make the move on the board
            board.makeMove(userMove);
            board.printBoard();

            // Check for draw conditions after the move
            if (board.isThreefoldRepetition()) {
                std::cout << "Draw by threefold repetition!\n";
                break;
            }

            if (board.isFiftyMoveRule()) {
                std::cout << "Draw by the 50-move rule!\n";
                break;
            }
        } else {
            // Engine move
            std::cout << "Engine is thinking...\n";
            Move engineMove = Search::findBestMove(board, depth);

            if (engineMove.from_square == -1 || engineMove.to_square == -1) {
                std::cout << "Engine has no legal moves. Game over!\n";
                break;
            }

            std::cout << "Engine plays: " << toUCI(engineMove) << "\n";
            board.makeMove(engineMove);
            board.printBoard();

            // Check for draw conditions after the move
            if (board.isThreefoldRepetition()) {
                std::cout << "Draw by threefold repetition!\n";
                break;
            }

            if (board.isFiftyMoveRule()) {
                std::cout << "Draw by the 50-move rule!\n";
                break;
            }
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
