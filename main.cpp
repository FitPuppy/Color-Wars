#include <iostream>
#include <cmath>
#include <map>
#include "raylib.h"
#include <string>
#include <vector>

//#include <emscripten.h>
//#define EMSCRIPTEN__
#define SIZE_10
#define DEFENSE_CARD 1
#define LIGHTNING_CARD 3
#define PLUS_TWO_CARD 2
#define PLACE_CARD 4
#define FIRE_CARD 5
#define METEOR_CARD 6
#define WALL_CARD 7
#define REVERSE_CARD 8
#define FREEZE_CARD 9
#define BOMB_CARD 10
using namespace std;
bool touchOrMouse = true;
double inputLimit;
double lasttime = 0, lasttime_alert = -8, lasttime_thunder = -1, lasttime_fire = -3, lasttime_meteor = -3;
uint8_t player = 1;
std::string alert_string;
int thunderx, thundery, xfire, yfire;
#ifdef SIZE_5
// Sta³e
int numofplayers = 1
const int CELL_SIZE = 32 * 4;//128
const int CELL_PADDING = 32;
const int CELL_P_SIZE = 16;
constexpr int CSIZE = CELL_SIZE + CELL_PADDING;
const int BOARD_WIDTH = 5; // Szerokoœæ planszy 
const int BOARD_HEIGHT = 5; // Wysokoœæ planszy 
const float ANIMATION_DURATION = 0.250f; // Czas trwania animacji w sekundach
#endif
#ifdef SIZE_10
// Sta³e
const uint8_t numberOfCards = 9;
const int8_t fire[9][2] = { {-1,-1},{0,-1},{1,-1},{-1,0},{0,0},{1,0},{-1,1},{0,1},{1,1} };
std::vector<std::pair<int,int>> meteor;
int numofplayers = 2;
int playerFrozen = 25;
int playerFrozenToRound = -1;
int BOARD_WIDTH = 7; // Szerokoœæ planszy 
int BOARD_HEIGHT = 7; // Wysokoœæ planszy 
int CELL_SIZE = (((1000 / BOARD_HEIGHT) / 5) * 4) - 2;//128;//64 86
int CELL_PADDING = CELL_SIZE / 4;
int CELL_P_SIZE = CELL_PADDING;
int CSIZE = CELL_SIZE + CELL_PADDING;
float ANIMATION_DURATION = 0.25f; // Czas trwania animacji w sekundach
#endif
//const Color pcolors[16] = { Color(0xFFFFFFFF), Color(0xFF0000FF), Color(0x0000FFFF)};
//const Color wcolors[16] = { Color(0xFFFFFFFF), Color(0xFF6161FF), Color(0x6161FFFF) };
//                          white                  red                blue
Color pcolors[16] = { GetColor(0xFFFFFFFF), GetColor(0xFF0000FF),  GetColor(0x0c52f5),GetColor(0x0CC29AFF), GetColor(0xFFCC00FF) };
Color wcolors[16] = { GetColor(0xFFFFFFFF), GetColor(0xFF6161FF),  GetColor(0x5a7ccc), GetColor(0x3ab54cFF), GetColor(0xffe066FF) };
//                          white                  red                blue               green

std::string names[16] = { "" , "Ania",         "Tomasz",            "Antek",    "Ania"};
uint8_t cards[16][4] = { 0 };
bool choosedCards[16][4] = { false };
int points[16] = { 0 };
int total_points[16] = { 0 };
Texture cardsTexture, thunderTexture, fireTexture, meteorTexture;
struct cell {
    uint8_t v;
    uint8_t p : 4;
    uint16_t protectedTo = 0;
};

// Tablice do przechowywania wartoœci komórek
cell board[40][40];    // Plansza pocz¹tkowa
cell aboard[40][40];   // Plansza po interakcji

// Struktura do przechowywania animacji dla ka¿dej komórki
struct CellAnimation {
    float elapsedTime = 0.0f;
    bool animating = false;
    int startValue = 0;
    int endValue = 0;
};

CellAnimation animations[40][40]; // Tablica animacji
int rounds = 0;

