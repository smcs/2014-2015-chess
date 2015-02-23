#define NAN -1000
//chessprogramming wiki simplified evaluation piece square tables


int pawnTable[120] = {
	NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, 
	NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN,
	NAN,   0,   0,   0,   0,   0,   0,   0,   0, NAN,
	NAN,  50,  50,  50,  50,  50,  50,  50,  50, NAN,
	NAN,  10,  10,  20,  30,  30,  20,  10,  10, NAN,
	NAN,   5,   5,  10,  25,  25,  10,   5,   5, NAN,
	NAN,   0,   0,   0,  20,  20,   0,   0,   0, NAN,
	NAN,   5,  -5, -10,   0,   0, -10,  -5,   5, NAN,
	NAN,   5,  10,  10, -20, -20,  10,  10,   5, NAN,
	NAN,   0,   0,   0,   0,   0,   0,   0,   0, NAN,
	NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN,
	NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN
};

int knightTable[120] = {
	NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN,
	NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN,
	NAN, -50, -40, -30, -30, -30, -30, -40, -50, NAN,
	NAN, -40, -20,   0,   0,   0,   0, -20, -40, NAN,
	NAN, -30,   0,  10,  15,  15,  10,   0, -30, NAN,
	NAN, -30,   5,  15,  20,  20,  15,   5, -30, NAN,
	NAN, -30,   0,  15,  20,  20,  15,   0, -30, NAN,
	NAN, -30,   5,  10,  15,  15,  10,   5, -30, NAN,
	NAN, -40, -20,   0,   5,   5,   0, -20, -40, NAN,
	NAN, -50, -40, -30, -30, -30, -30, -40, -50, NAN,
	NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN,
	NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN
};
int bishopTable[120] = {
	NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN,
	NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN,
	NAN, -20, -10, -10, -10, -10, -10, -10, -10, NAN,
	NAN, -10,   0,   0,   0,   0,   0,   0, -10, NAN,
	NAN, -10,   0,   5,  10,  10,   5,   0, -10, NAN,
	NAN, -10,   5,   5,  10,  10,   5,   5, -10, NAN,
	NAN, -10,   0,  10,  10,  10,  10,   0, -10, NAN,
	NAN, -10,  10,  10,  10,  10,  10,  10, -10, NAN,
	NAN, -10,   5,   0,   0,   0,   0,   5, -10, NAN,
	NAN, -20, -10, -10, -10, -10, -10, -10, -10, NAN,
	NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN,
	NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN
};
int rookTable[120] = {
	NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN,
	NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN,
	NAN,   0,   0,   0,   0,   0,   0,   0,   0, NAN,
	NAN,   5,  10,  10,  10,  10,  10,  10,   5, NAN,
	NAN,  -5,   0,   0,   0,   0,   0,   0,  -5, NAN,
	NAN,  -5,   0,   0,   0,   0,   0,   0,  -5, NAN,
	NAN,  -5,   0,   0,   0,   0,   0,   0,  -5, NAN,
	NAN,  -5,   0,   0,   0,   0,   0,   0,  -5, NAN,
	NAN,  -5,   0,   0,   0,   0,   0,   0,  -5, NAN,
	NAN,   0,   0,   0,   5,   5,   0,   0,   0, NAN,
	NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN,
	NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN
};

int queenTable[120] = {
	NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN,
	NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN,
	NAN, -20, -10, -10,  -5,  -5, -10, -10, -20, NAN,
	NAN, -10,   0,   0,   0,   0,   0,   0,   0, NAN,
	NAN, -10,   0,   5,   5,   5,   5,   0,   0, NAN,
	NAN,  -5,   0,   5,   5,   5,   5,   0,   0, NAN,
	NAN,   0,   0,   5,   5,   5,   5,   0,   0, NAN,
	NAN,   0,   5,   5,   5,   5,   5,   0,   0, NAN,
	NAN,   0,   0,   5,   0,   0,   0,   0,   0, NAN,
	NAN, -20, -10, -10,  -5,  -5, -10, -10, -20, NAN,
	NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN,
	NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN
};

int kingTable[120] = {
	NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN,
	NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN,
	NAN, -30, -40, -40, -50, -50, -40, -40, -30, NAN,
	NAN, -30, -40, -40, -50, -50, -40, -40, -30, NAN,
	NAN, -30, -40, -40, -50, -50, -40, -40, -30, NAN,
	NAN, -30, -40, -40, -50, -50, -40, -40, -30, NAN,
	NAN, -20, -30, -30, -40, -40, -30, -30, -20, NAN,
	NAN, -10, -20, -20, -20, -20, -20, -20, -20, NAN,
	NAN,  20,  30,   0,   0,   0,   0,  20,  20, NAN,
	NAN,  20,  30,  10,   0,   0,  10,  30,  20, NAN,
	NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN,
	NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN
};