#ifndef MOVES_H
#define MOVES_H

#include "board.h"
#include <stdbool.h>
#include <stdio.h>
enum file {
  FILELESS = 0,
  A = 'A',
  B = 'B',
  C = 'C',
  D = 'D',
  E = 'E',
  F = 'F',
  G = 'G',
  H = 'H',
};
#define CHESS_ERROR 0
#define CHESS_NORMAL 1
#define TAKES 2
#define PROMOTION 4
#define ENPASSANT 8
#define CHECK 16
#define CHECKMATE 32
#define SHORT_CASTLE 64
#define LONG_CASTLE 128
#define VERTICAL_CASTLE 256

#define CHESS_EOF ~0U
struct move {
  // Piece is the piece that is being moved.
  enum piece piece;
  // File is the letter, or vertical groupings on the board.
  enum file file;
  // Rank defines the horizontal groupings on the board
  int rank;
  // Disambiguating file.
  enum file disFile;
  // Disambiguating rank.
  int disRank;
  // Special cases for a particular move
  unsigned flags;
};

#define DEFAULT_PIECE (struct move) { .piece = 0, .file = 0, .rank = 0, .disFile = 0,\
            .disRank = 0, .flags = CHESS_ERROR }

struct moveTuple {
    struct move m1;
    struct move m2;
};
struct move pullMove(FILE *file);


struct moveTuple pullMoves(FILE *file1, FILE *file2);

// Imports and interprets list
struct linkedList *getList(FILE *file1, FILE *file2);
// prepares regex's for checking the character stream
// for moves
bool initMoves(void);
// determines if the move is allowed to play
bool legalMove(const struct move *move, const struct board *board, enum player player);
// prints move for debugging purposes
void printMove(const struct move *move, enum player player);
// puts the move in a string for debugging
char *moveToString(const struct move *move, enum player player);
// frees all the moves
void quitMoves(void);

bool isCascle(unsigned flags);

#endif
