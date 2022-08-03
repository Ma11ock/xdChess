#include "board.h"
#include "list.h"
#include "moves.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define RULE_GETCHAR() bool isEof = false; c = pullChar(f, &isEof);\
    if(isEof) { move.flags = CHESS_EOF; return move; }


void printMove(const struct move *move, enum player player) {
    char *moveString = moveToString(move, player);
  printf("Board after playing the move: %s\n\n", moveString);
  free(moveString);
}

char *moveToString(const struct move *move, enum player player) {
  int size =
      snprintf(NULL, 0, "%s %s %c%d", getPlayerString(player),
               getPieceString(move->piece), (char)move->file, move->rank + 1);
  char *string = malloc(size + 1);
  if (string == NULL) {
    return NULL;
  }
  snprintf(string, size + 1, "%s %s %c%d", getPlayerString(player),
           getPieceString(move->piece), (char)move->file, move->rank);
  return string;
}
// resetting moves for the time the program interprets moves
// in a list

static bool isPeice(char c) {
    switch(c) {
    case 'Q':
    case 'N':
    case 'B':
    case 'R':
    case 'K':
        return true;
    default:
        break;
    }
    return false;
}

static bool isNormalPiece(char c) {
    switch(c) {
    case 'Q':
    case 'N':
    case 'B':
    case 'R':
        return true;
    default:
        break;
    }
    return false;
}


static bool isRank(char c) {
    return (c <= '8' && c >= '1');
}

static bool isFile(char c) {
    return (c <= 'h' && c >= 'a');
}

static bool shouldRandom(unsigned long chance) {
    return false;
    static bool isInit = false;
    if(!isInit) {
        srandom((unsigned int)getpid());
        isInit = true;
    }

    chance = MIN(chance, 100);
    return (unsigned long)random() % 100 <= chance;
}

static char pullChar(FILE* file, bool *end) {
    int result = fgetc(file);
    *end = (result == EOF);
    return (char)result;
}

static struct move checkCheck(FILE *f, struct move move) {
    char c;
    while(true) {
        RULE_GETCHAR();
        switch(c) {
        case '#':
            move.flags |= CHECKMATE;
            return move;
            break;
        case '+':
            move.flags |= CHECK;
            return move;
            break;
        default:
            break;
        }
    }
    return DEFAULT_MOVE;
}

static struct move piecesTakesSeesFile(FILE *f, struct move move) {
    char c;
    while(true) {
        RULE_GETCHAR();
        if(isRank(c)) {
            move.rank = c - '0';
            if(shouldRandom(10)) {
                return checkCheck(f, move);
            }
            return move;
        }
    }
    return DEFAULT_MOVE;
}

static struct move pieceSeesTake(FILE *f, struct move move) {
    char c;
    while(true) {
        RULE_GETCHAR();
        if(isFile(c)) {
            move.file = c;
            return piecesTakesSeesFile(f, move);
        }
    }
    return DEFAULT_MOVE;
}

static struct move pieceSeesFile(FILE *f, struct move move) {
    char c;
    while(true) {
        RULE_GETCHAR();
        if(isRank(c)) {
            move.rank = c - '0';
            return move;
        }
    }
    return DEFAULT_MOVE;
}

static struct move startPieceMove(FILE *f, struct move move) {
    char c;
    while(true) {
        RULE_GETCHAR();
        if(isFile(c)) {
            move.file = c;
            return pieceSeesFile(f, move);
        }
        if(c == 'x') {
            move.flags |= TAKES;
            return pieceSeesTake(f, move);
        }
    }
}

static struct move pawnGetEP(FILE *f, struct move move) {
    char c;
    int numFound = 0;
    while(true) {
        RULE_GETCHAR();
        if(c == 'e' && !numFound) {
            numFound++;
        }
        else if(c == '.') {
            switch(numFound) {
            case 1:
                numFound++;
                break;
            case 3:
                move.flags |= ENPASSANT;
                return move;
                break;
            default:
                break;
            }
        }
        else if(c =='p' && numFound == 2) {
            numFound++;
        }
    }
}

static struct move startPawnMove(FILE *f, struct move move) {
    move.piece = PAWN;
    char c;
    while(true) {
        RULE_GETCHAR();

        if(isRank(c)) {
            move.rank = c - '0';
            if(shouldRandom(5)) {
                // Get more information like en passant.
                return pawnGetEP(f, move);
            }
            return move;
        }
        if(c == 'x' && (move.flags & TAKES) == 0) {
            move.flags |= TAKES;
            move.disFile = move.file;
            // Get the destination file.
            char c;
            while(true) {
                RULE_GETCHAR();
                if(isFile(c)) {
                    move.file = c;
                    return startPawnMove(f, move);
                }
            }
        }
    }
}

static struct move castleMove(FILE *f, struct move move) {
    int charsFound = 1;
    move.piece = KING;
    char c;
    while(true) {
        RULE_GETCHAR();
        if(c == 'O') {
            switch(charsFound) {
            case 2:
                charsFound++;
                if(!shouldRandom(50)) {
                    move.flags |= SHORT_CASTLE;
                    return move;
                }
                break;
            case 4:
                if(!shouldRandom(90)) {
                    move.flags |= LONG_CASTLE;
                    return move;
                }
                break;
            case 6:
            case 8:
                charsFound++;
                break;
            case 10:
                move.flags |= VERTICAL_CASTLE;
                return move;
                break;
            default:
                break;
            }
        }
        else if(c == '-') {
            switch(charsFound) {
            case 1:
            case 3:
            case 5:
            case 7:
            case 9:
                charsFound++;
                break;
            default:
                break;
            }
        }
    }
}

static struct move startMove(FILE *f) {
    char c;
    struct move move = DEFAULT_MOVE;
    while(true) {
        RULE_GETCHAR();
        if(isNormalPiece(c)) {
            move.piece = c;
            move.flags = CHESS_NORMAL;
            return startPieceMove(f, move);
        }
        else if(isFile(c)) {
            move.file = c;
            move.flags = CHESS_NORMAL;
            return startPawnMove(f, move);
        }
        else if(c == 'O') {
            return castleMove(f, move);
        }
    }
    return move;
}


struct move pullMove(FILE *file) {
    if(!file) {
        fprintf(stderr, "pullmove: file is NULL.\n");
        return DEFAULT_PIECE;
    }
    return startMove();
}


bool isCastle(unsigned flags) {
    return (flags & LONG_CASTLE) || (flags & SHORT_CASTLE) || (flags & VERTICAL_CASTLE);
}
