
/*DEFINITION*/
#define MAX_MOVES 269
#define MAX_PLY 10
#define ENGINE_DEPTH 3

#define PAWN_VALUE 100
#define KNIGHT_VALUE 300
#define BISHOP_VALUE 300
#define ROOK_VALUE 500
#define QUEEN_VALUE 900
#define KING_VALUE 9999

/*IMPORTS*/
#include <stdio.h>
#include "enum.h"
#include "pcsq.h"
#include <time.h>

/*GLOBAL VARIABLES*/
int board[120]; // 10x12
int moveGen[MAX_MOVES][2]; // first column: starting square, second column: ending square
int moveCount = 0;
bool castling[4] = { true, }; //check if castling is available
time_t timer;

/*FUNCTIONS*/
void setupBoard(); // initializes the board
void printBoard(); // prints the current board
void moveGenerator(int board[120], int color); // generates and stores all possible moves
void pawnMoves(int board[120], int position, int color); // generates and stores all possible pawn moves
void knightMoves(int board[120], int position, int color); // generates and stores all possible knight moves
void rookMoves(int board[120], int position, int color); // generates and stores all possible rook moves
void bishopMoves(int board[120], int position, int color); // generates and stores all possible bishop moves
void queenMoves(int board[120], int position, int color); // generates and stores all possible queen moves
void kingMoves(int board[120], int position, int color); // generates and stores all possible king moves
int boardEvaluation(int board[120]); //evaluates current board situation using PCSQ tables and existing pieces
int determineColor(int piece); //returns the color of the piece (wK -> WHITE, bK -> BLACK, NOCOLOR)
bool legalSquare(int position); //asks if the position number is one on the 8x8 chessboard
int reverseBoard(int position); //used for PCSQ evaluation
void makeMove(int move[2], int currentPly);
void unmakeMove(int currentPly);
void printBoardSimple(int board[120]);
char numberToPiece(int number); //function that returns a symbol of piece the parameter number is