void Check44() {
    for (int y = 0; y < BOARD_HEIGHT; y++)
    {
        for (int x = 0; x < BOARD_WIDTH; x++)
        {
            if (board[x][y].v >= 4) {
                board[x][y].v -= 4;
                int p = board[x][y].p;
                if (board[x][y].v <= 0) {
                    aboard[x][y].p = 0;
                }
                aboard[x][y].v -= 4;
                int _x = x, _y = x;

                _x = x - 1;
                _y = y;
                if (_x < 0) { _x = BOARD_WIDTH - 1; }
                if ((board[_x][_y].protectedTo < rounds || (board[_x][_y].p == p) )&&( !(board[_x][_y].protectedTo > rounds && board[_x][_y].p == 0))) {
                    aboard[_x][_y].v++;
                    aboard[_x][_y].p = p;
                }
                _x = x + 1;
                _y = y;
                if (_x > (BOARD_WIDTH - 1)) { _x = 0; }
                if ((board[_x][_y].protectedTo < rounds || (board[_x][_y].p == p) && !(board[_x][_y].protectedTo > rounds && board[_x][_y].p == 0))) {
                    aboard[_x][_y].v++;
                    aboard[_x][_y].p = p;
                }
                _x = x;
                _y = y - 1;
                if (_y < 0) { _y = BOARD_HEIGHT - 1; }
                if ((board[_x][_y].protectedTo < rounds || (board[_x][_y].p == p)) && (!(board[_x][_y].protectedTo > rounds && board[_x][_y].p == 0))) {
                    aboard[_x][_y].v++;
                    aboard[_x][_y].p = p;
                }
                _x = x;
                _y = y + 1;
                if (_y > (BOARD_WIDTH - 1)) { _y = 0; }
                if ((board[_x][_y].protectedTo < rounds || (board[_x][_y].p == p) && !(board[_x][_y].protectedTo > rounds && board[_x][_y].p == 0))) {
                    aboard[_x][_y].v++;
                    aboard[_x][_y].p = p;
                }
            }
        }
    }
}
void skipDeadPlayer() {
    bool is = false;
    for (int y = 0; y < BOARD_HEIGHT; y++)
    {
        for (int x = 0; x < BOARD_WIDTH; x++)
        {
            if (board[x][y].p == player) { is = true; break; }
        }
    }
    if (playerFrozen == player) {
        is = true;
        if (playerFrozenToRound <= rounds) {
            playerFrozen = 25;
        }
    }
    if (!is) {
        player++;
        skipDeadPlayer();
    }
}
void updateAnimations(float deltaTime) {
    //Check44();
    for (int x = 0; x < BOARD_WIDTH; ++x) {
        for (int y = 0; y < BOARD_HEIGHT; ++y) {
            if (board[x][y].v != aboard[x][y].v && !animations[x][y].animating) {
                animations[x][y].animating = true;
                animations[x][y].startValue = board[x][y].v;
                animations[x][y].endValue = aboard[x][y].v;
                animations[x][y].elapsedTime = 0.0f;
            }

            if (animations[x][y].animating) {
                animations[x][y].elapsedTime += deltaTime;
                if (animations[x][y].elapsedTime >= ANIMATION_DURATION) {
                    board[x][y] = aboard[x][y];
                    Check44();
                    animations[x][y].animating = false;
                }
            }
        }
    }
}

