
/*DEFINITION*/
#define MAX_MOVES 269
#define MAX_PLY 10
#define ENGINE_DEPTH 4
#define MAX_MATCH_LENGTH 500
#define TURN_COUNT 10

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
int moveGen[MAX_MOVES][3]; // first column: starting square, second column: ending square, third column: move type
int moveCount = 0;
bool castling[4] = { true, }; //check if castling is available
time_t timer;
int totalMoveCount = 0;
bool whiteKingMoved = false;
bool blackKingMoved = false;
bool whiteKingsideRookMoved = false;
bool whiteQueensideRookMoved = false;
bool blackKingsideRookMoved = false;
bool blackQueensideRookMoved = false;
bool enPassantPossible = false; //this is true if the previous move was a two-rank move by pawn
int enPassantPosition = 0; //this is where the en Passant position currently is
//int whiteAttackPosition[120];
//int blackAttackPosition[120];
int finalMaxScore;
bool attackTable[120];
bool attackTableList[ENGINE_DEPTH+1][120];

int moveMadeList[MAX_MATCH_LENGTH][2];
int moveMadeCount=0;
/*RECURSION GLOBAL VARIABLES*/
int tempBoard[120]; //board being moved
int tempMoveMadeList[MAX_PLY][2]; //list of moves made
int capturedPiece[MAX_PLY]; //list of pieces captured from move
int moveCountList[MAX_PLY]; //list of number of possible moves
int moveGenList[MAX_PLY][MAX_MOVES][2]; //list of possible moves
int bestMoveList[MAX_PLY][2];
int currentMoveList[MAX_PLY][2];