/*CODE*/
void setupBoard() {
	
	for (int i = 0; i < 12; i++) { // initialize board
		for (int j = 0; j < 10; j++) {
			board[i * 10 + j] = ERROR;
		}
	}

	for (int i = 2; i < 10; i++) { // initialize the playable board (8x8)
		for (int j = 1; j < 9; j++) {
			board[i * 10 + j] = EMPTY;
		}
	}

	for (int i = 0; i < 8; i++) { // initialize pawns
		board[A2 + i] = wP;
		board[A7 + i] = bP;
	}

	//initialize the rest of the pieces
	board[A1] = wR;
	board[B1] = wN;
	board[C1] = wB;
	board[D1] = wQ;
	board[E1] = wK;
	board[F1] = wB;
	board[G1] = wN;
	board[H1] = wR;

	board[A8] = bR;
	board[B8] = bN;
	board[C8] = bB;
	board[D8] = bQ;
	board[E8] = bK;
	board[F8] = bB;
	board[G8] = bN;
	board[H8] = bR;
}
void printBoard(int board[120]) {
	for (int i = 0; i < 12; i++) {
		for (int j = 0; j < 10; j++) {
			switch (board[i * 10 + j]) {
			case wP:
				printf("P");
				break;
			case wR:
				printf("R");
				break;
			case wN:
				printf("N");
				break;
			case wB:
				printf("B");
				break;
			case wQ:
				printf("Q");
				break;
			case wK:
				printf("K");
				break;
			case bP:
				printf("p");
				break;
			case bR:
				printf("r");
				break;
			case bN:
				printf("n");
				break;
			case bB:
				printf("b");
				break;
			case bQ:
				printf("q");
				break;
			case bK:
				printf("k");
				break;
			case EMPTY:
				printf("_");
				break;
			case ERROR:
				printf("X");
				break;
			
			}
			printf(" ");
		}
		printf("\n");
	}
}
void moveGenerator(int board[120], int color) {
	//initialize (not needed)
	for (int i = 0; i < moveCount; i++) {
		moveGen[i][0] = 0;
		moveGen[i][1] = 0;
	}
	moveCount = 0;

	for (int i = 0; i < 120; i++) {
		if (board[i] == EMPTY || board[i] == ERROR) {
			continue;
		}
		if (wP <= board[i] && board[i] <= wK && color == BLACK) {
			continue;
		}
		if (bP <= board[i] && board[i] <= bK && color == WHITE) {
			continue;
		}

		switch (board[i]) {
		case wP:
			pawnMoves(board, i, WHITE);
			break;
		case wN:
			knightMoves(board, i, WHITE);
			break;
		case wB:
			bishopMoves(board, i, WHITE);
			break;
		case wR:
			rookMoves(board, i, WHITE);
			break;
		case wQ:
			queenMoves(board, i, WHITE);
			break;
		case wK:
			kingMoves(board, i, WHITE);
			break;
		case bP:
			pawnMoves(board, i, BLACK);
			break;
		case bN:
			knightMoves(board, i, BLACK);
			break;
		case bB:
			bishopMoves(board, i, BLACK);
			break;
		case bR:
			rookMoves(board, i, BLACK);
			break;
		case bQ:
			queenMoves(board, i, BLACK);
			break;
		case bK:
			kingMoves(board, i, BLACK);
			break;
		}
	}

	//pawn moves
	//knight moves
	//rook moves
	//bishop moves
	//queen moves
	//king moves
}
void pawnMoves(int board[120], int position, int color) {
	if (color == WHITE) {
		if (A2 <= position && position <= H2 && board[position - 20] == EMPTY && board[position - 10] == EMPTY) { //if it did not move yet
			moveGen[moveCount][0] = position;
			moveGen[moveCount][1] = position - 20;
			moveCount++;
		}
		if (board[position - 10] == EMPTY) {
			moveGen[moveCount][0] = position;
			moveGen[moveCount][1] = position - 10;
			moveCount++;
		}
		if (determineColor(board[position - 10 - 1]) == BLACK) {
			moveGen[moveCount][0] = position;
			moveGen[moveCount][1] = position - 10 - 1;
			moveCount++;
		}
		if (determineColor(board[position - 10 + 1]) == BLACK) {
			moveGen[moveCount][0] = position;
			moveGen[moveCount][1] = position - 10 + 1;
			moveCount++;
		}
	}
	else if (color == BLACK) {
		if (A7 <= position && position <= H7 && board[position + 20] == EMPTY && board[position + 10] == EMPTY) { //if it did not move yet
			moveGen[moveCount][0] = position;
			moveGen[moveCount][1] = position + 20;
			moveCount++;
		}
		if (board[position + 10] == EMPTY) {
			moveGen[moveCount][0] = position;
			moveGen[moveCount][1] = position + 10;
			moveCount++;
		}
		if (determineColor(board[position + 10 - 1]) == WHITE) {
			moveGen[moveCount][0] = position;
			moveGen[moveCount][1] = position + 10 - 1;
			moveCount++;
		}
		if (determineColor(board[position + 10 + 1]) == WHITE) {
			moveGen[moveCount][0] = position;
			moveGen[moveCount][1] = position + 10 + 1;
			moveCount++;
		}
	}
}
void knightMoves(int board[120], int position, int color) {
	// _ 8 _ 1 _
	// 7 _ _ _ 2
	// _ _ N _ _
	// 6 _ _ _ 3
	// _ 5 _ 4 _

	if (color == WHITE) {
		if (determineColor(board[position - 20 + 1]) != WHITE && legalSquare(position - 20 + 1) == true) { // position 1
			moveGen[moveCount][0] = position;
			moveGen[moveCount][1] = position - 20 + 1;
			moveCount++;
		}
		if (determineColor(board[position - 10 + 2]) != WHITE && legalSquare(position - 10 + 2) == true) { // position 2
			moveGen[moveCount][0] = position;
			moveGen[moveCount][1] = position - 10 + 2;
			moveCount++;
		}
		if (determineColor(board[position + 10 + 2]) != WHITE && legalSquare(position + 10 + 2) == true) { // position 3
			moveGen[moveCount][0] = position;
			moveGen[moveCount][1] = position + 10 + 2;
			moveCount++;
		}
		if (determineColor(board[position + 20 + 1]) != WHITE && legalSquare(position + 20 + 1) == true) { // position 4
			moveGen[moveCount][0] = position;
			moveGen[moveCount][1] = position + 20 + 1;
			moveCount++;
		}
		if (determineColor(board[position + 20 - 1]) != WHITE && legalSquare(position + 20 - 1) == true) { // position 5
			moveGen[moveCount][0] = position;
			moveGen[moveCount][1] = position + 20 - 1;
			moveCount++;
		}
		if (determineColor(board[position + 10 - 2]) != WHITE && legalSquare(position + 10 - 2) == true) { // position 6
			moveGen[moveCount][0] = position;
			moveGen[moveCount][1] = position + 10 - 2;
			moveCount++;
		}
		if (determineColor(board[position - 10 - 2]) != WHITE && legalSquare(position - 10 - 2) == true) { // position 7
			moveGen[moveCount][0] = position;
			moveGen[moveCount][1] = position - 10 - 2;
			moveCount++;
		}
		if (determineColor(board[position - 20 - 1]) != WHITE && legalSquare(position - 20 - 1) == true) { // position 8
			moveGen[moveCount][0] = position;
			moveGen[moveCount][1] = position - 20 - 1;
			moveCount++;
		}
	}
	if (color == BLACK) {
		if (determineColor(board[position - 20 + 1]) != BLACK && legalSquare(position - 20 + 1) == true) { // position 1
			moveGen[moveCount][0] = position;
			moveGen[moveCount][1] = position - 20 + 1;
			moveCount++;
		}
		if (determineColor(board[position - 10 + 2]) != BLACK && legalSquare(position - 10 + 2) == true) { // position 2
			moveGen[moveCount][0] = position;
			moveGen[moveCount][1] = position - 10 + 2;
			moveCount++;
		}
		if (determineColor(board[position + 10 + 2]) != BLACK && legalSquare(position + 10 + 2) == true) { // position 3
			moveGen[moveCount][0] = position;
			moveGen[moveCount][1] = position + 10 + 2;
			moveCount++;
		}
		if (determineColor(board[position + 20 + 1]) != BLACK && legalSquare(position + 20 + 1) == true) { // position 4
			moveGen[moveCount][0] = position;
			moveGen[moveCount][1] = position + 20 + 1;
			moveCount++;
		}
		if (determineColor(board[position + 20 - 1]) != BLACK && legalSquare(position + 20 - 1) == true) { // position 5
			moveGen[moveCount][0] = position;
			moveGen[moveCount][1] = position + 20 - 1;
			moveCount++;
		}
		if (determineColor(board[position + 10 - 2]) != BLACK && legalSquare(position + 10 - 2) == true) { // position 6
			moveGen[moveCount][0] = position;
			moveGen[moveCount][1] = position + 10 - 2;
			moveCount++;
		}
		if (determineColor(board[position - 10 - 2]) != BLACK && legalSquare(position - 10 - 2) == true) { // position 7
			moveGen[moveCount][0] = position;
			moveGen[moveCount][1] = position - 10 - 2;
			moveCount++;
		}
		if (determineColor(board[position - 20 - 1]) != BLACK && legalSquare(position - 20 - 1) == true) { // position 8
			moveGen[moveCount][0] = position;
			moveGen[moveCount][1] = position - 20 - 1;
			moveCount++;
		}
	}
}
void rookMoves(int board[120], int position, int color) {
	if (color == WHITE) {
		//moving down
		for (int i = 1; i < 8; i++) {
			if (legalSquare(position + 10 * i) == false) {
				break;
			}
			if (determineColor(board[position + 10 * i]) == WHITE) {
				break;
			}
			else if (determineColor(board[position + 10 * i]) == BLACK) {
				moveGen[moveCount][0] = position;
				moveGen[moveCount][1] = position + 10 * i;
				moveCount++;
				break;
			}
			else { //board is unoccupied
				moveGen[moveCount][0] = position;
				moveGen[moveCount][1] = position + 10 * i;
				moveCount++;
			}
		}

		//moving up
		for (int i = 1; i < 8; i++) {
			if (legalSquare(position - 10 * i) == false) {
				break;
			}
			if (determineColor(board[position - 10 * i]) == WHITE) {
				break;
			}
			else if (determineColor(board[position - 10 * i]) == BLACK) {
				moveGen[moveCount][0] = position;
				moveGen[moveCount][1] = position - 10 * i;
				moveCount++;
				break;
			}
			else { //board is unoccupied
				moveGen[moveCount][0] = position;
				moveGen[moveCount][1] = position - 10 * i;
				moveCount++;
			}
		}

		//moving right
		for (int i = 1; i < 8; i++) {
			if (legalSquare(position + i) == false) {
				break;
			}
			if (determineColor(board[position + i]) == WHITE) {
				break;
			}
			else if (determineColor(board[position + i]) == BLACK) {
				moveGen[moveCount][0] = position;
				moveGen[moveCount][1] = position + i;
				moveCount++;
				break;
			}
			else { //board is unoccupied
				moveGen[moveCount][0] = position;
				moveGen[moveCount][1] = position + i;
				moveCount++;
			}
		}

		//moving left
		for (int i = 1; i < 8; i++) {
			if (legalSquare(position - i) == false) {
				break;
			}
			if (determineColor(board[position - i]) == WHITE) {
				break;
			}
			else if (determineColor(board[position - i]) == BLACK) {
				moveGen[moveCount][0] = position;
				moveGen[moveCount][1] = position - i;
				moveCount++;
				break;
			}
			else { //board is unoccupied
				moveGen[moveCount][0] = position;
				moveGen[moveCount][1] = position - i;
				moveCount++;
			}
		}
	}
	
	if (color == BLACK) {
		
		//moving down
		for (int i = 1; i < 8; i++) {
			if (legalSquare(position + 10 * i) == false) {
				break;
			}
			if (determineColor(board[position + 10 * i]) == BLACK) {
				break;
			}
			else if (determineColor(board[position + 10 * i]) == WHITE) {
				moveGen[moveCount][0] = position;
				moveGen[moveCount][1] = position + 10 * i;
				moveCount++;
				break;
			}
			else { //board is unoccupied
				moveGen[moveCount][0] = position;
				moveGen[moveCount][1] = position + 10 * i;
				moveCount++;
			}
		}

		//moving up
		for (int i = 1; i < 8; i++) {
			if (legalSquare(position - 10 * i) == false) {
				break;
			}
			if (determineColor(board[position - 10 * i]) == BLACK) {
				break;
			}
			else if (determineColor(board[position - 10 * i]) == WHITE) {
				moveGen[moveCount][0] = position;
				moveGen[moveCount][1] = position - 10 * i;
				moveCount++;
				break;
			}
			else { //board is unoccupied
				moveGen[moveCount][0] = position;
				moveGen[moveCount][1] = position - 10 * i;
				moveCount++;
			}
		}

		//moving right
		for (int i = 1; i < 8; i++) {
			if (legalSquare(position + i) == false) {
				break;
			}
			if (determineColor(board[position + i]) == BLACK) {
				break;
			}
			else if (determineColor(board[position + i]) == WHITE) {
				moveGen[moveCount][0] = position;
				moveGen[moveCount][1] = position + i;
				moveCount++;
				break;
			}
			else { //board is unoccupied
				moveGen[moveCount][0] = position;
				moveGen[moveCount][1] = position + i;
				moveCount++;
			}
		}

		//moving left
		for (int i = 1; i < 8; i++) {
			if (legalSquare(position - i) == false) {
				break;
			}
			if (determineColor(board[position - i]) == BLACK) {
				break;
			}
			else if (determineColor(board[position - i]) == WHITE) {
				moveGen[moveCount][0] = position;
				moveGen[moveCount][1] = position - i;
				moveCount++;
				break;
			}
			else { //board is unoccupied
				moveGen[moveCount][0] = position;
				moveGen[moveCount][1] = position - i;
				moveCount++;
			}
		}
	}
}
void bishopMoves(int board[120], int position, int color) {
	if (color == WHITE) {
		// 2 _ _ _ 1
		// _ 2 _ 1 _
		// _ _ B _ _
		// _ 3 _ 4 _
		// 3 _ _ _ 4

		//diagonal 1
		for (int i = 1; i < 8; i++) {
			if (legalSquare(position + (-10 + 1) * i) == false) {
				break;
			}
			if (determineColor(board[position + (-10 + 1) * i]) == WHITE) {
				break;
			}
			else if (determineColor(board[position + (-10 + 1) * i]) == BLACK) {
				moveGen[moveCount][0] = position;
				moveGen[moveCount][1] = position + (-10 + 1) * i;
				moveCount++;
				break;
			}
			else { //board is unoccupied
				moveGen[moveCount][0] = position;
				moveGen[moveCount][1] = position + (-10 + 1) * i;
				moveCount++;
			}
		}

		//diagonal 2
		for (int i = 1; i < 8; i++) {
			if (legalSquare(position + (-10 - 1) * i) == false) {
				break;
			}
			if (determineColor(board[position + (-10 - 1) * i]) == WHITE) {
				break;
			}
			else if (determineColor(board[position + (-10 - 1) * i]) == BLACK) {
				moveGen[moveCount][0] = position;
				moveGen[moveCount][1] = position + (-10 - 1) * i;
				moveCount++;
				break;
			}
			else { //board is unoccupied
				moveGen[moveCount][0] = position;
				moveGen[moveCount][1] = position + (-10 - 1) * i;
				moveCount++;
			}
		}

		//diagonal 3
		for (int i = 1; i < 8; i++) {
			if (legalSquare(position + (10 - 1) * i) == false) {
				break;
			}
			if (determineColor(board[position + (10 - 1) * i]) == WHITE) {
				break;
			}
			else if (determineColor(board[position + (10 - 1) * i]) == BLACK) {
				moveGen[moveCount][0] = position;
				moveGen[moveCount][1] = position + (10 - 1) * i;
				moveCount++;
				break;
			}
			else { //board is unoccupied
				moveGen[moveCount][0] = position;
				moveGen[moveCount][1] = position + (10 - 1) * i;
				moveCount++;
			}
		}

		//diagonal 4
		for (int i = 1; i < 8; i++) {
			if (legalSquare(position + (10 + 1) * i) == false) {
				break;
			}
			if (determineColor(board[position + (10 + 1) * i]) == WHITE) {
				break;
			}
			else if (determineColor(board[position + (10 + 1) * i]) == BLACK) {
				moveGen[moveCount][0] = position;
				moveGen[moveCount][1] = position + (10 + 1) * i;
				moveCount++;
				break;
			}
			else { //board is unoccupied
				moveGen[moveCount][0] = position;
				moveGen[moveCount][1] = position + (10 + 1) * i;
				moveCount++;
			}
		}
	}
	if (color == BLACK) {
		// 2 _ _ _ 1
		// _ 2 _ 1 _
		// _ _ B _ _
		// _ 3 _ 4 _
		// 3 _ _ _ 4

		//diagonal 1
		for (int i = 1; i < 8; i++) {
			if (legalSquare(position + (-10 + 1) * i) == false) {
				break;
			}
			if (determineColor(board[position + (-10 + 1) * i]) == BLACK) {
				break;
			}
			else if (determineColor(board[position + (-10 + 1) * i]) == WHITE) {
				moveGen[moveCount][0] = position;
				moveGen[moveCount][1] = position + (-10 + 1) * i;
				moveCount++;
				break;
			}
			else { //board is unoccupied
				moveGen[moveCount][0] = position;
				moveGen[moveCount][1] = position + (-10 + 1) * i;
				moveCount++;
			}
		}

		//diagonal 2
		for (int i = 1; i < 8; i++) {
			if (legalSquare(position + (-10 - 1) * i) == false) {
				break;
			}
			if (determineColor(board[position + (-10 - 1) * i]) == BLACK) {
				break;
			}
			else if (determineColor(board[position + (-10 - 1) * i]) == WHITE) {
				moveGen[moveCount][0] = position;
				moveGen[moveCount][1] = position + (-10 - 1) * i;
				moveCount++;
				break;
			}
			else { //board is unoccupied
				moveGen[moveCount][0] = position;
				moveGen[moveCount][1] = position + (-10 - 1) * i;
				moveCount++;
			}
		}

		//diagonal 3
		for (int i = 1; i < 8; i++) {
			if (legalSquare(position + (10 - 1) * i) == false) {
				break;
			}
			if (determineColor(board[position + (10 - 1) * i]) == BLACK) {
				break;
			}
			else if (determineColor(board[position + (10 - 1) * i]) == WHITE) {
				moveGen[moveCount][0] = position;
				moveGen[moveCount][1] = position + (10 - 1) * i;
				moveCount++;
				break;
			}
			else { //board is unoccupied
				moveGen[moveCount][0] = position;
				moveGen[moveCount][1] = position + (10 - 1) * i;
				moveCount++;
			}
		}

		//diagonal 4
		for (int i = 1; i < 8; i++) {
			if (legalSquare(position + (10 + 1) * i) == false) {
				break;
			}
			if (determineColor(board[position + (10 + 1) * i]) == BLACK) {
				break;
			}
			else if (determineColor(board[position + (10 + 1) * i]) == WHITE) {
				moveGen[moveCount][0] = position;
				moveGen[moveCount][1] = position + (10 + 1) * i;
				moveCount++;
				break;
			}
			else { //board is unoccupied
				moveGen[moveCount][0] = position;
				moveGen[moveCount][1] = position + (10 + 1) * i;
				moveCount++;
			}
		}
	}
}
void queenMoves(int board[120], int position, int color) {
	
	rookMoves(board, position, color);
	bishopMoves(board, position, color);
}
void kingMoves(int board[120], int position, int color) {
	if (color == WHITE) {
		//   6 7 8
		//   5 K 1
  		//   4 3 2
		if (determineColor(board[position + 1]) != WHITE && legalSquare(position + 1) == true) { // position 1
			moveGen[moveCount][0] = position;
			moveGen[moveCount][1] = position + 1;
			moveCount++;
		}
		if (determineColor(board[position + 10 + 1]) != WHITE && legalSquare(position + 10 + 1) == true) { // position 2
			moveGen[moveCount][0] = position;
			moveGen[moveCount][1] = position + 10 + 1;
			moveCount++;
		}
		if (determineColor(board[position + 10]) != WHITE && legalSquare(position + 10) == true) { // position 3
			
			moveGen[moveCount][0] = position;
			moveGen[moveCount][1] = position + 10;
			moveCount++;
		}
		if (determineColor(board[position + 10 - 1]) != WHITE && legalSquare(position + 10 - 1) == true) { // position 4
			moveGen[moveCount][0] = position;
			moveGen[moveCount][1] = position + 10 - 1;
			moveCount++;
		}
		if (determineColor(board[position - 1]) != WHITE && legalSquare(position - 1) == true) { // position 5
			moveGen[moveCount][0] = position;
			moveGen[moveCount][1] = position - 1;
			moveCount++;
		}
		if (determineColor(board[position - 10 - 1]) != WHITE && legalSquare(position - 10 - 1) == true) { // position 6
			moveGen[moveCount][0] = position;
			moveGen[moveCount][1] = position - 10 - 1;
			moveCount++;
		}
		if (determineColor(board[position - 10]) != WHITE && legalSquare(position - 10) == true) { // position 7
			moveGen[moveCount][0] = position;
			moveGen[moveCount][1] = position - 10;
			moveCount++;
		}
		if (determineColor(board[position - 10 + 1]) != WHITE && legalSquare(position - 10 + 1) == true) { // position 8
			moveGen[moveCount][0] = position;
			moveGen[moveCount][1] = position - 10 + 1;
			moveCount++;
		}
	}
	if (color == BLACK) {
		//   6 7 8
		//   5 K 1
		//   4 3 2
		if (determineColor(board[position + 1]) != BLACK && legalSquare(position + 1) == true) { // position 1
			moveGen[moveCount][0] = position;
			moveGen[moveCount][1] = position + 1;
			moveCount++;
		}
		if (determineColor(board[position + 10 + 1]) != BLACK && legalSquare(position + 10 + 1) == true) { // position 2
			moveGen[moveCount][0] = position;
			moveGen[moveCount][1] = position + 10 + 1;
			moveCount++;
		}
		if (determineColor(board[position + 10]) != BLACK && legalSquare(position + 10) == true) { // position 3

			moveGen[moveCount][0] = position;
			moveGen[moveCount][1] = position + 10;
			moveCount++;
		}
		if (determineColor(board[position + 10 - 1]) != BLACK && legalSquare(position + 10 - 1) == true) { // position 4
			moveGen[moveCount][0] = position;
			moveGen[moveCount][1] = position + 10 - 1;
			moveCount++;
		}
		if (determineColor(board[position - 1]) != BLACK && legalSquare(position - 1) == true) { // position 5
			moveGen[moveCount][0] = position;
			moveGen[moveCount][1] = position - 1;
			moveCount++;
		}
		if (determineColor(board[position - 10 - 1]) != BLACK && legalSquare(position - 10 - 1) == true) { // position 6
			moveGen[moveCount][0] = position;
			moveGen[moveCount][1] = position - 10 - 1;
			moveCount++;
		}
		if (determineColor(board[position - 10]) != BLACK && legalSquare(position - 10) == true) { // position 7
			moveGen[moveCount][0] = position;
			moveGen[moveCount][1] = position - 10;
			moveCount++;
		}
		if (determineColor(board[position - 10 + 1]) != BLACK && legalSquare(position - 10 + 1) == true) { // position 8
			moveGen[moveCount][0] = position;
			moveGen[moveCount][1] = position - 10 + 1;
			moveCount++;
		}
	}
}
void printMoveList() {
	for (int i = 0; i < moveCount; i++) {
		printf("%d to %d", moveGen[i][0], moveGen[i][1]);
		printf("\n");
	}
}
int determineColor(int piece) {
	if (wP <= piece && piece <= wK) {
		return WHITE;
	}
	if (bP <= piece && piece <= bK) {
		return BLACK;
	}
	return NOCOLOR;
}
bool legalSquare(int position) {
	if (21 <= position && position <= 98 && position % 10 != 0 && position % 10 != 9) {
		return true;
	}
	return false;
}
int boardEvaluation(int board[120]) {
	int whiteAdvantage = 0;
	//add pieces' value
	for (int i = 0; i < 120; i++) {
		switch (board[i]) {
		case EMPTY:
			break;
		case ERROR:
			break;
		case wP:
			whiteAdvantage += PAWN_VALUE;
			whiteAdvantage += pawnTable[i];
			break;
		case wN:
			whiteAdvantage += KNIGHT_VALUE;
			whiteAdvantage += knightTable[i];
			break;
		case wB:
			whiteAdvantage += BISHOP_VALUE;
			whiteAdvantage += bishopTable[i];
			break;
		case wR:
			whiteAdvantage += ROOK_VALUE;
			whiteAdvantage += rookTable[i];
			break;
		case wQ:
			whiteAdvantage += QUEEN_VALUE;
			whiteAdvantage += queenTable[i];
			break;
		case wK:
			whiteAdvantage += KING_VALUE;
			whiteAdvantage += kingTable[i];
			break;
		case bP:
			whiteAdvantage -= PAWN_VALUE;
			whiteAdvantage -= pawnTable[reverseBoard(i)];
			break;
		case bN:
			whiteAdvantage -= KNIGHT_VALUE;
			whiteAdvantage -= knightTable[reverseBoard(i)];
			break;
		case bB:
			whiteAdvantage -= BISHOP_VALUE;
			whiteAdvantage -= bishopTable[reverseBoard(i)];
			break;
		case bR:
			whiteAdvantage -= ROOK_VALUE;
			whiteAdvantage -= rookTable[reverseBoard(i)];
			break;
		case bQ:
			whiteAdvantage -= QUEEN_VALUE;
			whiteAdvantage -= queenTable[reverseBoard(i)];
			break;
		case bK:
			whiteAdvantage -= KING_VALUE;
			whiteAdvantage -= kingTable[reverseBoard(i)];
			break;
		}
	}
	
	return whiteAdvantage;
}
int reverseBoard(int position) {
	//21 -> 91, 22 -> 92, ...
	//kings are both in E - column, so reversing must be done this way
	int newPosition;
	newPosition = (110 - (position - position%10)) + position%10;
	return newPosition;
}
void printBoardSimple(int board[120]) {

	for (int i = 2; i < 10; i++) {
		for (int j = 1; j < 9; j++) {
			switch (board[i * 10 + j]) {
			case wP:
				printf("P");
				break;
			case wR:
				printf("R");
				break;
			case wN:
				printf("N");
				break;
			case wB:
				printf("B");
				break;
			case wQ:
				printf("Q");
				break;
			case wK:
				printf("K");
				break;
			case bP:
				printf("p");
				break;
			case bR:
				printf("r");
				break;
			case bN:
				printf("n");
				break;
			case bB:
				printf("b");
				break;
			case bQ:
				printf("q");
				break;
			case bK:
				printf("k");
				break;
			case EMPTY:
				printf("_");
				break;
			}
			printf(" ");
		}
		printf("\n");
	}
}
char numberToPiece(int number) {
	char c;
	switch (number) {
	case wP:
		c = 'P';
		break;
	case wN:
		c = 'N';
		break;
	case wB:
		c = 'B';
		break;
	case wR:
		c = 'R';
		break;
	case wQ:
		c = 'Q';
		break;
	case wK:
		c = 'K';
		break;
	case bP:
		c = 'p';
		break;
	case bN:
		c = 'n';
		break;
	case bB:
		c = 'b';
		break;
	case bR:
		c = 'r';
		break;
	case bQ:
		c = 'q';
		break;
	case bK:
		c = 'k';
		break;
	case EMPTY:
		c = '_';
		break;
	case ERROR:
		c = 'X';
		break;
	}
	return c;
}