void drawBoard() {
    //updateAnimations(1);
    for (int x = 0; x < BOARD_WIDTH; ++x) {
        for (int y = 0; y < BOARD_HEIGHT; ++y) {
            // Pozycja komórki
            int posX = x * (CELL_SIZE + CELL_PADDING);
            int posY = y * (CELL_SIZE + CELL_PADDING);

            Color colo;
            Color color_point = wcolors[board[x][y].p];
            // G³ówna komórka (szary kwadrat)
            if (board[x][y].protectedTo > rounds) {
                if (!(board[x][y].v != 0 || board[x][y].p == 0)) {
                    colo = GetColor(0x3a3d3d77);
                }
                else {
                    colo = GetColor(0x00000000);
                }
            }
            if (board[x][y].p == playerFrozen) {
                color_point = pcolors[player];
                colo = GetColor(0xb3f0ff);
            }
            else {
                colo = WHITE;
            }
            DrawRectangle(posX + CELL_PADDING, posY + CELL_PADDING, CELL_SIZE, CELL_SIZE, colo);
            // Animacja rozmiaru czerwonego kwadratu
            float progress = std::min(animations[x][y].elapsedTime / ANIMATION_DURATION, 1.0f);
            float currentValue = animations[x][y].animating
                ? animations[x][y].startValue + static_cast<float>(progress * (animations[x][y].endValue - animations[x][y].startValue))
                : board[x][y].v;

            // Wewnêtrzny czerwony kwadrat
            float redSquareSize = currentValue * CELL_P_SIZE;
            DrawRectangle(posX + CELL_PADDING + (CELL_SIZE - redSquareSize) / 2, posY + CELL_PADDING + (CELL_SIZE - redSquareSize) / 2, redSquareSize, redSquareSize, color_point);
            //std::cout << (char)(board[x][y].p + '0');
            //DrawRectangle(posX + CELL_PADDING, posY + CELL_PADDING, redSquareSize, 20, GREEN);
        }
    }
}
void displayText() {

    DrawText(std::to_string(rounds).c_str(), 1050, 100, 48, WHITE);

    for (int i = 1; i < numofplayers + 1; i++)
    {
        DrawText(std::string(names[i]).c_str(), 1050, 200 + (50 * i), 48, WHITE);
    }

    for (int i = 1; i < numofplayers + 1; i++)
    {
        DrawText(std::to_string(points[i]).c_str(), 1250, 200 + (50 * i), 48, WHITE);
    }

    for (int i = 1; i < numofplayers + 1; i++)
    {
        DrawText(std::to_string(total_points[i]).c_str(), 1400, 200 + (50 * i), 48, WHITE);
    }
}
void Countpoints() {
    for (int i = 0; i < numofplayers + 1; i++)
    {
        total_points[i] += points[i];
        points[i] = 0;
    }
    for (int y = 0; y < BOARD_HEIGHT; y++)
    {
        for (int x = 0; x < BOARD_WIDTH; x++)
        {
            points[board[x][y].p] += board[x][y].v;
        }
    }
}
void GenerateBoard() {
    rounds = 0;
    for (int i  = 0; i < 16; i++)
    {
        cards[i][0] = 0;
        cards[i][1] = 0;
        cards[i][2] = 0;
        points[i] = 0;
        total_points[i] = 0;
    }
    for (int y = 0; y < BOARD_HEIGHT; y++)
    {
        for ( int x = 0; x < BOARD_WIDTH; x++)
        {
            board[x][y] = { 0,0 };
            aboard[x][y] = { 0,0 };
            animations[x][y] = { 0,0,0,0 };
        }
    }
    player = rand() % numofplayers + 1;
    if (player == 1) {
        player++;
    }
    for (int i = 1; i < numofplayers + 1; i++)
    {
        int rx = rand() % (BOARD_WIDTH - 1), ry = rand() % (BOARD_HEIGHT - 1);
        
        aboard[rx][ry].p = i;
        aboard[rx][ry].v = 3;
        cards[i][1] = (rand() % numberOfCards)  + 1;
        cards[i][2] = (rand() % numberOfCards) + 1;
        cards[i][0] = (rand() % numberOfCards) + 1;
        cards[i][3] = (rand() % numberOfCards) +1;
    }
    cards[player][0] = (rand() % numberOfCards) + 2;
}
void DrawCards(int _x, int _y) {
    for (int i = 0; i < 4; i++)
    {
        if (cards[player][i] != 0) {
            int up = 0;
            if (choosedCards[player][i]) {
                up = -150;
            }
            DrawTexturePro(cardsTexture, { 22.0f * cards[player][i],0,22.0f,32.0f}, {(float)_x + (150 * i),(float)_y + up,88 * 2,128 * 2}, {88,128}, 330 + (i * 20), WHITE);
        }
    }
}
#ifdef EMSCRIPTEN__
EM_JS(int, returnPlayerCount, (), {
    return playerCountInput.value;
});
EM_JS(int, returnBoardSize, (), {
    return boardSizeInput.value;
});
EM_JS(int, returnnames, (), {
    return playerCountInput.value;
});
void UpdateDataFromSite() {
    numofplayers = returnPlayerCount();
    BOARD_HEIGHT = emscripten_run_script_int("boardSizeInput.value");
    BOARD_WIDTH = BOARD_HEIGHT;
    for (int i = 1; i < numofplayers + 1; i++)
    {
        names[i] = emscripten_run_script_string("document.getElementById('text" + std::to_string(i) + "').value");
        pcolors[i] = GetColor(emscripten_run_script_int("document.getElementById('color" + std::to_string(i) + "').value"));
        wcolors[i] = GetColor(emscripten_run_script_int("document.getElementById('color_" + std::to_string(i) + "').value"));
    }
    GenerateBoard();
}
#endif
void ShowAlert(std::string alert) {
    lasttime_alert = GetTime();
    alert_string = alert;
}
void ShowThunder(int x, int y) {
    lasttime_thunder = GetTime();
    thunderx = x;
    thundery = y;
}
void ShowFire(int x, int y) {
    lasttime_fire = GetTime();
    xfire = x;
    yfire = y;
}
void ShowMeteor() {
    lasttime_meteor = GetTime();
}
int main() {
    srand(time(0));
    InitWindow(1800, 1000, "Color Wars");
    LoadFont("dpcomic.ttf");
    Image image = LoadImage("graphics/thunder.png");
    thunderTexture = LoadTextureFromImage(image);
    image = LoadImage("graphics/karty.png");
    cardsTexture = LoadTextureFromImage(image);
    image = LoadImage("graphics/fire.png");
    fireTexture = LoadTextureFromImage(image);
    image = LoadImage("graphics/meteor.png");
    meteorTexture = LoadTextureFromImage(image);
    UnloadImage(image);
    GenerateBoard();
    while (!WindowShouldClose()) {

        float deltaTime = GetTime() - lasttime;
        lasttime = GetTime();
        updateAnimations(deltaTime * 1.5);

        if (player > numofplayers || player == 0) {
            player = 1;
            rounds++;
        }
        if (rounds > 0) {
            skipDeadPlayer();
        }
        ClearBackground(wcolors[player]);

        BeginDrawing();

        drawBoard();
        DrawCards(1150, 800);
        displayText();
        if ((lasttime_thunder + 1) > GetTime()) {
            DrawTexturePro(thunderTexture, { 0,0,113,126 }, { (float)thunderx,(float)thundery,113 * 6,126 * 6 }, { 50 * 6,126 * 6 }, 0, WHITE);
        }
        if ((lasttime_fire + 3) > GetTime()) {
            DrawTexturePro(fireTexture, { 0,0,43,58 }, { (float)xfire,(float)yfire,(float)((86.0f * (GetTime() - lasttime_fire + 3))),(float)(116.0f * (GetTime() - lasttime_fire + 3)) }, { (float)(42 * (GetTime() - lasttime_fire + 3) - CSIZE),(float)(116.0f * (GetTime() - lasttime_fire + 3) - CSIZE) }, 0, WHITE);
        }
        if ((lasttime_alert + 4) > GetTime()) {
            DrawRectangle(1000, 550, 750, 200, WHITE);
            DrawText((alert_string.c_str()), 1025, 600, 48, BLACK);
        }
        if ((lasttime_meteor + 3) > GetTime()) {
            for (int i = 0; i < meteor.size(); i++)
            {
                int _x = meteor[i].first, _y = meteor[i].second;
                DrawTexturePro(meteorTexture, { 0,0,19,18 }, { (float)(_x),(float)(_y),190,180 }, { 15,18 }, 45, WHITE);
            }
        }
        EndDrawing();
        Vector2 m;
        if (touchOrMouse) {
            m = GetTouchPosition(0);
        }
        else {
            //m = GetMousePosition();
        }
        //ShowFire(m.x,m.y);
        if (m.x > 1740 && m.y < 60 && IsMouseButtonDown(MOUSE_BUTTON_LEFT) && GetTime() - inputLimit > 0.4f) {
            GenerateBoard();
            numofplayers = rand() % 2 + 2;
            BOARD_WIDTH = rand() % 5 + 3;
            BOARD_HEIGHT = BOARD_WIDTH;
            inputLimit = GetTime();
        }
        if (m.x > 1740 && m.y > 940 && IsMouseButtonDown(MOUSE_BUTTON_LEFT) && GetTime() - inputLimit > 0.4f) {
            inputLimit = GetTime();
            for (int i = 0; i < 4; i++)
            {
                cards[player][i] = (rand() % numberOfCards) + 1;

            }
        }
        if (IsKeyDown(KEY_R) && GetTime() - inputLimit > 0.4f) {
            inputLimit = GetTime();
            for (int i = 0; i < 4; i++)
            {
                cards[player][i] = (rand() % numberOfCards) + 1;
                
            }
        }
        if (IsKeyDown(KEY_N) && GetTime() - inputLimit > 0.4f) {
            inputLimit = GetTime();
            GenerateBoard();
        }
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && GetTime() - inputLimit > 0.4f) {
            inputLimit = GetTime();
            cell null_cell;
            null_cell.p = 0;
            null_cell.v = 0;
            // m = GetMousePosition();
            int x = (int)(m.x - CELL_PADDING) / CSIZE, y = (int)(m.y - CELL_PADDING) / CSIZE;
            if (m.x > 1050 && m.x < 1800 && m.y >700 && m.y < 1000) {
                if (m.x < 1300) {
                    choosedCards[player][0] = !choosedCards[player][0];
                    choosedCards[player][1] = false;
                    choosedCards[player][2] = false;
                    choosedCards[player][3] = false;
                    std::cout << "choosed card number : " << (char)(cards[player][0] + '0');
                }
                else if (m.x < 1450) {
                    choosedCards[player][0] = false;
                    choosedCards[player][1] = !choosedCards[player][1];
                    choosedCards[player][2] = false;
                    choosedCards[player][3] = false;
                }
                else if (m.x < 1550){
                    choosedCards[player][0] = false;
                    choosedCards[player][1] = false;
                    choosedCards[player][2] = !choosedCards[player][2];
                    choosedCards[player][3] = false;
                }
                else {
                    choosedCards[player][0] = false;
                    choosedCards[player][1] = false;
                    choosedCards[player][3] = !choosedCards[player][3];
                    choosedCards[player][2] = false;
                }
            }
            else if (true || (board[(int)(m.x - CELL_PADDING) / CSIZE][(int)(m.y - CELL_PADDING) / CSIZE].v > 0)) {

                if (x > -1 && x < BOARD_WIDTH && y > -1 && y < BOARD_HEIGHT)
                {
                    bool yours = board[(int)(m.x - CELL_PADDING) / CSIZE][(int)(m.y - CELL_PADDING) / CSIZE].p == player && board[x][y].v > 0;
                    uint8_t activeCard = 0;
                    uint8_t cardIndex = 0;
                    for (int i = 0; i < 4; i++)
                    {
                        if (choosedCards[player][i]) { activeCard = cards[player][i]; cardIndex = i; }
                    }
                    if (activeCard == 0 && yours) {
                        aboard[x][y].v += 1;
                        aboard[x][y].p = player;
                        player++;
                        Countpoints();
                    }
                    else if (activeCard == DEFENSE_CARD && yours) {
                        cards[player][cardIndex] = 0;
                        //std::cout << "aktywowano kartê ochrony\n";
                        aboard[x][y].protectedTo = rounds + ((rand() % (numofplayers * 20)));
                        board[x][y].protectedTo = aboard[x][y].protectedTo;
                    }
                    else if (activeCard == LIGHTNING_CARD) {
                        if (rounds > 20 || IsKeyDown(KEY_A)) {
                            cards[player][cardIndex] = 0;
                            aboard[x][y] = null_cell;
                            aboard[(x - 1) % BOARD_WIDTH][y] = null_cell;
                            aboard[(x + 1) % BOARD_WIDTH][y] = null_cell;
                            aboard[x][(y - 1) % BOARD_HEIGHT] = null_cell;
                            aboard[x][(y + 1) % BOARD_HEIGHT] = null_cell;
                            ShowThunder(x * CSIZE, y * CSIZE);
                            player++;
                            Countpoints();
                        }
                        else {
                            ShowAlert("tej umiejetnosci mozesz urzyc\n\n\n dopiero po 20 rundzie");
                        }
                    }
                    else if (activeCard == PLUS_TWO_CARD && yours) {
                        cards[player][cardIndex] = 0;
                        aboard[x][y].v += (rand() % 2) + 2;
                        aboard[x][y].p = player;
                        player++;
                        Countpoints();
                    }
                    else if (activeCard == PLACE_CARD) {
                        if (board[x][y].v == 0 && (rounds > 8 || IsKeyDown(KEY_A))) {
                            cards[player][cardIndex] = 0;
                            aboard[x][y].v = 2 + (rand() % 2);
                            aboard[x][y].p = player;
                            player++;
                            Countpoints();
                        }
                        else {
                            ShowAlert("musisz dac komorke w\n\n\npuste miejsce po 8 rundzie");
                        }
                    }
                    else if (activeCard == FIRE_CARD) {
                        cards[player][cardIndex] = 0;
                        player++;
                        for (int i = 0; i < 9; i++)
                        {
                            int firex = x + fire[i][0], firey = y + fire[i][1];
                            if (firex < 0) { firex += BOARD_WIDTH; }
                            if (firex > BOARD_WIDTH) { firex -= BOARD_WIDTH; }
                            if (firey < 0) { firey += BOARD_HEIGHT; }
                            if (firey > BOARD_HEIGHT) { firey -= BOARD_HEIGHT; }
                            if (board[firex][firey].v > 0) {
                                aboard[firex][firey].v = 1;
                                aboard[firex][firey].protectedTo = rounds + 5;
                            }
                        }
                        ShowFire(CSIZE * x, CSIZE * y);
                        player++;
                        Countpoints();
                    }
                    else if (activeCard == METEOR_CARD)
                    {
                        if (rounds > 25 || IsKeyDown(KEY_A)) {
                            cards[player][cardIndex] = 0;
                            meteor.clear();
                            int r = 10 + (rand() % 4);
                            for (int i = 0; i < r; i++)
                            {
                                int rx = rand() % BOARD_WIDTH, ry = rand() % BOARD_HEIGHT;
                                if (board[rx][ry].v != 0 && board[rx][ry].p !=player) {
                                    aboard[rx][ry] = null_cell;
                                    meteor.push_back({ rx * CELL_SIZE, ry * CELL_SIZE });
                                }
                                else {
                                    i--;
                                }
                            }
                            ShowMeteor();
                            player++;
                        }
                        else {
                            ShowAlert("Mozesz urzyc tej karty\n\n\ndopiero po 25 rundzie");
                        }
                    }
                    else if (activeCard == WALL_CARD) {
                        if (rounds > 15 || IsKeyDown(KEY_A)) {
                            cards[player][cardIndex] = 0;
                            player++;
                            int r = rand() % 2;
                            switch (r) {
                            case 0: // poziomo
                                for (int i = 0; i < BOARD_WIDTH; i++)
                                {
                                    board[i][y].v = 1;
                                    board[i][y].p = 0;
                                    aboard[i][y] = null_cell;
                                    aboard[i][y].protectedTo = rounds + (numofplayers * (rand() % 7))+3;
                                }
                                break;
                            case 1:
                                for (int i = 0; i < BOARD_WIDTH; i++)
                                {
                                    board[x][i].v = 1;
                                    board[x][i].p = 0;
                                    aboard[x][i] = null_cell;
                                    aboard[x][i].protectedTo = rounds + (numofplayers * (rand() % 7)) + 3;
                                }
                            }
                        }
                        else {
                            ShowAlert("mozsz urzyc tej karty\n\n\ndopiero po 15 rundzie");
                        }
                    }
                    else if (activeCard == BOMB_CARD) {
                        if (rounds > 20 || IsKeyDown(KEY_A)) {
                            cards[player][cardIndex] = 0;
                            player++;
                            cell full_cell;
                            full_cell.p = player - 1;
                            full_cell.v = 4;
                            const int8_t pos[4][2] = { {-1,0},{1,0},{0,-1},{0,1} };
                            for (int i = 0; i < 4; i++)
                            {
                                int _x = x + pos[i][0], _y = pos[i][1] + y;
                                _x = _x % (BOARD_WIDTH - 1);
                                _y = _y % (BOARD_HEIGHT - 1);
                                for (int i = 0; i < 4; i++)
                                {
                                    if (board[_x][_y].v == 0 || board[_x][_y].p == player) {
                                        aboard[_x][_y] = full_cell;
                                    }
                                }
                            }
                        }
                        else {
                            ShowAlert("mozsz urzyc tej karty\n\n\ndopiero po 20 rundzie");
                        }
                    }
                    else if (activeCard == FREEZE_CARD) {
                        cards[player][cardIndex] = 0;
                        playerFrozen = board[x][y].p;
                        playerFrozenToRound = rounds + 2;
                        if (rand() % 5 == 0) { playerFrozenToRound++; }
                        if (rand() % 5 != 0) { player++; }
                    }

                }
                cout << "Mouse x: " << m.x << "   y: " << m.y << "     x/40 & y/40 =   " << x << "    " << y << "    protected for: " << aboard[x][y].protectedTo << "     value: " << (char)(board[x][y].v + '0') << "     value a: " << (char)(aboard[x][y].v + '0') << "\n";
            }

        }
    }
    return 0;
}


