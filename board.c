#include "board.h"
#include "moves.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Initilizing the board
struct board *makeBoard(void) {
  struct board *newBoard = malloc(sizeof(struct board));
  if (newBoard == NULL) {
    return NULL;
  }
  memset(newBoard, '-', sizeof(*newBoard));
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      // this if is for the backranks of both sides
      if (i == 0 || i == 7) {
        if (j == 0 || j == 7) {
          newBoard->tiles[i][j].piece = ROOK;
        } else if (j == 1 || j == 6) {
          newBoard->tiles[i][j].piece = KNIGHT;
        } else if (j == 2 || j == 5) {
          newBoard->tiles[i][j].piece = BISHOP;
        } else if (j == 3) {
          newBoard->tiles[i][j].piece = QUEEN;
        } else {
          newBoard->tiles[i][j].piece = KING;
        }

      }
      // this if is for the pawn ranks of both sides
      else if (i == 1 || i == 6) {
        newBoard->tiles[i][j].piece = PAWN;
      } else {
        newBoard->tiles[i][j].piece = EMPTY;
      }
      if (i > 5) {
        newBoard->tiles[i][j].player = BLACK;
      } else if (i < 2) {
        newBoard->tiles[i][j].player = WHITE;
      } else {
        newBoard->tiles[i][j].player = NONE;
      }
    }
  }
  return newBoard;
}
// Displays the board
void printBoard(const struct board *board) {
  if (board == NULL) {
    return;
  }
  printf("    A B C D E F G H\n");
  for (int i = 0; i < 8; i++) {
    printf("%d [ ", i + 1);
    for (int j = 0; j < 8; j++) {
      if (board->tiles[i][j].player == BLACK) {
        printf("\033[30;1;1m%c \033[0m", board->tiles[i][j].piece);
      } else if (board->tiles[i][j].player == WHITE) {
        printf("\033[97;1;1m%c \033[0m", board->tiles[i][j].piece);
      } else {
        printf("\033[0m%c ", board->tiles[i][j].piece);
      }
    }
    printf("]\n");
  }
  printf("\n");
}
struct board *copyBoard(const struct board *board) {
  if (board == NULL) {
    return NULL;
  }
  struct board *newBoard = malloc(sizeof(*board));
  if (newBoard == NULL) {
    return NULL;
  }
  memcpy(newBoard, board, sizeof(*board));
  return newBoard;
}
bool rookCollision(const struct board *board, const struct move *move, enum player player) {
  bool rank = false;
  bool up = false;
  bool left = false;
  struct board *newBoard = copyBoard(board);
  if (newBoard == NULL) {
    return true;
  }
  newBoard->tiles[move->rank][move->file - 'A'].piece = move->piece;
  newBoard->tiles[move->rank][move->file - 'A'].player = player;
  int file = move->file - 'A';

  for (int i = 0; i < 8; i++) {

    if (move->rank - i >= 0) {
      if (newBoard->tiles[move->rank - i][file].piece ==
              newBoard->tiles[move->rank][file].piece &&
          newBoard->tiles[move->rank - i][file].player ==
              newBoard->tiles[move->rank][file].player) {
        rank = false;
        break;
      }
    }
    if (move->rank + i < 8) {
      if (newBoard->tiles[move->rank + i][file].piece ==
              newBoard->tiles[move->rank][file].piece &&
          newBoard->tiles[move->rank + i][file].player ==
              newBoard->tiles[move->rank][file].player) {
        up = true;
        rank = false;
        break;
      }
    }
    if ((file) + i < 8) {
      if (newBoard->tiles[move->rank][file + i].piece ==
              newBoard->tiles[move->rank][file].piece &&
          newBoard->tiles[move->rank][file + i].player ==
              newBoard->tiles[move->rank][file].player) {
        rank = true;
        break;
      }
    }
    if ((file)-i >= 0) {
      if (newBoard->tiles[move->rank][(file)-i].piece ==
              newBoard->tiles[move->rank][file].piece &&
          newBoard->tiles[move->rank][(file)-i].player ==
              newBoard->tiles[move->rank][file].player) {
        left = true;
        rank = true;
        break;
      }
    }
  }
  if (rank) {
    for (int i = 0; i < 8; i++) {
      if (left) {
        if (newBoard->tiles[move->rank][file - i].piece != EMPTY ||
            newBoard->tiles[move->rank][file - i].piece != EMPTY ||
            (newBoard->tiles[move->rank][file - i].piece != ROOK &&
             newBoard->tiles[move->rank][file - i].player != player)) {

          free(newBoard);
          return true;
        }
        if (!left) {
          if (newBoard->tiles[move->rank][file + i].piece != EMPTY ||
              newBoard->tiles[move->rank][file + i].piece != EMPTY ||
              (newBoard->tiles[move->rank][file + i].piece != ROOK &&
               newBoard->tiles[move->rank][file + i].player != player)) {

            free(newBoard);
            return true;
          }
        }
      }
    }
  }
  if (!rank) {
    for (int i = 0; i < 8; i++) {
      if (up) {
        if (newBoard->tiles[move->rank + i][file].piece != EMPTY ||
            newBoard->tiles[move->rank + i][file].piece != EMPTY ||
            (newBoard->tiles[move->rank + i][file].piece != ROOK &&
             newBoard->tiles[move->rank + i][file].player != player)) {
          free(newBoard);
          return true;
        }
      } else {
        if (newBoard->tiles[move->rank - i][file].piece != EMPTY ||
            newBoard->tiles[move->rank - i][file].piece != EMPTY ||
            (newBoard->tiles[move->rank - i][file].piece != ROOK &&
             newBoard->tiles[move->rank - i][file].player != player)) {
          free(newBoard);
          return true;
        }
      }
    }
  }
  free(newBoard);
  return false;
}