//TODO: moveGen does not work correctly - pawns
//TODO: RECURSION does not work correctly - "max" for negative (non-white) values need to be reexamined

/*RECURSION*/
int tempBoard[120]; //board being moved
int moveMadeList[MAX_PLY][2]; //list of moves made
int capturedPiece[MAX_PLY]; //list of pieces captured from move

/*RECURSION 2*/
int moveCountList[MAX_PLY]; //list of number of possible moves
int moveGenList[MAX_PLY][MAX_MOVES][2]; //list of possible moves

/*RECURSION 3*/
int maxScore = -9999;
int bestMoveList[MAX_PLY][2];
int currentMoveList[MAX_PLY][2];

void makeMove( int move[2], int currentPly ) { //move[0]: initial square, move[1]: terminal square
	//printf("MakeMove called - Ply: %d\n", currentPly);

	//save the piece captured (if any piece is captured)
	capturedPiece[currentPly] = tempBoard[move[1]];

	//makemove
	tempBoard[move[1]] = tempBoard[move[0]];
	tempBoard[move[0]] = EMPTY;
	
	//save the move made
	moveMadeList[currentPly][0] = move[0];
	moveMadeList[currentPly][1] = move[1];

	
}
void unmakeMove( int currentPly ) {
	//printf("UnmakeMove called - Ply: %d\n", currentPly);

	int move[2]; //the move to be unmade
	int piece; //the piece to be un-captured

	//get move & piece
	move[0] = moveMadeList[currentPly][0];
	move[1] = moveMadeList[currentPly][1];
	piece = capturedPiece[currentPly];

	//unmake move
	tempBoard[move[0]] = tempBoard[move[1]];
	tempBoard[move[1]] = piece;

	
}