/*#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <map>
#define SIZE_10
using namespace std;
using namespace sf;

Clock inputLimit;
uint8_t player = 1;
#ifdef SIZE_5
// Sta³e
int numofplayers = 1
const int CELL_SIZE = 32 * 4;//128
const int CELL_PADDING = 32;
const int CELL_P_SIZE = 16;
constexpr int CSIZE = CELL_SIZE + CELL_PADDING;
const int BOARD_WIDTH = 5; // Szerokoœæ planszy 
const int BOARD_HEIGHT = 5; // Wysokoœæ planszy 
const float ANIMATION_DURATION = 0.250f; // Czas trwania animacji w sekundach
#endif
#ifdef SIZE_10
// Sta³e
int numofplayers = 2;
int BOARD_WIDTH = 10; // Szerokoœæ planszy 
int BOARD_HEIGHT = 10; // Wysokoœæ planszy 
int CELL_SIZE = (((1000 / BOARD_HEIGHT) / 5) * 4) - 2;//128;//64 86
int CELL_PADDING = CELL_SIZE / 4;
int CELL_P_SIZE = CELL_PADDING;
int CSIZE = CELL_SIZE + CELL_PADDING;
float ANIMATION_DURATION = 0.25f; // Czas trwania animacji w sekundach
#endif
//const Color pcolors[16] = { Color(0xFFFFFFFF), Color(0xFF0000FF), Color(0x0000FFFF)};
//const Color wcolors[16] = { Color(0xFFFFFFFF), Color(0xFF6161FF), Color(0x6161FFFF) };
//                          white                  red                blue
const Color pcolors[16] = { Color(0xFFFFFFFF), Color(0xFF0000FF),  Color(0x0c52f5),Color(0x0CC29AFF), Color(0xFFCC00FF)};
const Color wcolors[16] = { Color(0xFFFFFFFF), Color(0xFF6161FF),  Color(0x5a7ccc),Color(0x3ab54cFF), Color(0xffe066FF)};
//                          white                  red                blue               green

std::string names[16] = { "" , "Antek",         "Tomasz",            "Julek"};
int points[16] = { 0 };
int total_points[16] = { 0 };
struct cell {
    uint8_t v : 4;
    uint8_t p : 4;
};

// Tablice do przechowywania wartoœci komórek
cell board[40][40];    // Plansza pocz¹tkowa
cell aboard[40][40];   // Plansza po interakcji

// Struktura do przechowywania animacji dla ka¿dej komórki
struct CellAnimation {
    float elapsedTime = 0.0f;
    bool animating = false;
    int startValue = 0;
    int endValue = 0;
};

CellAnimation animations[40][40]; // Tablica animacji
map<uint8_t, uint8_t> playerToPlayer;
int rounds = 0;

Text text;
Font dpcomic;
void Check44() {
    for (int y = 0; y < BOARD_HEIGHT; y++)
    {
        for (int x = 0; x < BOARD_WIDTH; x++)
        {
            if (board[x][y].v >= 4) {
                board[x][y].v -= 4;
                int p = board[x][y].p;
                aboard[x][y].v -= 4;
                if (x > 0) {
                    aboard[x - 1][y].v++;
                    aboard[x - 1][y].p = p;
                }
                else {
                    aboard[BOARD_WIDTH-1][y].v++;
                    aboard[BOARD_WIDTH-1][y].p = p;
                }
                if (x < (BOARD_WIDTH - 1)) {
                    aboard[x + 1][y].v++;
                    aboard[x + 1][y].p = p;
                }
                else {
                    aboard[0][y].v++;
                    aboard[0][y].p = p;
                }
                if (y > 0) {
                    aboard[x][y - 1].v++;
                    aboard[x][y - 1].p = p;
                }
                else {
                    aboard[x][BOARD_HEIGHT-1].v++;
                    aboard[x][BOARD_HEIGHT-1].p = p;
                }
                if (y < (BOARD_HEIGHT - 1)) {
                    aboard[x][y+1].v++;
                    aboard[x][y+1].p = p;
                }
                else {
                    aboard[x][0].v++;
                    aboard[x][0].p = p;
                }
            }
        }
    }
}
void skipDeadPlayer() {
    bool is = false;
    for (int y = 0; y < BOARD_HEIGHT; y++)
    {
        for (int x = 0; x < BOARD_WIDTH; x++)
        {
            if (board[x][y].p == player) { is = true; break; }
        }
    }
    if (!is) {
        player++;
        skipDeadPlayer();
    }
}
void updateAnimations(float deltaTime) {
    Check44();
    for (int x = 0; x < BOARD_WIDTH; ++x) {
        for (int y = 0; y < BOARD_HEIGHT; ++y) {
            if (board[x][y].v != aboard[x][y].v && !animations[x][y].animating) {
                animations[x][y].animating = true;
                animations[x][y].startValue = board[x][y].v;
                animations[x][y].endValue = aboard[x][y].v;
                animations[x][y].elapsedTime = 0.0f;
            }
            
            if (animations[x][y].animating) {
                animations[x][y].elapsedTime += deltaTime;
                if (animations[x][y].elapsedTime >= ANIMATION_DURATION) {
                    board[x][y] = aboard[x][y];
                    animations[x][y].animating = false;
                }
            }
        }
    }
}

void drawBoard(sf::RenderWindow& window) {
    for (int x = 0; x < BOARD_WIDTH; ++x) {
        for (int y = 0; y < BOARD_HEIGHT; ++y) {
            // Pozycja komórki
            int posX = x * (CELL_SIZE + CELL_PADDING);
            int posY = y * (CELL_SIZE + CELL_PADDING);

            // G³ówna komórka (szary kwadrat)
            sf::RectangleShape cell(sf::Vector2f(CELL_SIZE, CELL_SIZE));
            cell.setPosition(posX + CELL_PADDING, posY + CELL_PADDING);
            cell.setFillColor(sf::Color::White);
            window.draw(cell);

            // Animacja rozmiaru czerwonego kwadratu
            float progress = std::min(animations[x][y].elapsedTime / ANIMATION_DURATION, 1.0f);
            float currentValue = animations[x][y].animating
                ? animations[x][y].startValue + static_cast<float>(progress * (animations[x][y].endValue - animations[x][y].startValue))
                : board[x][y].v;
            
            // Wewnêtrzny czerwony kwadrat
            float redSquareSize = currentValue * CELL_P_SIZE;
            sf::RectangleShape innerCell(sf::Vector2f(redSquareSize, redSquareSize));
            innerCell.setPosition(posX + CELL_PADDING + (CELL_SIZE - redSquareSize) / 2, posY + CELL_PADDING + (CELL_SIZE - redSquareSize) / 2);
            innerCell.setFillColor(pcolors[board[x][y].p]);
            window.draw(innerCell);
        }
    }
}
void displayText(sf::RenderWindow& window) {
    text.setPosition(1050, 100);
    text.setString(sf::String("Runda: " + to_string(rounds)));
    window.draw(text);
    text.move(0, 100);
    for (int i = 1; i < numofplayers + 1; i++)
    {
        text.setString(sf::String(names[i]));
        window.draw(text);
        text.move(0, 50);
    }
    text.setPosition(1250, 200);
    for (int i = 1; i < numofplayers + 1; i++)
    {
        text.setString(sf::String(to_string(points[i])));
        window.draw(text);
        text.move(0, 50);
    }
    text.setPosition(1400, 200);
    for (int i = 1; i < numofplayers + 1; i++)
    {
        text.setString(sf::String(to_string(total_points[i])));
        window.draw(text);
        text.move(0, 50);
    }
}
void Countpoints() {
    for (int i = 0; i < numofplayers + 1; i++)
    {
        total_points[i] += points[i];
        points[i] = 0;
    }
    for (int y = 0; y < BOARD_HEIGHT; y++)
    {
        for (int x = 0; x < BOARD_WIDTH; x++)
        {
            points[board[x][y].p] += board[x][y].v;
        }
    }
}
int main() {

    sf::RenderWindow window(sf::VideoMode(1800, 1000), "Color Wars");
    sf::Clock clock;
    dpcomic.loadFromFile("dpcomic.ttf");
    text.setFont(dpcomic);
    text.setCharacterSize(48);
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        float deltaTime = clock.restart().asSeconds();
        updateAnimations(deltaTime);

        if (player > numofplayers || player == 0) {
            player = 1;
            rounds++;
        }
        if (rounds > 0) {
            skipDeadPlayer();
        }
        window.clear(wcolors[player]);
        drawBoard(window);
        displayText(window);
        window.display();

        Vector2i m;
        if (Mouse::isButtonPressed(Mouse::Left) && inputLimit.getElapsedTime().asSeconds() > 0.4f) {
            m = Mouse::getPosition(window);
            if (board[(m.x - CELL_PADDING)/ CSIZE][(m.y - CELL_PADDING ) / CSIZE].v > 0 && board[(m.x - CELL_PADDING) / CSIZE][(m.y - CELL_PADDING)/CSIZE].p == player) {
                aboard[(m.x - CELL_PADDING)/ CSIZE][(m.y - CELL_PADDING)/ CSIZE].v += 1;
                aboard[(m.x - CELL_PADDING)/ CSIZE][(m.y - CELL_PADDING)/ CSIZE].p = player;
                player++;
                Countpoints();
            }
            else if (sf::Keyboard::isKeyPressed(Keyboard::Num3)) {
                aboard[(m.x - CELL_PADDING)/ CSIZE][(m.y - CELL_PADDING) / CSIZE].v = 3;
                aboard[(m.x - CELL_PADDING)/ CSIZE][(m.y - CELL_PADDING )/ CSIZE].p = player;
                player++;
            }

            cout << "Mouse x: " << m.x << "   y: " << m.y << "     x/40 & y/40 =   " << m.x / 40 << "    " << m.y / 40 << "     value: " << (char)(board[m.x / 40][m.y / 40].v + '0') << "     value a: " << (char)(aboard[m.x / 40][m.y / 40].v + '0') << "\n";
            inputLimit.restart();
        }
        
    }

    return 0;
}*/