struct board *updateBoard(const struct board *board, const struct move *move, enum player player) {
  struct board *newBoard = makeBoard();
  if (newBoard == NULL || board == NULL) {
    return NULL;
  }
  *newBoard = *board;
  // appending the piece that moved to an empty space where it came from
  switch (move->piece) {
  case PAWN: {
    if (legalMove(move, board)) {
      // the new board has the new move added to the board, piece and side
      newBoard->tiles[move->rank][move->file - 'A'].player = player;
      newBoard->tiles[move->rank][move->file - 'A'].piece = move->piece;
      int mFile = (int)move->file - 'A';
      // checking for first move double space move
      if (player == WHITE) {
        if (newBoard->tiles[move->rank - 2][mFile].piece ==
                newBoard->tiles[move->rank][mFile].piece &&
            newBoard->tiles[move->rank - 2][mFile].player ==
                newBoard->tiles[move->rank][mFile].player) {
          newBoard->tiles[move->rank - 2][mFile].piece = EMPTY;
          newBoard->tiles[move->rank - 2][mFile].player = NONE;
          break;
        } else if (newBoard->tiles[move->rank - 1][mFile].piece ==
                       newBoard->tiles[move->rank][mFile].piece &&
                   newBoard->tiles[move->rank - 1][mFile].player ==
                       newBoard->tiles[move->rank][mFile].player) {
          newBoard->tiles[move->rank - 1][mFile].piece = EMPTY;
          newBoard->tiles[move->rank - 1][mFile].player = NONE;
          break;
        }
      }
      if (player == BLACK) {
        if (newBoard->tiles[move->rank + 2][mFile].piece ==
                newBoard->tiles[move->rank][mFile].piece &&
            newBoard->tiles[move->rank + 2][mFile].player ==
                newBoard->tiles[move->rank][mFile].player) {
          newBoard->tiles[move->rank + 2][mFile].piece = EMPTY;
          newBoard->tiles[move->rank + 2][mFile].player = NONE;
          break;
        }
        if (newBoard->tiles[move->rank + 1][mFile].piece ==
                newBoard->tiles[move->rank][mFile].piece &&
            newBoard->tiles[move->rank + 1][mFile].player ==
                newBoard->tiles[move->rank][mFile].player) {
          newBoard->tiles[move->rank + 1][mFile].piece = EMPTY;
          newBoard->tiles[move->rank + 1][mFile].player = NONE;
          break;
        }
      }
    } else {
      printf("Move %s not found- updateBoard\n", moveToString(move));
      return newBoard;
    }
  } break;
  case KNIGHT:
    if (legalMove(move, board)) {
      // the new board has the new move added to the board, piece and side
      newBoard->tiles[move->rank][move->file - 'A'].player = player;
      newBoard->tiles[move->rank][move->file - 'A'].piece = move->piece;
      int moveFile = (int)move->file;
      // making sure that we remove a white knight where it was moved from
      if (newBoard->tiles[move->rank][moveFile - 'A'].player == WHITE) {
        if ((move->rank - 2 >= 0 || move->rank - 2 <= 7) &&
            (moveFile - '@' >= 0 || moveFile - '@' <= 7 ||
             moveFile - 'B' >= 0 || moveFile - 'B' <= 7)) {
          if (newBoard->tiles[move->rank - 2][moveFile - 'B'].piece == KNIGHT &&
              newBoard->tiles[move->rank - 2][moveFile - 'B'].player ==
                  player) {
            // checking if the old knight is two ranks down and one file
            // right from the present position of the new move on success:
            // the spot matching the placement of the old knight is returned
            // to '-' on the board and the player to none

            newBoard->tiles[move->rank - 2][moveFile - 'B'].piece = EMPTY;
            newBoard->tiles[move->rank - 2][moveFile - 'B'].player = NONE;
            break;
          }
          if (newBoard->tiles[move->rank - 2][moveFile - '@'].piece == KNIGHT &&
              newBoard->tiles[move->rank - 2][moveFile - '@'].player ==
                  player) {
            // checking if the old knight is two ranks down and one file
            // left from the present position of the new move on success:
            // the spot matching the placement of the old knight is returned
            // to '-' on the board and the player to none

            newBoard->tiles[move->rank - 2][moveFile - '@'].piece = EMPTY;
            newBoard->tiles[move->rank - 2][moveFile - '@'].player = NONE;
            break;
          }
          if (newBoard->tiles[move->rank - 2][moveFile - '@'].piece == KNIGHT &&
              newBoard->tiles[move->rank - 2][moveFile - '@'].player ==
                  player) {
            // checking if the old knight is two ranks down and one file
            // left from the present position of the new move on success:
            // the spot matching the placement of the old knight is returned
            // to '-' on the board and the player to none

            newBoard->tiles[move->rank - 2][moveFile - '@'].piece = EMPTY;
            newBoard->tiles[move->rank - 2][moveFile - '@'].player = NONE;
            break;
          }
        }
        if ((move->rank + 2 >= 0 || move->rank + 2 <= 7) &&
            (moveFile - '@' - 1 >= 0 || moveFile - '@' - 1 <= 7 ||
             moveFile - 'B' >= 0 || moveFile - 'B' + 1 <= 7)) {
          if (newBoard->tiles[move->rank + 2][moveFile - 'B'].piece == KNIGHT &&
              newBoard->tiles[move->rank + 2][moveFile - 'B'].player ==
                  player) {
            // checking if the old knight is two ranks up and one file right
            // from the present position of the new move on success: the
            // spot matching the placement of the old knight is returned to
            // '-' on the board and the player to none

            newBoard->tiles[move->rank + 2][moveFile - 'B'].piece = EMPTY;
            newBoard->tiles[move->rank + 2][moveFile - 'B'].player = NONE;
            break;
          }
          if (newBoard->tiles[move->rank + 2][moveFile - '@'].piece == KNIGHT &&
              newBoard->tiles[move->rank + 2][moveFile - '@'].player ==
                  player) {
            // checking if the old knight is two ranks up and one file left
            // from the present position of the new move on success: the
            // spot matching the placement of the old knight is returned to
            // '-' on the board and the player to none

            newBoard->tiles[move->rank + 2][moveFile - '@'].piece = EMPTY;
            newBoard->tiles[move->rank + 2][moveFile - '@'].player = NONE;
            break;
          }
        }
        if ((move->rank + 1 >= 0 || move->rank + 1 <= 7) &&
            (moveFile - '?' - 1 >= 0 || moveFile - '?' <= 7 ||
             moveFile - 'C' >= 0 || moveFile - 'C' <= 7)) {
          if (newBoard->tiles[move->rank + 1][moveFile - 'C'].piece == KNIGHT &&
              newBoard->tiles[move->rank + 1][moveFile - 'C'].player ==
                  player) {
            // checking if the old knight is one rank up and two files right
            // from the present position of the new move on success: the
            // spot matching the placement of the old knight is returned to
            // '-' on the board and the player to none

            newBoard->tiles[move->rank + 1][moveFile - 'C'].piece = EMPTY;
            newBoard->tiles[move->rank + 1][moveFile - 'C'].player = NONE;
            break;
          }
          if (newBoard->tiles[move->rank + 1][moveFile - '?'].piece == KNIGHT &&
              newBoard->tiles[move->rank + 1][moveFile - '?'].player ==
                  player) {
            // checking if the old knight is one rank up and two files left
            // from the present position of the new move on success: the
            // spot matching the placement of the old knight is returned to
            // '-' on the board and the player to none

            newBoard->tiles[move->rank + 1][moveFile - '?'].piece = EMPTY;
            newBoard->tiles[move->rank + 1][moveFile - '?'].player = NONE;
            break;
          }
          if (newBoard->tiles[move->rank - 1][moveFile - 'C'].piece == KNIGHT &&
              newBoard->tiles[move->rank - 1][moveFile - 'C'].player ==
                  player) {
            // checking if the old knight is one rank down and two files
            // right from the present position of the new move on success:
            // the spot matching the placement of the old knight is returned
            // to '-' on the board and the player to none
            newBoard->tiles[move->rank - 1][moveFile - 'C'].piece = EMPTY;
            newBoard->tiles[move->rank - 1][moveFile - 'C'].player = NONE;
            break;
          }
          if (newBoard->tiles[move->rank - 1][moveFile - '?'].piece == KNIGHT &&
              newBoard->tiles[move->rank - 1][moveFile - '?'].player ==
                  player) {
            // checking if the old knight is one rank down and two files
            // left from the present position of the new move on success:
            // the spot matching the placement of the old knight is returned
            // to '-' on the board and the player to none
            newBoard->tiles[move->rank - 1][moveFile - '?'].piece = EMPTY;
            newBoard->tiles[move->rank - 1][moveFile - '?'].player = NONE;
            break;
          }
        }
      }
      if (newBoard->tiles[move->rank][moveFile - 'A'].player == BLACK) {
        if (newBoard->tiles[move->rank - 2][moveFile - 'B'].piece == KNIGHT &&
            newBoard->tiles[move->rank - 2][moveFile - 'B'].player ==
                player) {
          // checking if the old knight is two ranks down and one file right
          // from the present position of the new move on success: the spot
          // matching the placement of the old knight is returned to '-' on
          // the board and the player to none
          newBoard->tiles[move->rank - 2][moveFile - 'B'].piece = EMPTY;
          newBoard->tiles[move->rank - 2][moveFile - 'B'].player = NONE;
          break;
        }
        if (newBoard->tiles[move->rank - 2][moveFile - '@'].piece == KNIGHT &&
            newBoard->tiles[move->rank - 2][moveFile - '@'].player ==
                player) {
          // checking if the old knight is two ranks down and one file left
          // from the present position of the new move on success: the spot
          // matching the placement of the old knight is returned to '-' on
          // the board and the player to none
          newBoard->tiles[move->rank - 2][moveFile - '@'].piece = EMPTY;
          newBoard->tiles[move->rank - 2][moveFile - '@'].player = NONE;
          break;
        }
        if (newBoard->tiles[move->rank + 2][moveFile - 'B'].piece == KNIGHT &&
            newBoard->tiles[move->rank + 2][moveFile - 'B'].player ==
                player) {
          // checking if the old knight is two ranks up and one file right
          // from the present position of the new move on success: the spot
          // matching the placement of the old knight is returned to '-' on
          // the board and the player to none
          newBoard->tiles[move->rank + 2][moveFile - 'B'].piece = EMPTY;
          newBoard->tiles[move->rank + 2][moveFile - 'B'].player = NONE;
          break;
        }
        if (newBoard->tiles[move->rank + 2][moveFile - '@'].piece == KNIGHT &&
            newBoard->tiles[move->rank + 2][moveFile - '@'].player ==
                player) {
          // checking if the old knight is two ranks up and one file left
          // from the present position of the new move on success: the spot
          // matching the placement of the old knight is returned to '-' on
          // the board and the player to none
          newBoard->tiles[move->rank + 2][moveFile - '@'].piece = EMPTY;
          newBoard->tiles[move->rank + 2][moveFile - '@'].player = NONE;
          break;
        }
        if (newBoard->tiles[move->rank + 1][moveFile - 'C'].piece == KNIGHT &&
            newBoard->tiles[move->rank + 1][moveFile - 'C'].player ==
                player) {
          // checking if the old knight is one rank up and two files right
          // from the present position of the new move on success: the spot
          // matching the placement of the old knight is returned to '-' on
          // the board and the player to none
          newBoard->tiles[move->rank + 1][moveFile - 'C'].piece = EMPTY;
          newBoard->tiles[move->rank + 1][moveFile - 'C'].player = NONE;
          break;
        }
        if (newBoard->tiles[move->rank + 1][moveFile - '?'].piece == KNIGHT &&
            newBoard->tiles[move->rank + 1][moveFile - '?'].player ==
                player) {
          // checking if the old knight is one rank up and two files left
          // from the present position of the new move on success: the spot
          // matching the placement of the old knight is returned to '-' on
          // the board and the player to none
          newBoard->tiles[move->rank + 1][moveFile - '?'].piece = EMPTY;
          newBoard->tiles[move->rank + 1][moveFile - '?'].player = NONE;
          break;
        }
        if (newBoard->tiles[move->rank - 1][moveFile - 'C'].piece == KNIGHT &&
            newBoard->tiles[move->rank - 1][moveFile - 'C'].player ==
                player) {
          // checking if the old knight is one rank down and two files right
          // from the present position of the new move on success: the spot
          // matching the placement of the old knight is returned to '-' on
          // the board and the player to none
          newBoard->tiles[move->rank - 1][moveFile - 'C'].piece = EMPTY;
          newBoard->tiles[move->rank - 1][moveFile - 'C'].player = NONE;
          break;
        }
        if (newBoard->tiles[move->rank - 1][moveFile - '?'].piece == KNIGHT &&
            newBoard->tiles[move->rank - 1][moveFile - '?'].player ==
                player) {
          // checking if the old knight is one rank down and two files left
          // from the present position of the new move on success: the spot
          // matching the placement of the old knight is returned to '-' on
          // the board and the player to none
          newBoard->tiles[move->rank - 1][moveFile - '?'].piece = EMPTY;
          newBoard->tiles[move->rank - 1][moveFile - '?'].player = NONE;
          break;
        }
      } else {
        printf("Move %s not found\n", moveToString(move));
        return newBoard;
      }

      break;
    case BISHOP:
      // first, checking for the legality of the move
      if (legalMove(move, board)) {
        // the new board has the new move added to the board, piece and side
        newBoard->tiles[move->rank][move->file - 'A'].player = player;
        newBoard->tiles[move->rank][move->file - 'A'].piece = move->piece;
        int file = move->file - 'A';
        for (int i = 1; i < 8; i++) {
          if (newBoard->tiles[move->rank - i][file - i].piece ==
                  newBoard->tiles[move->rank][move->file - 'A'].piece &&
              newBoard->tiles[move->rank - i][file - i].player ==
                  newBoard->tiles[move->rank][move->file - 'A'].player) {
            newBoard->tiles[move->rank - i][file - i].piece = EMPTY;
            newBoard->tiles[move->rank - i][file - i].player = NONE;
          }
          if (newBoard->tiles[move->rank + i][file - i].piece ==
                  newBoard->tiles[move->rank][move->file - 'A'].piece &&
              newBoard->tiles[move->rank + i][file - i].player ==
                  newBoard->tiles[move->rank][move->file - 'A'].player) {
            newBoard->tiles[move->rank + i][file - i].piece = EMPTY;
            newBoard->tiles[move->rank + i][file - i].player = NONE;
          }
          if (newBoard->tiles[move->rank - i][file + i].piece ==
                  newBoard->tiles[move->rank][move->file - 'A'].piece &&
              newBoard->tiles[move->rank - i][file + i].player ==
                  newBoard->tiles[move->rank][move->file - 'A'].player) {
            newBoard->tiles[move->rank - i][file + i].piece = EMPTY;
            newBoard->tiles[move->rank - i][file + i].player = NONE;
          }
          if (newBoard->tiles[move->rank + i][file + i].piece ==
                  newBoard->tiles[move->rank][move->file - 'A'].piece &&
              newBoard->tiles[move->rank + i][file + i].player ==
                  newBoard->tiles[move->rank][move->file - 'A'].player) {
            newBoard->tiles[move->rank + i][file + i].piece = EMPTY;
            newBoard->tiles[move->rank + i][file + i].player = NONE;
          }
        }
      } else {
        printf("Move %s not found\n", moveToString(move));
        return newBoard;
      }
      break;
    case ROOK:
      // the new board has the new move added to the board, piece and side
      newBoard->tiles[move->rank][move->file - 'A'].player = player;
      newBoard->tiles[move->rank][move->file - 'A'].piece = move->piece;
      // first, checking for the legality of the move
      if (legalMove(move, board)) {
        // ensuring that a collison on the file or rank that the rook
        // moves on does not happen
        for (int i = 0; i < 8; i++) {
          if (newBoard->tiles[move->rank - i][move->file - 'A'].piece ==
                  newBoard->tiles[move->rank][move->file - 'A'].piece &&
              newBoard->tiles[move->rank - i][move->file - 'A'].player ==
                  newBoard->tiles[move->rank][move->file - 'A'].player) {
            newBoard->tiles[move->rank - i][move->file - 'A'].player = NONE;
            newBoard->tiles[move->rank - i][move->file - 'A'].piece = EMPTY;
            break;
          }
          if (newBoard->tiles[move->rank + i][move->file - 'A'].piece ==
                  newBoard->tiles[move->rank][move->file - 'A'].piece &&
              newBoard->tiles[move->rank + i][move->file - 'A'].player ==
                  newBoard->tiles[move->rank][move->file - 'A'].player) {
            newBoard->tiles[move->rank + i][move->file - 'A'].player = NONE;
            newBoard->tiles[move->rank + i][move->file - 'A'].piece = EMPTY;
            break;
          }
          if (newBoard->tiles[move->rank][move->file - 'A' - i].piece ==
                  newBoard->tiles[move->rank][move->file - 'A'].piece &&
              newBoard->tiles[move->rank][move->file - 'A' - i].player ==
                  newBoard->tiles[move->rank][move->file - 'A'].player) {
            newBoard->tiles[move->rank][move->file - 'A' - i].player = NONE;
            newBoard->tiles[move->rank][move->file - 'A' - i].piece = EMPTY;
            break;
          }
          if (newBoard->tiles[move->rank][move->file - 'A' + i].piece ==
                  newBoard->tiles[move->rank][move->file - 'A'].piece &&
              newBoard->tiles[move->rank][move->file - 'A' + i].player ==
                  newBoard->tiles[move->rank][move->file - 'A'].player) {
            newBoard->tiles[move->rank][move->file - 'A' + i].player = NONE;
            newBoard->tiles[move->rank][move->file - 'A' + i].piece = EMPTY;
            break;
          }
        }
      } else {
        printf("Failure to detect rook in move: %s %s %c %d\n\n",
               getPlayerString(player), getPieceString(move->piece),
               (char)move->file, move->rank + 1);
        break;
      }
      break;
    case KING:
      // the new board has the new move added to the board, piece and side
      newBoard->tiles[move->rank][move->file - 'A'].player = player;
      newBoard->tiles[move->rank][move->file - 'A'].piece = move->piece;
      int fileMove = (int)move->file - 'A';
      if (legalMove(move, board)) {
        if (player == WHITE) {
          if ((move->rank + 1 <= 7) &&
              (fileMove - 1 >= 0 || fileMove + 1 <= 7)) {
            if (board->tiles[move->rank + 1][fileMove + 1].piece ==
                    move->piece &&
                board->tiles[move->rank + 1][fileMove + 1].player ==
                    player) {
              newBoard->tiles[move->rank + 1][fileMove + 1].player = NONE;
              newBoard->tiles[move->rank + 1][fileMove + 1].piece = EMPTY;
            }
            if (board->tiles[move->rank + 1][fileMove].piece == move->piece &&
                board->tiles[move->rank + 1][fileMove].player == player) {
              newBoard->tiles[move->rank + 1][fileMove].player = NONE;
              newBoard->tiles[move->rank + 1][fileMove].piece = EMPTY;
            }
            if (board->tiles[move->rank + 1][fileMove - 1].piece ==
                    move->piece &&
                board->tiles[move->rank + 1][fileMove - 1].player ==
                    player) {
              newBoard->tiles[move->rank + 1][fileMove - 1].player = NONE;
              newBoard->tiles[move->rank + 1][fileMove - 1].piece = EMPTY;
            }
          }
          if (fileMove - 1 >= 0 || fileMove + 1 <= 7) {
            if (board->tiles[move->rank][fileMove - 1].piece == move->piece &&
                board->tiles[move->rank][fileMove - 1].player == player) {
              newBoard->tiles[move->rank][fileMove - 1].player = NONE;
              newBoard->tiles[move->rank][fileMove - 1].piece = EMPTY;
            }
            if (board->tiles[move->rank][fileMove + 1].piece == move->piece &&
                board->tiles[move->rank][fileMove + 1].player == player) {
              newBoard->tiles[move->rank][fileMove + 1].player = NONE;
              newBoard->tiles[move->rank][fileMove + 1].piece = EMPTY;
            }
            if ((move->rank + 1 <= 7 || move->rank >= 0) &&
                (fileMove - 1 >= 0 || fileMove + 1 <= 7)) {
              if (board->tiles[move->rank - 1][fileMove - 1].piece ==
                      move->piece &&
                  board->tiles[move->rank - 1][fileMove - 1].player ==
                      player) {
                newBoard->tiles[move->rank - 1][fileMove - 1].player = NONE;
                newBoard->tiles[move->rank - 1][fileMove - 1].piece = EMPTY;
              }
              if (board->tiles[move->rank - 1][fileMove].piece == move->piece &&
                  board->tiles[move->rank - 1][fileMove].player ==
                      player) {
                newBoard->tiles[move->rank - 1][fileMove].player = NONE;
                newBoard->tiles[move->rank - 1][fileMove].piece = EMPTY;
              }
              if (board->tiles[move->rank - 1][fileMove + 1].piece ==
                      move->piece &&
                  board->tiles[move->rank - 1][fileMove + 1].player ==
                      player) {
                newBoard->tiles[move->rank - 1][fileMove + 1].player = NONE;
                newBoard->tiles[move->rank - 1][fileMove + 1].piece = EMPTY;
              }
            }
          } else {
            printf("Move %s not found\n", moveToString(move));
            return newBoard;
          }
        }
        if (player == BLACK) {
          if ((move->rank + 1 <= 7) &&
              (fileMove - 1 >= 0 || fileMove + 1 <= 7)) {
            if (board->tiles[move->rank + 1][fileMove + 1].piece ==
                    move->piece &&
                board->tiles[move->rank + 1][fileMove + 1].player ==
                    player) {
              newBoard->tiles[move->rank + 1][fileMove + 1].player = NONE;
              newBoard->tiles[move->rank + 1][fileMove + 1].piece = EMPTY;
            }
            if (board->tiles[move->rank + 1][fileMove].piece == move->piece &&
                board->tiles[move->rank + 1][fileMove].player == player) {
              newBoard->tiles[move->rank + 1][fileMove].player = NONE;
              newBoard->tiles[move->rank + 1][fileMove].piece = EMPTY;
            }
            if (board->tiles[move->rank + 1][fileMove - 1].piece ==
                    move->piece &&
                board->tiles[move->rank + 1][fileMove - 1].player ==
                    player) {
              newBoard->tiles[move->rank + 1][fileMove - 1].player = NONE;
              newBoard->tiles[move->rank + 1][fileMove - 1].piece = EMPTY;
            }
          }
          if (fileMove - 1 >= 0 || fileMove + 1 <= 7) {
            if (board->tiles[move->rank][fileMove - 1].piece == move->piece &&
                board->tiles[move->rank][fileMove - 1].player == player) {
              newBoard->tiles[move->rank][fileMove - 1].player = NONE;
              newBoard->tiles[move->rank][fileMove - 1].piece = EMPTY;
            }
            if (board->tiles[move->rank][fileMove + 1].piece == move->piece &&
                board->tiles[move->rank][fileMove + 1].player == player) {
              newBoard->tiles[move->rank][fileMove + 1].player = NONE;
              newBoard->tiles[move->rank][fileMove + 1].piece = EMPTY;
            }
            if ((move->rank + 1 <= 7 || move->rank >= 0) &&
                (fileMove - 1 >= 0 || fileMove + 1 <= 7)) {
              if (board->tiles[move->rank - 1][fileMove - 1].piece ==
                      move->piece &&
                  board->tiles[move->rank - 1][fileMove - 1].player ==
                      player) {
                newBoard->tiles[move->rank - 1][fileMove - 1].player = NONE;
                newBoard->tiles[move->rank - 1][fileMove - 1].piece = EMPTY;
              }
              if (board->tiles[move->rank - 1][fileMove].piece == move->piece &&
                  board->tiles[move->rank - 1][fileMove].player ==
                      player) {
                newBoard->tiles[move->rank - 1][fileMove].player = NONE;
                newBoard->tiles[move->rank - 1][fileMove].piece = EMPTY;
              }
              if (board->tiles[move->rank - 1][fileMove + 1].piece ==
                      move->piece &&
                  board->tiles[move->rank - 1][fileMove + 1].player ==
                      player) {
                newBoard->tiles[move->rank - 1][fileMove + 1].player = NONE;
                newBoard->tiles[move->rank - 1][fileMove + 1].piece = EMPTY;
              }
            } else {
              printf("Move %s not found\n", moveToString(move));
              return newBoard;
            }
          }
        }
      }
      break;
    case QUEEN:

      if (legalMove(move, board)) {
        // the new board has the new move added to the board, piece and side
        newBoard->tiles[move->rank][move->file - 'A'].player = player;
        newBoard->tiles[move->rank][move->file - 'A'].piece = move->piece;
        int moveFile = (int)move->file - 'A';
        for (int i = 1; i < 8; i++) {
          if (newBoard->tiles[move->rank + i][moveFile].piece == move->piece &&
              newBoard->tiles[move->rank + i][moveFile].player ==
                  player) {
            newBoard->tiles[move->rank + i][moveFile].piece = EMPTY;
            newBoard->tiles[move->rank + i][moveFile].player = NONE;

            break;
          }
          if (newBoard->tiles[move->rank + i][moveFile + i].piece ==
                  move->piece &&
              newBoard->tiles[move->rank + i][moveFile + i].player ==
                  player) {
            newBoard->tiles[move->rank + i][moveFile + i].piece = EMPTY;
            newBoard->tiles[move->rank + i][moveFile + i].player = NONE;
            break;
          }
          if (newBoard->tiles[move->rank + i][moveFile - i].piece ==
                  move->piece &&
              newBoard->tiles[move->rank + i][moveFile - i].player ==
                  player) {
            newBoard->tiles[move->rank + i][moveFile - i].piece = EMPTY;
            newBoard->tiles[move->rank + i][moveFile - i].player = NONE;
            break;
          }
          if (newBoard->tiles[move->rank - i][moveFile].piece == move->piece &&
              newBoard->tiles[move->rank - i][moveFile].player ==
                  player) {
            newBoard->tiles[move->rank - i][moveFile].piece = EMPTY;
            newBoard->tiles[move->rank - i][moveFile].player = NONE;
            break;
          }
          if (newBoard->tiles[move->rank - i][moveFile + i].piece ==
                  move->piece &&
              newBoard->tiles[move->rank - i][moveFile + i].player ==
                  player) {
            newBoard->tiles[move->rank - i][moveFile + i].piece = EMPTY;
            newBoard->tiles[move->rank - i][moveFile + i].player = NONE;
            break;
          }
          if (newBoard->tiles[move->rank - i][moveFile - i].piece ==
                  move->piece &&
              newBoard->tiles[move->rank - i][moveFile - i].player ==
                  player) {
            newBoard->tiles[move->rank - i][moveFile - i].piece = EMPTY;
            newBoard->tiles[move->rank - i][moveFile - i].player = NONE;
            break;
          }
          if (newBoard->tiles[move->rank][moveFile + i].piece == move->piece &&
              newBoard->tiles[move->rank][moveFile + i].player ==
                  player) {
            newBoard->tiles[move->rank][moveFile + i].piece = EMPTY;
            newBoard->tiles[move->rank][moveFile + i].player = NONE;
            break;
          }
          if (newBoard->tiles[move->rank][moveFile - i].piece == move->piece &&
              newBoard->tiles[move->rank][moveFile - i].player ==
                  player) {
            newBoard->tiles[move->rank][moveFile - i].piece = EMPTY;
            newBoard->tiles[move->rank][moveFile - i].player = NONE;
            break;
          }
        }
      } else {
        printf("Move %s not found\n", moveToString(move));
        return newBoard;
      }
      break;
    default:
        if (isCastle(move->flags)) {
        if (player == WHITE) {
          newBoard->tiles[0][5].piece = ROOK;
          newBoard->tiles[0][5].player = WHITE;
          newBoard->tiles[0][6].piece = KING;
          newBoard->tiles[0][6].player = WHITE;
          newBoard->tiles[0][7].piece = EMPTY;
          newBoard->tiles[0][7].player = NONE;
          newBoard->tiles[0][4].piece = EMPTY;
          newBoard->tiles[0][4].player = NONE;
        }
        if (player == BLACK) {
          newBoard->tiles[7][5].piece = ROOK;
          newBoard->tiles[7][5].player = BLACK;
          newBoard->tiles[7][6].piece = KING;
          newBoard->tiles[7][6].player = BLACK;
          newBoard->tiles[7][7].piece = EMPTY;
          newBoard->tiles[7][7].player = NONE;
          newBoard->tiles[7][4].piece = EMPTY;
          newBoard->tiles[7][4].player = NONE;
        }
      }
      if (move->flags == TAKES) {
        break;
      }
      return newBoard;
      break;
    }
  }
  return newBoard;
}
const char *getPieceString(enum piece piece) {
  switch (piece) {
  case QUEEN:
    return "Queen";
    break;
  case KING:
    return "King";
    break;
  case ROOK:
    return "Rook";
    break;
  case KNIGHT:
    return "Knight";
    break;
  case BISHOP:
    return "Bishop";
    break;
  case PAWN:
    return "Pawn";
    break;
  case EMPTY:
    return "No piece";
    break;
  }
  return "Invalid piece";
}
const char *getPlayerString(enum player player) {
  switch (player) {
  case WHITE:
    return "White";
    break;
  case BLACK:
    return "Black";
    break;
  case NONE:
    return "No player";
    break;
  }
  return "Invalid player";
}
void destroyBoard(struct board *board) { free(board); }