int negaMax(int ply, int startColor) {
	int score = 1;
	int tempScore;

	//printf("Negamax called - Ply: %d\n", ply);
	//printBoard(tempBoard);

	if (ply == 0) {
		score = boardEvaluation(tempBoard);
		
		/*
		for (int i = 3; i > 0; i--) {
			printf("%d to %d, ", currentMoveList[i][0], currentMoveList[i][1]);
		}
		*/
		//printf("\n");
		//printf("Score = %d\n", score);
		return score;
	}

	//printf("moveGen called\n");
	moveGenerator(tempBoard, startColor);
	printf("\nMoveGenList\n");
	for (int i = 0; i < moveCount; i++) {

		moveGenList[ply][i][0] = moveGen[i][0];
		moveGenList[ply][i][1] = moveGen[i][1];

		printf("Ply %d ", ply);
		printf("%d to %d", moveGen[i][0], moveGen[i][1]);
		printf(" (%c to %c)", numberToPiece(tempBoard[moveGen[i][0]]), numberToPiece(tempBoard[moveGen[i][1]]));
		printf("\n");
	}
	printf("\n");
	moveCountList[ply] = moveCount;

	if (startColor == WHITE) {
		startColor = BLACK;  
	}
	else startColor = WHITE;

	for (int i = 0; i < moveCountList[ply]; i++) {
		makeMove(moveGenList[ply][i], ply);
		currentMoveList[ply][0] = moveGenList[ply][i][0];
		currentMoveList[ply][1] = moveGenList[ply][i][1];
		tempScore = negaMax(ply - 1, startColor);
		if (maxScore <= tempScore) { //renew max if the new score is higher & save where the max is
			maxScore = tempScore;
			//TODO: indexing moves

		}
		unmakeMove(ply);
	}
}

