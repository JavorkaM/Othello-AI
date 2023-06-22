# Othello Bot

This project implements a basic player for the game of Othello (Reversi) using the Minimax algorithm with alpha-beta pruning. The bot is written in C++.

## Othello Game Rules

Othello is a two-player board game played on an 8x8 grid. Players take turns placing their discs on the board, with the objective of flipping the opponent's discs to their own color. The game ends when there are no more valid moves or the board is filled. The player with the most discs of their color wins.

## Code Structure

The code is structured into functions and data structures to implement the Othello AI player.

## Usage

1. Compile the code using a C++ compiler.
2. Run the compiled executable.
3. Send commands to the program to start the game, make moves, or stop the program.

### Commands

- `START <color> <time>`: Starts the game with the player's color (`W` for white, `B` for black), and maximum time per move.
- `MOVE <board>`: Makes a move on the board and receives the AI player's move in response.
- `STOP`: Stops the program.

**Note:** `<board>` represents the current board state as a string. `-` for empty cells, `X` for black discs, `O` for white discs.

## Limitations

- The implementation has a fixed depth of 4 for the Minimax algorithm.
- The code assumes a timeout value for each move but lacks a timeout mechanism.
