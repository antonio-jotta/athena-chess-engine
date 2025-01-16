# Athena Chess Engine

A chess engine written in C++ capable of doing a move search of depths up to 4, in a reasonable time. The most important next steps for this project include introducing a parallelization in the move search.

## Menu
  - [**How to run the program**](#extra-tests-and-images)
  - [**Files**](#code)
  - [**Authors**](#authors)

## Running the program

Build the project with the command(make sure to have g++ installed):

```bash
`make`
```

Now, you can simply run the program by doing
```bash
`./athena`
```

### Arguments

If you want to customize the depth of the engine's search you can add the depth as an argument when running the program:
```bash
`./athena 6`
```

Beware that this depth is limited due to the current performance limitations.

## Playing versus the engine

First, start by selecting your collor, white "w", or black "b", and then enter your move.

```bash
8  r n b q k b n r 
7  p p p p p p p p 
6  . . . . . . . . 
5  . . . . . . . . 
4  . . . . . . . . 
3  . . . . . . . . 
2  P P P P P P P P 
1  R N B Q K B N R 

   A B C D E F G H
Side to move: White
En Passant: None
Castling: KQkq
Choose your side (w for White, b for Black): w
Enter your move in UCI format (e.g., e2e4 or 'q' to quit): d2d4
```

The white pieces are represented by the upper case letters, and the black pieces by the lower case letters. Here is a table containing each piece symbol and their corresponding meaning:

| Symbol | Piece          |
|--------|----------------|
| P      | White Pawn     |
| N      | White Knight   |
| B      | White Bishop   |
| R      | White Rook     |
| Q      | White Queen    |
| K      | White King     |
| p      | Black Pawn     |
| n      | Black Knight   |
| b      | Black Bishop   |
| r      | Black Rook     |
| q      | Black Queen    |
| k      | Black King     |

### Making a move
Currently, the move-making system does not use standard chess notation (like Nf3 or exd5). Instead, moves are entered by specifying the starting square of the piece followed by its destination square. Case sensitivity is not required, so you can use either uppercase or lowercase letters.

For example:

    To move a knight from b1 to c3, enter: b1c3
    To move a king from e8 to f8, enter: e8f8

This simple format makes it easy to specify moves directly without worrying about standard chess notation.

### Search information
Upon making a move, the engine will give you some information about it's search:
```bash
Time taken for the search: 0.126477 seconds
Leaf nodes evaluated: 1691
```
The leaf nodes evaluated and time for the search grow exponentially with the increase in depth. For further information on this topic, go to: 
https://www.chessprogramming.org/Alpha-Beta


## Code

- `athena.cpp` - Main game loop.

- `board.cpp`- Board class and board related functions like printing the board, three fold repetition checking, etc.

- `move_generator.cpp` - Class for generating all legal moves given a board state.


- `evaluation.cpp` - Board evaluation class containing piece tables and various scoring metrics, like pawn structure and material scoring.


- `search.cpp` - Move search class using negamax and quiescence.

- `evaluation_test.cpp` and `move_generation_test.cpp` - Test files for the various elements of each of these classes that needed robust testing.



## Authors

António Jotta 