/*MAIN*/
void main() {

	time(&timer);
	int startTime = timer;

	setupBoard();
	/*
	//simpler setup for test
	for (int i = 1; i < 9; i++) {
		board[20 + i] = EMPTY;
		board[30 + i] = EMPTY;
		board[80 + i] = EMPTY;
		board[90 + i] = EMPTY;
	}
	board[E2] = wP;
	board[D7] = bP;
	*/


//	moveGenerator(board, WHITE);
//	for (int i = 0; i < moveCount; i++) {
//		printf("%d, %d\n", moveGen[i][0], moveGen[i][1]);
//	}

 	for (int i = 0; i < 120; i++) {
		tempBoard[i] = board[i];
	}

	negaMax(ENGINE_DEPTH, WHITE);
	
	printf("Max Score: %d\n", maxScore);

	time(&timer);
	printf("Calculation done in %d seconds\n", timer - startTime);

	// MAKEMOVE UNMAKEMOVE TEST
	/*
	for (int i = 0; i < 120; i++) {
		tempBoard[i] = board[i];
	}
	//printBoard(tempBoard);

	moveGenerator(WHITE);
	printf("%d to %d\n\n", moveGen[0][0], moveGen[0][1]);
	makeMove(moveGen[0], 1);
	//printBoard(tempBoard);
	unmakeMove(1);
	printBoardSimple(tempBoard);
	*/

	//printf("White has an advantage of %d centipawns.\n", boardEvaluation());

}