/*FUNCTIONS*/
void setupBoard(); // initializes the board
void printBoard(int board[120]); // prints the current board
void moveGenerator(int board[120], int color); // generates and stores all possible moves
void pawnMoves(int board[120], int position, int color); // generates and stores all possible pawn moves
void knightMoves(int board[120], int position, int color); // generates and stores all possible knight moves
void rookMoves(int board[120], int position, int color); // generates and stores all possible rook moves
void bishopMoves(int board[120], int position, int color); // generates and stores all possible bishop moves
void queenMoves(int board[120], int position, int color); // generates and stores all possible queen moves
void kingMoves(int board[120], int position, int color); // generates and stores all possible king moves
void kingCastling(int board[120], int position, int color); //generates and stores all possible castling moves
int boardEvaluation(int board[120]); //evaluates current board situation using PCSQ tables and existing pieces
int determineColor(int piece); //returns the color of the piece (wK -> WHITE, bK -> BLACK, NOCOLOR)
bool legalSquare(int position); //asks if the position number is one on the 8x8 chessboard
int reverseBoard(int position); //used for PCSQ evaluation
void makeMove(int move[2], int currentPly);
void unmakeMove(int currentPly);
void printBoardSimple(int board[120]);
char numberToPiece(int number); //function that returns a symbol of piece the parameter number is
bool whiteKingsideCastling(int board[120]); //function that returns whether or not white can perform kingside castling 
bool whiteQueensideCastling(int board[120]); //function that returns whether or not white can perform queenside castling
bool blackKingsideCastling(int board[120]); //function that returns whether or not black can perform kingside castling 
bool blackQueensideCastling(int board[120]); //function that returns whether or not black can perform queenside castling
void kingCastling(int board[120], int position, int color); //generates and stores all possible castling moves
void addMove(int initPosition, int finalPosition, int moveType); // function that is called when a move should be added to moveGen
bool insufficientPieces(int board[120]);
void setupTestBoard(); //simpler setup for test
int numberToRank(int position);
char numberToFile(int position);
void clearAttackTable();
void updateAttackTable(int attackedPosition);
void copyAttackTable(int ply);

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
		moveGen[i][INITIAL] = 0;
		moveGen[i][FINAL] = 0;
		moveGen[i][MOVETYPE] = 0;
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
			kingCastling(board, i, WHITE);
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
			kingCastling(board, i, BLACK);
			break;
		}
	}
}
void pawnMoves(int board[120], int position, int color) {
	if (color == WHITE) {
		if (A2 <= position && position <= H2 && board[position - 20] == EMPTY && board[position - 10] == EMPTY) { //if it did not move yet
			addMove(position, position - 20, NOATTACK);
		}
		if (board[position - 10] == EMPTY) {
			addMove(position, position - 10, NOATTACK);
		}
		if (determineColor(board[position - 10 - 1]) == BLACK) {
			addMove(position, position - 10 - 1, NORMAL);
		}
		if (determineColor(board[position - 10 + 1]) == BLACK) {
			addMove(position, position - 10 + 1, NORMAL);
		}

		//enPassant capture
		if (enPassantPossible == true) {
			if (enPassantPosition == position - 1 || enPassantPosition == position + 1) {
				addMove(position, enPassantPosition - 10, ENPASSANT);
			}
		}
	}
	else if (color == BLACK) {
		if (A7 <= position && position <= H7 && board[position + 20] == EMPTY && board[position + 10] == EMPTY) { //if it did not move yet
			addMove(position, position + 20, NOATTACK);
		}
		if (board[position + 10] == EMPTY) {
			addMove(position, position + 10, NOATTACK);
		}
		if (determineColor(board[position + 10 - 1]) == WHITE) {
			addMove(position, position + 10 - 1, NORMAL);
		}
		if (determineColor(board[position + 10 + 1]) == WHITE) {
			addMove(position, position + 10 + 1, NORMAL);
		}
		if (enPassantPossible == true) {
			if (enPassantPosition == position - 1 || enPassantPosition == position + 1) {
				addMove(position, enPassantPosition + 10, ENPASSANT);
			}
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
			addMove(position, position - 20 + 1, NORMAL);
		}
		if (determineColor(board[position - 10 + 2]) != WHITE && legalSquare(position - 10 + 2) == true) { // position 2
			addMove(position, position - 10 + 2, NORMAL);
		}
		if (determineColor(board[position + 10 + 2]) != WHITE && legalSquare(position + 10 + 2) == true) { // position 3
			addMove(position, position + 10 + 2, NORMAL);
		}
		if (determineColor(board[position + 20 + 1]) != WHITE && legalSquare(position + 20 + 1) == true) { // position 4
			addMove(position, position + 20 + 1, NORMAL);
		}
		if (determineColor(board[position + 20 - 1]) != WHITE && legalSquare(position + 20 - 1) == true) { // position 5
			addMove(position, position + 20 - 1, NORMAL);
		}
		if (determineColor(board[position + 10 - 2]) != WHITE && legalSquare(position + 10 - 2) == true) { // position 6
			addMove(position, position + 10 - 2, NORMAL);
		}
		if (determineColor(board[position - 10 - 2]) != WHITE && legalSquare(position - 10 - 2) == true) { // position 7
			addMove(position, position - 10 - 2, NORMAL);
		}
		if (determineColor(board[position - 20 - 1]) != WHITE && legalSquare(position - 20 - 1) == true) { // position 8
			addMove(position, position - 20 - 1, NORMAL);
		}
	}
	if (color == BLACK) {
		if (determineColor(board[position - 20 + 1]) != BLACK && legalSquare(position - 20 + 1) == true) { // position 1
			addMove(position, position - 20 + 1, NORMAL);
		}
		if (determineColor(board[position - 10 + 2]) != BLACK && legalSquare(position - 10 + 2) == true) { // position 2
			addMove(position, position - 10 + 2, NORMAL);
		}
		if (determineColor(board[position + 10 + 2]) != BLACK && legalSquare(position + 10 + 2) == true) { // position 3
			addMove(position, position + 10 + 2, NORMAL);
		}
		if (determineColor(board[position + 20 + 1]) != BLACK && legalSquare(position + 20 + 1) == true) { // position 4
			addMove(position, position + 20 + 1, NORMAL);
		}
		if (determineColor(board[position + 20 - 1]) != BLACK && legalSquare(position + 20 - 1) == true) { // position 5
			addMove(position, position + 20 - 1, NORMAL);
		}
		if (determineColor(board[position + 10 - 2]) != BLACK && legalSquare(position + 10 - 2) == true) { // position 6
			addMove(position, position + 10 - 2, NORMAL);
		}
		if (determineColor(board[position - 10 - 2]) != BLACK && legalSquare(position - 10 - 2) == true) { // position 7
			addMove(position, position - 10 - 2, NORMAL);
		}
		if (determineColor(board[position - 20 - 1]) != BLACK && legalSquare(position - 20 - 1) == true) { // position 8
			addMove(position, position - 20 - 1, NORMAL);
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
				addMove(position, position + 10 * i, NORMAL);
				break;
			}
			else { //board is unoccupied
				addMove(position, position + 10 * i, NORMAL);
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
				addMove(position, position - 10 * i, NORMAL);
				break;
			}
			else { //board is unoccupied
				addMove(position, position - 10 * i, NORMAL);
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
				addMove(position, position + i, NORMAL);
				break;
			}
			else { //board is unoccupied
				addMove(position, position + i, NORMAL);
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
				addMove(position, position - i, NORMAL);
				break;
			}
			else { //board is unoccupied
				addMove(position, position - i, NORMAL);
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
				addMove(position, position + 10 * i, NORMAL);
				break;
			}
			else { //board is unoccupied
				addMove(position, position + 10 * i, NORMAL);
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
				addMove(position, position - 10 * i, NORMAL);
				break;
			}
			else { //board is unoccupied
				addMove(position, position - 10 * i, NORMAL);
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
				addMove(position, position + i, NORMAL);
				break;
			}
			else { //board is unoccupied
				addMove(position, position + i, NORMAL);
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
				addMove(position, position - i, NORMAL);
				break;
			}
			else { //board is unoccupied
				addMove(position, position - i, NORMAL);
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
				addMove(position, position + (-10 + 1) * i, NORMAL);
				break;
			}
			else { //board is unoccupied
				addMove(position, position + (-10 + 1) * i, NORMAL);
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
				addMove(position, position + (-10 - 1) * i, NORMAL);
				break;
			}
			else { //board is unoccupied
				addMove(position, position + (-10 - 1) * i, NORMAL);
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
				addMove(position, position + (10 - 1) * i, NORMAL);
				break;
			}
			else { //board is unoccupied
				addMove(position, position + (10 - 1) * i, NORMAL);
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
				addMove(position, position + (10 + 1) * i, NORMAL);
				break;
			}
			else { //board is unoccupied
				addMove(position, position + (10 + 1) * i, NORMAL);
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
				addMove(position, position + (-10 + 1) * i, NORMAL);
				break;
			}
			else { //board is unoccupied
				addMove(position, position + (-10 + 1) * i, NORMAL);
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
				addMove(position, position + (-10 - 1) * i, NORMAL);
				break;
			}
			else { //board is unoccupied
				addMove(position, position + (-10 - 1) * i, NORMAL);
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
				addMove(position, position + (10 - 1) * i, NORMAL);
				break;
			}
			else { //board is unoccupied
				addMove(position, position + (10 - 1) * i, NORMAL);
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
				addMove(position, position + (10 + 1) * i, NORMAL);
				break;
			}
			else { //board is unoccupied
				addMove(position, position + (10 + 1) * i, NORMAL);
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
			addMove(position, position + 1, NORMAL);
		}
		if (determineColor(board[position + 10 + 1]) != WHITE && legalSquare(position + 10 + 1) == true) { // position 2
			addMove(position, position + 10 + 1, NORMAL);
		}
		if (determineColor(board[position + 10]) != WHITE && legalSquare(position + 10) == true) { // position 3
			addMove(position, position + 10, NORMAL);
		}
		if (determineColor(board[position + 10 - 1]) != WHITE && legalSquare(position + 10 - 1) == true) { // position 4
			addMove(position, position + 10 - 1, NORMAL);
		}
		if (determineColor(board[position - 1]) != WHITE && legalSquare(position - 1) == true) { // position 5
			addMove(position, position - 1, NORMAL);
		}
		if (determineColor(board[position - 10 - 1]) != WHITE && legalSquare(position - 10 - 1) == true) { // position 6
			addMove(position, position - 10 - 1, NORMAL);
		}
		if (determineColor(board[position - 10]) != WHITE && legalSquare(position - 10) == true) { // position 7
			addMove(position, position - 10, NORMAL);
		}
		if (determineColor(board[position - 10 + 1]) != WHITE && legalSquare(position - 10 + 1) == true) { // position 8
			addMove(position, position - 10 + 1, NORMAL);
		}
	}
	if (color == BLACK) {
		//   6 7 8
		//   5 K 1
		//   4 3 2
		if (determineColor(board[position + 1]) != BLACK && legalSquare(position + 1) == true) { // position 1
			addMove(position, position + 1, NORMAL);
		}
		if (determineColor(board[position + 10 + 1]) != BLACK && legalSquare(position + 10 + 1) == true) { // position 2
			addMove(position, position + 10 + 1, NORMAL);
		}
		if (determineColor(board[position + 10]) != BLACK && legalSquare(position + 10) == true) { // position 3
			addMove(position, position + 10, NORMAL);
		}
		if (determineColor(board[position + 10 - 1]) != BLACK && legalSquare(position + 10 - 1) == true) { // position 4
			addMove(position, position + 10 - 1, NORMAL);
		}
		if (determineColor(board[position - 1]) != BLACK && legalSquare(position - 1) == true) { // position 5
			addMove(position, position - 1, NORMAL);
		}
		if (determineColor(board[position - 10 - 1]) != BLACK && legalSquare(position - 10 - 1) == true) { // position 6
			addMove(position, position - 10 - 1, NORMAL);
		}
		if (determineColor(board[position - 10]) != BLACK && legalSquare(position - 10) == true) { // position 7
			addMove(position, position - 10, NORMAL);
		}
		if (determineColor(board[position - 10 + 1]) != BLACK && legalSquare(position - 10 + 1) == true) { // position 8
			addMove(position, position - 10 + 1, NORMAL);
		}
	}
}
bool whiteKingsideCastling(int board[120]) {
	if (whiteKingMoved == true) { // if the king has moved, castling cannot happen
		return false;
	}
	if (whiteKingsideRookMoved == true) { // if the kingside rook has moved, kingside castling cannot happen
		return false;
	}

	if (board[F1] != EMPTY || board[G1] != EMPTY) { // if space between king and rook is occupied, castling cannot happen
		return false;
	}

	//TODO: if king gets itself on check during castling, castling cannot happen
	return true;
}
bool whiteQueensideCastling(int board[120]) {
	if (whiteKingMoved == true) { // if the king has moved, castling cannot happen
		return false;
	}
	if (whiteQueensideRookMoved == true) { // if the queenside rook has moved, queenside castling cannot happen
		return false;
	}

	if (board[B1] != EMPTY || board[C1] != EMPTY || board[D1] != EMPTY) { // if space between king and rook is occupied, castling cannot happen
		return false;
	}

	//TODO: if king gets itself on check during castling, castling cannot happen
	return true;
}
bool blackKingsideCastling(int board[120]) {
	if (blackKingMoved == true) { // if the king has moved, castling cannot happen
		return false;
	}
	if (blackKingsideRookMoved == true) { // if the kingside rook has moved, kingside castling cannot happen
		return false;
	}

	if (board[F8] != EMPTY || board[G8] != EMPTY) { // if space between king and rook is occupied, castling cannot happen
		return false;
	}

	//TODO: if king gets itself on check during castling, castling cannot happen
	return true;
}
bool blackQueensideCastling(int board[120]) {
	if (blackKingMoved == true) { // if the king has moved, castling cannot happen
		return false;
	}
	if (blackQueensideRookMoved == true) { // if the queenside rook has moved, queenside castling cannot happen
		return false;
	}

	if (board[B8] != EMPTY || board[C8] != EMPTY || board[D8] != EMPTY) { // if space between king and rook is occupied, castling cannot happen
		return false;
	}

	//TODO: if king gets itself on check during castling, castling cannot happen
	return true;
}
void setupTestBoard(){
	//simpler setup for test
	for (int i = 0; i < 120; i++) {
		board[i] = ERROR;
	}
	for (int i = 2; i < 10; i++) {
		for (int j = 1; j < 9; j++) {
			board[i * 10 + j] = EMPTY;
		}
	}

	board[E1] = wK;
	board[A1] = wR;
	board[G1] = wN;
	board[H1] = wR;
	board[E8] = bK;
	board[A8] = bR;
	board[G8] = bN;
	board[H8] = bR;	
}
void printMoveGen(int movePerLine) {
	printf("-----MOVEGEN-----\n");
	for (int i = 0; i < moveCount; i++) {
		printf("%2d: %c%d to %c%d  ", i + 1, numberToFile(moveGen[i][INITIAL]), numberToRank(moveGen[i][INITIAL]),
											 numberToFile(moveGen[i][FINAL]), numberToRank(moveGen[i][FINAL]));
		if (i % movePerLine == movePerLine - 1) {
			printf("\n");
		}
	}
	printf("\n");
}
void printCurrentMoveList(int ply, int movePerLine) {
	printf("--CURRENT MOVES--\n");
	for (int i = ENGINE_DEPTH; i > ply; i--) {
		printf("%c%d to %c%d  ", numberToFile(currentMoveList[i][INITIAL]), numberToRank(currentMoveList[i][INITIAL]), 
			                     numberToFile(currentMoveList[i][FINAL]), numberToRank(currentMoveList[i][FINAL]));
		if (i % movePerLine == movePerLine - 1) {
			printf("\n");
		}
	}
}
void printFileRankBoard(int board[120]) {
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
			case ERROR:
				printf("X");
				break;

			}
			printf(" ");
		}
		printf("| %d \n", 10-i);
	}
	printf("--------------- \n");
	printf("A B C D E F G H \n");
}
int numberToRank(int position) {
	return 10 - position / 10;
}
char numberToFile(int position) {
	char file;
	file = (char)position % 10 - 1 + 'A';
	return file;
}
void kingCastling(int board[120], int position, int color) {
	if (color == WHITE) {
		if (whiteKingsideCastling(board) == true) {
			addMove(E1, G1, CASTLING);
		}
		if (whiteQueensideCastling(board) == true) {
			addMove(E1, C1, CASTLING);
		}
	}
	if (color == BLACK) {
		if (blackKingsideCastling(board) == true) {
			addMove(E8, G8, CASTLING);
		}
		if (blackQueensideCastling(board) == true) {
			addMove(E8, C8, CASTLING);
		}
	}
}
void printMoveList() {
	for (int i = 0; i < moveCount; i++) {
		printf("%d to %d", moveGen[i][INITIAL], moveGen[i][FINAL]);
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
void addMove(int initPosition, int finalPosition, int moveType) {
	moveGen[moveCount][0] = initPosition;
	moveGen[moveCount][1] = finalPosition;
	moveGen[moveCount][2] = moveType;
	moveCount++;
	if (moveType == NORMAL) {
		updateAttackTable(finalPosition);
	}
}
bool insufficientPieces(int board[120]) { //only when KvK, K+NvK or K+BvK
	int count = 0;
	for (int i = 0; i < 120; i++) {
		if (board[i] == wQ || board[i] == bQ) {
			return false;
		}
		if (board[i] == wR || board[i] == wR) {
			return false;
		}
		if (board[i] == wN || board[i] == bN || board[i] == wB || board[i] == bB) {
			count++;
		}
		if (count >= 2) {
			return false;
		}
	}
	return true;
}
void makeMove( int move[2], int currentPly ) { 
	//move[0]: initial square, move[1]: terminal square
	//printf("MakeMove called - Ply: %d\n", currentPly);

	//save the piece captured (if any piece is captured)
	capturedPiece[currentPly] = tempBoard[move[FINAL]];

	//makemove
	tempBoard[move[FINAL]] = tempBoard[move[INITIAL]];
	tempBoard[move[INITIAL]] = EMPTY;
	
	//save the move made
	tempMoveMadeList[currentPly][INITIAL] = move[INITIAL];
	tempMoveMadeList[currentPly][FINAL] = move[FINAL];

	
}
void unmakeMove( int currentPly ) {
	//printf("UnmakeMove called - Ply: %d\n", currentPly);

	int move[2]; //the move to be unmade
	int piece; //the piece to be un-captured

	//get move & piece
	move[INITIAL] = tempMoveMadeList[currentPly][INITIAL];
	move[FINAL] = tempMoveMadeList[currentPly][FINAL];
	piece = capturedPiece[currentPly];

	//unmake move
	tempBoard[move[INITIAL]] = tempBoard[move[FINAL]];
	tempBoard[move[FINAL]] = piece;

	
}

bool currentCheckDetection(int detectColor) {
	for (int i = 0; i < moveCount; i++) {
		if (moveGen[i][FINAL] == bK && detectColor == WHITE){
			return true;
		}
		if (moveGen[i][FINAL] == wK && detectColor == BLACK){
			return true;
		}
	}
	return false;
}

void clearAttackTable() {
	for (int i = 0; i < 120; i++) {
		attackTable[i] = false;
	}
}
void clearAttackTable(int ply) {
	for (int i = 0; i < 120; i++) {
		attackTableList[ply][i] = false;
	}
}

void updateAttackTable(int attackedPosition) {
	attackTable[attackedPosition] = true;
}
void copyAttackTable(int ply) {
	for (int i = 0; i < 120; i++) {
		attackTableList[ply][i] = attackTable[i];
	}
}
void printAttackTable() {
	printf("---ATTACK TABLE---\n");
	for (int i = 2; i < 10; i++) {
		for (int j = 1; j < 9; j++){
			if (attackTable[i * 10 + j] == true)
				printf("X ");
			else
				printf("_ ");
		}
		printf("\n");
	}
	printf("\n");
}
void printAttackTable(int ply) {
	printf("---ATTACK TABLE---\n");
	for (int i = 2; i < 10; i++) {
		for (int j = 1; j < 9; j++){
			if (attackTableList[ply][i * 10 + j] == true)
				printf("X ");
			else
				printf("_ ");
		}
		printf("\n");
	}
	printf("\n");
}

void saveMove(int initialPos, int finalPos){
	moveMadeList[moveMadeCount][INITIAL] = initialPos;
	moveMadeList[moveMadeCount][FINAL] = finalPos;
	moveMadeCount++;
}
void printPreviousMoves() {
	printf("---ALL MOVES MADE---\n");
	for (int i = 0; i < moveMadeCount; i++) {
		printf("%d: %c%d to %c%d\n", i+1, numberToFile(moveMadeList[i][INITIAL]), numberToRank(moveMadeList[i][INITIAL]),
										  numberToFile(moveMadeList[i][FINAL]), numberToRank(moveMadeList[i][FINAL]));
	}
}

void directCheckDetection(int board[120], int position) {
	//Search if the last move directly checks the king
	switch (board[position]) {

	}
}
void discoveredCheckDetection(int board[120]) {
	//Search if the last move that moves away from a ray (rook, bishop, queen) and therefore creates a discovered check


}


int negaMax(int ply, int startColor) {
	int score;
	int tempScore;
	int maxScore = -9999; //reset max score

	if (ply == 0) {
		score = boardEvaluation(tempBoard);
		totalMoveCount++;
		if (startColor == WHITE)
			return score;
		else return (-1)*score;
	}
	
	clearAttackTable();
	moveGenerator(tempBoard, startColor);
	//printCurrentMoveList(ply, 3);
	//printMoveGen(5);

	for (int i = 0; i < moveCount; i++) {
		moveGenList[ply][i][INITIAL] = moveGen[i][INITIAL];
		moveGenList[ply][i][FINAL] = moveGen[i][FINAL];
	}
	moveCountList[ply] = moveCount;

	clearAttackTable(ply);
	copyAttackTable(ply);
	//printAttackTable();

	for (int i = 0; i < moveCountList[ply]; i++) {
		makeMove(moveGenList[ply][i], ply);
		currentMoveList[ply][INITIAL] = moveGenList[ply][i][INITIAL];
		currentMoveList[ply][FINAL] = moveGenList[ply][i][FINAL];
		tempScore = -1*negaMax(ply - 1, !startColor);
		
		unmakeMove(ply);

	//	if (ply == ENGINE_DEPTH) {
	//		printf("Max Score: %d\n", maxScore);
	//		printf("Temp SCore: %d\n", tempScore);
	//	}	
		if (maxScore <= tempScore) { //renew max if the new score is higher & save where the max is
			maxScore = tempScore;
			//TODO: indexing moves
			bestMoveList[ply][INITIAL] = moveGenList[ply][i][INITIAL];
			bestMoveList[ply][FINAL] = moveGenList[ply][i][FINAL];
		}		
	}

	return maxScore;
}

/*MAIN*/
void main() {

	//FILE * out = fopen("output.txt", "w");

	time(&timer);
	int startTime = timer;

	setupBoard();
	//setupTestBoard();
	printBoardSimple(board);

	int cnt = 1;
	while (cnt <= TURN_COUNT) {
		cnt++;

		//Clear all arrays
		for (int i = 0; i < moveCount; i++){
			moveGen[i][INITIAL] = 0;
			moveGen[i][FINAL] = 0;
			for (int j = ENGINE_DEPTH; j > 0; j--) {
				moveGenList[j][i][INITIAL] = 0;
				moveGenList[j][i][FINAL] = 0;
			}
		}
		for (int i = ENGINE_DEPTH; i > 0; i--) {
			bestMoveList[i][INITIAL] = 0;
			bestMoveList[i][FINAL] = 0;
		}

		//Mark when the calculation started
		startTime = timer;

		//Copy the board
		for (int i = 0; i < 120; i++) {
			tempBoard[i] = board[i];
		}

		if (cnt % 2 == 0){
			finalMaxScore = negaMax(ENGINE_DEPTH, WHITE);
		}
		else {
			finalMaxScore = negaMax(ENGINE_DEPTH, BLACK);
		}
		
		//Print the maximum score
		printf("Max Score: %d\n", finalMaxScore);
		/*
		for (int i = ENGINE_DEPTH; i > 0; i--){
			printf("Best Move (Ply %d): %c%d %c%d\n", i, numberToFile(bestMoveList[i][0]), numberToRank(bestMoveList[i][0]),
				numberToFile(bestMoveList[i][1]), numberToRank(bestMoveList[i][1]));
		}
		*/
		//printf("%d Nondistinct Positions Detected\n", totalMoveCount);
		time(&timer);
		printf("Calculation done in %2d seconds\n", timer - startTime);
		//printAttackTable(ENGINE_DEPTH);

		
		//Make the best move on the board
		board[bestMoveList[ENGINE_DEPTH][FINAL]] = board[bestMoveList[ENGINE_DEPTH][INITIAL]];
		board[bestMoveList[ENGINE_DEPTH][INITIAL]] = EMPTY;
		
		//Save the move made
		saveMove(bestMoveList[ENGINE_DEPTH][0], bestMoveList[ENGINE_DEPTH][1]);

		//Print the board after the move was made
		printBoardSimple(board);

	}
	printPreviousMoves();
}