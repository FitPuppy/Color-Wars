#define WEB

#include <vector>
#include <ctime>
#include <iostream>

#ifdef WEB
#include <emscripten.h>
#endif

struct Cell {
	uint8_t value;
	uint8_t value_after;

	uint8_t playerId;
	uint8_t playerId_after;

	uint16_t protectedToRound;
	uint16_t lockedToRound;
};
struct Player {
	int16_t frozenToRound;

	uint8_t points;
	int32_t totalPoints;

	std::vector<uint8_t> cards;
	uint8_t choosedCard = -1;
};


uint8_t boardWidth = 5; //szerokoœæ planszy
uint8_t boardHeight = 5; //wysokoœæ planszy

uint8_t numberOfPlayers;
uint8_t numberOfCards;

uint8_t currentPlayer = 0;

std::vector<Player> players;
std::vector<std::vector<Cell>> board;

#ifdef WEB

void generateBoard() {
	players.clear();
	Player nullPlayer;

	nullPlayer.frozenToRound = 0;
	nullPlayer.points = 0;
	nullPlayer.totalPoints = 0;
	nullPlayer.choosedCard = -1;

	for (int i = 0; i < numberOfPlayers; i++)
	{
		players.push_back(nullPlayer);
	}

	board.clear();
	Cell nullCell;

	nullCell.value = 0;
	nullCell.value_after = 0;
	nullCell.playerId = 0;
	nullCell.playerId_after = 0;
	nullCell.protectedToRound = 0;
	nullCell.lockedToRound = 0;

	std::vector<Cell> v;
	
	for (int x = 0; x < boardWidth; x++)
	{
		for (int y = 0; y < boardHeight; y++)
		{
			v.push_back(nullCell);
		}
		board.push_back(v);
		v.clear();
	}
}
void updateBoard() {
	for (int y = 0; y < boardHeight; y++)
	{
		for (int x = 0; x < boardWidth; x++)
		{
			EM_ASM({ (setCell([$1],[$2],[$0], "black", 1); )}, rand() % 5, x, y);
		}
	}
}
extern "C" {
	EMSCRIPTEN_KEEPALIVE
	void cellClicked(int x, int y) {
		std::cout << "kliknieto    " << x << "     " << y << "\n";
	}
}
#endif
int main() {
	 
	srand(static_cast<unsigned int>(time(nullptr))); //random seed
	updateBoard();
} 