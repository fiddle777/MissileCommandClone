#include <conio.h>
#include <cwchar>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <windows.h>
#include <cmath>
#include <map>
#include <thread>
#include <mutex>
using namespace std;
bool endgameLevel = false, endgameFinal = false;
const int width = 99 + 2, height = 50 + 2;
HANDLE hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
mutex mtx;
class Missile {
private:
    char name;
    int coordX, coordY, targetX, targetY;
    int velocityX = 0, velocityY = 0;
    bool detonate = false;
public:
    Missile(int x, int y, int x2, int y2, char z) : coordX(x), coordY(y), targetX(x2), targetY(y2), name(z), velocityX(0), velocityY(0), detonate(false) {}
    ~Missile() {}
    void TrajectoryCalc() {
        int distanceX = targetX - coordX;
        int distanceY = targetY - coordY;
        int ratio;
        if (distanceX == 0 && distanceY == 0) {
            velocityX = 0;
            velocityY = 0;
            detonate = true;
        }
        else if (distanceX == 0) {
            velocityX = 0;
            velocityY = 1;
            if (distanceY < 0) {
                velocityY = -1;
            }
        }
        else if (distanceY == 0) {
            velocityY = 0;
            velocityX = 1;
            if (distanceX < 0) {
                velocityX = -1;
            }
        }
        else if (abs(distanceX) >= abs(distanceY)) {
            ratio = distanceY / distanceX;
            if (distanceX < 0) {
                velocityX = -1;
                velocityY = ratio * velocityX;
            }
            else {
                velocityX = 1;
                velocityY = ratio * velocityX;
            }
        }
        else {
            ratio = distanceX / distanceY;
            if (distanceY < 0) {
                velocityY = -1;
                velocityX = ratio * velocityY;
            }
            else {
                velocityY = 1;
                velocityX = ratio * velocityY;
            }
        }
    }
    char Getname() const { return name; }
    int GetcoordX() const { return coordX; }
    int GetcoordY() const { return coordY; }
    int Getdx() const { return velocityX; }
    int Getdy() const { return velocityY; }
    bool Getdetonate() const { return detonate; }
    void Setdetonate(bool d) { detonate = d; }
    void SetcoordX() { coordX += velocityX; }
    void SetcoordY() { coordY += velocityY; }
    bool operator==(const Missile& otherMissile) const {
        return(coordX == otherMissile.coordX && coordY == otherMissile.coordY);
    }
};
class WindowFunctions {
public:
    static void SetConsoleDimensions(int x, int y) {
        SMALL_RECT windowSize = { 0, 0, x - 1, y - 1 };
        COORD bufferSize = { x, y };
        SetConsoleScreenBufferSize(hConsoleOutput, bufferSize);
        SetConsoleWindowInfo(hConsoleOutput, TRUE, &windowSize);
    }
    static void CursorGetPosition(int& x, int& y) {
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        if (GetConsoleScreenBufferInfo(hConsoleOutput, &csbi)) {
            x = csbi.dwCursorPosition.X;
            y = csbi.dwCursorPosition.Y;
        }
        else {
            x = -1;
            y = -1;
        }
    }
    static void CursorSetPosition(int x, int y) {
        COORD cursor_pos;
        cursor_pos.X = x;
        cursor_pos.Y = y;
        SetConsoleCursorPosition(hConsoleOutput, cursor_pos);
    }
    static char GetCharAtPosition(int x, int y) {
        CHAR_INFO buffer[1];
        COORD coord = { static_cast<SHORT>(x), static_cast<SHORT>(y) };
        SMALL_RECT readRegion = { x, y, x, y };
        COORD bufferSize = { 1, 1 };
        COORD bufferCoord = { 0, 0 };
        if (ReadConsoleOutput(GetStdHandle(STD_OUTPUT_HANDLE), buffer, bufferSize, bufferCoord, &readRegion)) {
            return buffer[0].Char.AsciiChar;
        }
        else {
            return ' ';
        }
    }
    static void ClearArea(int x, int y, int object_width, int object_height) {
        COORD coord;
        DWORD charsWritten;
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(hConsoleOutput, &csbi);
        for (int i = y; i < object_height + y; ++i) {
            coord.Y = i;
            for (int j = x; j < object_width + x; ++j) {
                coord.X = j;
                FillConsoleOutputCharacter(hConsoleOutput, ' ', 1, coord, &charsWritten);
                FillConsoleOutputAttribute(hConsoleOutput, csbi.wAttributes, 1, coord,
                    &charsWritten);
            }
        }
    }
    static void DrawBox() {
        cout << "+";
        for (int i = 1; i <= width - 2; i++) {
            cout << "-";
        }
        cout << "+" << endl;
        for (int i = 1; i <= height - 3; i++) {
            cout << "|";
            for (int j = 1; j < width - 1; j++) {
                cout << " ";
            }
            cout << "|" << endl;
        }
        cout << "+";
        for (int i = 1; i < width - 1; i++) {
            cout << "-";
        }
        cout << "+" << endl;
    }
};
class MenuFunctions {
public:
    static void DisplayStart() {
        WindowFunctions::CursorSetPosition(5, 11);
        cout << "___  ___ _            _  _         _____                                                 _ ";
        WindowFunctions::CursorSetPosition(5, 12);
        cout << "|  \\/  |(_)          (_)| |       /  __ \\                                               | |";
        WindowFunctions::CursorSetPosition(5, 13);
        cout << "| .  . | _  ___  ___  _ | |  ___  | /  \\/  ___   _ __ ___   _ __ ___    __ _  _ __    __| |";
        WindowFunctions::CursorSetPosition(5, 14);
        cout << "| |\\/| || |/ __|/ __|| || | / _ \\ | |     / _ \\ | '_ ` _ \\ | '_ ` _ \\  / _` || '_ \\  / _` |";
        WindowFunctions::CursorSetPosition(5, 15);
        cout << "| |  | || |\\__ \\\\__ \\| || ||  __/ | \\__/\\| (_) || | | | | || | | | | || (_| || | | || (_| |";
        WindowFunctions::CursorSetPosition(5, 16);
        cout << "\\_|  |_/|_||___/|___/|_||_| \\___|  \\____/ \\___/ |_| |_| |_||_| |_| |_| \\__,_||_| |_| \\__,_|";
        WindowFunctions::CursorSetPosition(10, 17);
        cout << "clone";
        WindowFunctions::CursorSetPosition(32, 20);
        cout << "CLICK P TO PLAY";
        WindowFunctions::CursorSetPosition(32, 22);
        cout << "CLICK Q TO QUIT";
        WindowFunctions::CursorSetPosition(32, 26);
        cout << "CONTROLS:";
        WindowFunctions::CursorSetPosition(32, 28);
        cout << "CURSOR MOVEMENT: W A S D ||OR|| ARROW KEYS";
        WindowFunctions::CursorSetPosition(32, 30);
        cout << "CLICK SPACE TO FIRE";
        WindowFunctions::CursorSetPosition(20, 48);
        cout << "Deividas Narbutas PRIF 23/4, 2024, no rights reserved...";
    }
    static void DisplayEndLevel(int level, const map<pair<int, int>, bool> cities, int missiles) {
        WindowFunctions::CursorSetPosition(32, 20);
        cout << "LEVEL " << level << " COMPLETED";
        WindowFunctions::CursorSetPosition(32, 22);
        int citiesCount = 0;
        for (auto& city : cities) {
            if (!city.second) {
                citiesCount++;
            }
        }
        cout << "CITIES REMAINING: " << citiesCount;
        WindowFunctions::CursorSetPosition(32, 24);
        cout << "ENEMY MISSILES FIRIED: " << missiles;
        WindowFunctions::CursorSetPosition(32, 28);
        cout << "PRESS SPACE KEY TO START NEXT LEVEL";
    }
    static void DisplayEndGame(int level) {
        WindowFunctions::CursorSetPosition(32, 20);
        cout << "GAME OVER";
        WindowFunctions::CursorSetPosition(32, 22);
        cout << "LEVEL REACHED: " << level;
        WindowFunctions::CursorSetPosition(32, 26);
        cout << "PRESS Q KEY TO QUIT";
        WindowFunctions::CursorSetPosition(32, 28);
        cout << "PRESS SPACE KEY TO RESTART";
    }
};
class MapFunctions {
public:
    static void DrawCity(int x, int y) {
        WindowFunctions::CursorSetPosition(x, y);
        cout << "   XX   ";
        WindowFunctions::CursorSetPosition(x, y + 1);
        cout << "  X  X  ";
        WindowFunctions::CursorSetPosition(x, y + 2);
        cout << " X    X ";
        WindowFunctions::CursorSetPosition(x, y + 3);
        cout << "X      X";
        WindowFunctions::CursorSetPosition(x, y + 4);
        cout << "+------+";
    }
    static void DrawHQ(int x, int y) {
        WindowFunctions::CursorSetPosition(x, y);
        cout << "      *      ";
        WindowFunctions::CursorSetPosition(x, y + 1);
        cout << "      |      ";
        WindowFunctions::CursorSetPosition(x, y + 2);
        cout << "    +-+-+    ";
        WindowFunctions::CursorSetPosition(x, y + 3);
        cout << "    |   |    ";
        WindowFunctions::CursorSetPosition(x, y + 4);
        cout << "+---+---+---+";
        WindowFunctions::CursorSetPosition(x, y + 5);
        cout << "|           |";
        WindowFunctions::CursorSetPosition(x, y + 6);
        cout << "|           |";
        WindowFunctions::CursorSetPosition(x, y + 7);
        cout << "+-----------+";
    }
    static void DrawMap() {
        WindowFunctions::CursorSetPosition(0, height - 5);
        cout << "+";
        for (int i = 0; i < 99; i++) {
            cout << '-';
        }
        cout << "+";
        MapFunctions::DrawCity(5, height - 9);
        MapFunctions::DrawCity(18, height - 9);
        MapFunctions::DrawCity(31, height - 9);
        MapFunctions::DrawHQ(44, height - 12);
        MapFunctions::DrawCity(62, height - 9);
        MapFunctions::DrawCity(75, height - 9);
        MapFunctions::DrawCity(88, height - 9);
    }
};
class GameFunctions {
public:
    static bool CollisionCheck(int x, int y) {
        CHAR_INFO buffer[1];
        COORD coord = { static_cast<SHORT>(x), static_cast<SHORT>(y) };
        SMALL_RECT rect = { coord.X, coord.Y, coord.X, coord.Y };
        ReadConsoleOutput(hConsoleOutput, buffer, COORD{ 1, 1 }, COORD{ 0, 0 }, &rect);
        return buffer[0].Char.AsciiChar != ' ';
    }
    static bool BoundaryCheck(int x, int y) {
        if (x < 1 || x >= width - 1 || y < 1 || y >= height - 1) {
            return false;
        }
        else {
            return true;
        }
    }
    static void DrawObject(int x, int y, char symbol) {
        WindowFunctions::CursorSetPosition(x, y);
        cout << symbol;
    }
    static void ClearObject(int x, int y) {
        WindowFunctions::CursorSetPosition(x, y);
        cout << ' ';
    }
    static void MoveCursor(int& x, int& y, int dx, int dy, char symbol) {
        int x2 = x + dx;
        int y2 = y + dy;
        if (!GameFunctions::CollisionCheck(x2, y2) && GameFunctions::BoundaryCheck(x2, y2)) {
            GameFunctions::ClearObject(x, y);
            x = x2;
            y = y2;
            GameFunctions::DrawObject(x, y, symbol);
        }
    }
    static void MoveRocket(int x, int y, int dx, int dy, char symbol, bool& detonate) {
        int x2 = x + dx;
        int y2 = y + dy;
        if (!GameFunctions::CollisionCheck(x2, y2) && GameFunctions::BoundaryCheck(x2, y2)) {
            GameFunctions::ClearObject(x, y);
            x = x2;
            y = y2;
            GameFunctions::DrawObject(x, y, symbol);
        }
        else {
            detonate = true;
        }
    }
    static void DetonateRocket(int x, int y) {
        const int explosionRadius = 5;
        for (int i = -explosionRadius; i <= explosionRadius; ++i) {
            for (int j = -explosionRadius; j <= explosionRadius; ++j) {
                int dist = abs(i) + abs(j);
                if (dist <= explosionRadius) {
                    char huh = WindowFunctions::GetCharAtPosition(x + i, y + j);
                    if (huh != '-' && huh != '+' && huh != '|' && huh != '#') {
                        GameFunctions::DrawObject(x + i, y + j, '*');
                    }
                }
            }
        }
        Sleep(100);
        for (int i = -explosionRadius; i <= explosionRadius; ++i) {
            for (int j = -explosionRadius; j <= explosionRadius; ++j) {
                int dist = abs(i) + abs(j);
                if (dist <= explosionRadius) {
                    char huh = WindowFunctions::GetCharAtPosition(x + i, y + j);
                    if (huh != '-' && huh != '+' && huh != '|' && huh != '#') {
                        GameFunctions::ClearObject(x + i, y + j);
                    }
                }
            }
        }
    }
    static void SpawnEnemyMissile(vector<Missile>& missiles, const map<pair<int, int>, bool>& cities) {
        vector<pair<int, int>> citiesAlive;
        for (const auto& city : cities) {
            if (!city.second) {
                citiesAlive.push_back(city.first);
            }
        }
        if (!citiesAlive.empty()) {
            int spawnX = rand() % (width - 2) + 1;
            int spawnY = 1;
            pair<int, int> target = citiesAlive[rand() % citiesAlive.size()];
            Missile newEnemyMissile(spawnX, spawnY, target.first, target.second, 'O');
            missiles.push_back(newEnemyMissile);
        }
    }
    static void InterceptCheck(vector<Missile>& missiles, int x, int y, int radius) {
        for (size_t i = 0; i < missiles.size(); ++i) {
            int targetX = missiles[i].GetcoordX();
            int targetY = missiles[i].GetcoordY();
            double dist = sqrt(pow(targetX - x, 2) + pow(targetY - y, 2));
            if (dist <= radius && !(targetX == x && targetY == y)) {
                missiles[i].Setdetonate(true);
            }
        }
    }
    static void DetonateCity(map<pair<int, int>, bool>& cities) {
        for (auto& city : cities) {
            if (city.second == false) {
                int x = city.first.first;
                int y = city.first.second;
                if (WindowFunctions::GetCharAtPosition(x, y) != 'X') {
                    WindowFunctions::CursorSetPosition(x - 3, y);
                    cout << "        ";
                    WindowFunctions::CursorSetPosition(x - 3, y + 1);
                    cout << "        ";
                    WindowFunctions::CursorSetPosition(x - 3, y + 2);
                    cout << " *      ";
                    WindowFunctions::CursorSetPosition(x - 3, y + 3);
                    cout << "****  * ";
                    WindowFunctions::CursorSetPosition(x - 3, y + 4);
                    cout << "********";
                    city.second = true;
                }
            }
        }
    }
    static void ResetCityStatus(map<pair<int, int>, bool>& cities) {
        for (auto& city : cities) {
            city.second = false;
        }
    }
};
class LogFunctions {
public:
    static void DrawInfo(int level) {
        WindowFunctions::CursorSetPosition(width + 4, 4);
        cout << "LEVEL: " << level;
        WindowFunctions::CursorSetPosition(width + 4, 6);
        cout << "ENEMY MISSILES REMAINING: ";
        WindowFunctions::CursorSetPosition(width + 4, 8);
        cout << "REMAINING CITIES: ";
    }
    static void DrawStats(int endgameCount, map<pair<int, int>, bool> cities) {
        WindowFunctions::ClearArea(width + 4, 6, 30, 4);
        WindowFunctions::CursorSetPosition(width + 4, 6);
        cout << "ENEMY MISSILES REMAINING: " << endgameCount;
        int citiesCount = 0;
        for (auto& city : cities) {
            if (!city.second) {
                citiesCount++;
            }
        }
        WindowFunctions::CursorSetPosition(width + 4, 8);
        cout << "REMAINING CITIES: " << citiesCount;
    }
};
void Thread_UserInput(vector<Missile>& missiles, map<pair<int, int>, bool> cities, int cursorX, int cursorY, int HQX, int HQY) {
    while (!endgameLevel && !endgameFinal) {
        mtx.lock();
        if (_kbhit()) {
            int key = _getch();
            switch (key) {
            case 72: case 87: case 119: // UP
                GameFunctions::MoveCursor(cursorX, cursorY, 0, -1, '#');
                break;
            case 80: case 83: case 115: // DOWN
                GameFunctions::MoveCursor(cursorX, cursorY, 0, 1, '#');
                break;
            case 75: case 65: case 97: // LEFT
                GameFunctions::MoveCursor(cursorX, cursorY, -1, 0, '#');
                break;
            case 77: case 68: case 100: // RIGHT
                GameFunctions::MoveCursor(cursorX, cursorY, 1, 0, '#');
                break;
            case 32: // SPACE
                Missile newMissile(HQX, HQY, cursorX, cursorY, 'Y');
                missiles.push_back(newMissile);
                break;
            }
        }
        mtx.unlock();
    }
}
void Thread_MissileLogic(vector<Missile>& missiles, map<pair<int, int>, bool>& cities, int dif_ms, int dif_count, int dif_freq) {
    int endgameCount = dif_count;
    int dif_ms_2 = 0;
    LogFunctions::DrawStats(endgameCount, cities);
    while (!endgameLevel && !endgameFinal) {
        mtx.lock();
        if (dif_ms_2 >= dif_freq) {
            dif_ms_2 = 0;
            if (endgameCount > 0) {
                GameFunctions::SpawnEnemyMissile(missiles, cities);
                endgameCount--;
                LogFunctions::DrawStats(endgameCount, cities);
            }
        }
        vector<int>deleteMarker;
        for (size_t i = 0; i < missiles.size(); ++i) {
            missiles[i].TrajectoryCalc();
            char missileName = static_cast<char>(missiles[i].Getname());
            int missileX = static_cast<int>(missiles[i].GetcoordX());
            int missileY = static_cast<int>(missiles[i].GetcoordY());
            int missileDX = static_cast<int>(missiles[i].Getdx());
            int missileDY = static_cast<int>(missiles[i].Getdy());
            bool detonate = missiles[i].Getdetonate();
            GameFunctions::MoveRocket(missileX, missileY, missileDX, missileDY, missileName, detonate);
            GameFunctions::InterceptCheck(missiles, missileX, missileY, 2);
            missiles[i].SetcoordX();
            missiles[i].SetcoordY();
            if (detonate) {
                deleteMarker.push_back(i);
                GameFunctions::DetonateRocket(missileX, missileY);
                GameFunctions::InterceptCheck(missiles, missileX, missileY, 6);
                if (missileY >= height - 15) {
                    GameFunctions::DetonateCity(cities);
                    int citiesAliveCount = 0;
                    for (auto& city : cities) {
                        if (!city.second) {
                            citiesAliveCount++;
                        }
                    }
                    if (citiesAliveCount <= 0) {
                        endgameFinal = true;
                    }
                    LogFunctions::DrawStats(endgameCount, cities);
                }
            }
        }
        for (int i = deleteMarker.size() - 1; i >= 0; i--) {
            missiles.erase(missiles.begin() + deleteMarker[i]);
        }
        if (endgameCount <= 0 && missiles.empty()) {
            endgameLevel = true;
        }
        dif_ms_2 += dif_ms;
        mtx.unlock();
        Sleep(dif_ms);
    }
}
int main() {
    WindowFunctions::SetConsoleDimensions(width + 40, height);
    bool exit = false;
    while (!exit) {
        exit = false;
        srand(static_cast<unsigned>(time(0)));
        endgameLevel = false;
        endgameFinal = false;
        map<pair<int, int>, bool> cities = {
        {{8, height - 9}, false},
        {{21, height - 9}, false},
        {{34, height - 9}, false},
        {{65, height - 9}, false},
        {{78, height - 9}, false},
        {{91, height - 9}, false}
        };
        int cursorX = width / 2, cursorY = height / 2;
        int HQX = 50, HQY = height - 13;
        vector<Missile> missiles;
        int dif_lvl = 1, dif_ms = 300, dif_count = 6, dif_freq = 4000;
        system("CLS");
        WindowFunctions::DrawBox();
        MenuFunctions::DisplayStart();
        while (1) {
            if (_kbhit()) {
                int key = _getch();
                if (key == 80 || key == 112) {
                    break;
                }
                if (key == 81 || key == 113) {
                    exit = true;
                    break;
                }
            }
        }
        if (exit) {
            break;
        }
        while (!endgameFinal) {
            system("CLS");
            WindowFunctions::DrawBox();
            MapFunctions::DrawMap();
            GameFunctions::ResetCityStatus(cities);
            GameFunctions::DrawObject(cursorX, cursorY, '#');
            LogFunctions::DrawInfo(dif_lvl);
            try {
                thread UserinputThread(Thread_UserInput, ref(missiles), ref(cities), ref(cursorX), ref(cursorY), ref(HQX), ref(HQY));
                thread MissileThread(Thread_MissileLogic, ref(missiles), ref(cities), ref(dif_ms), ref(dif_count), ref(dif_freq));
                UserinputThread.join();
                MissileThread.join();
            }
            catch (const exception& err) {
                cerr << "CRITICAL ERROR! EXCEPTION IN THREAD OPERATIONS: " << err.what();
            }
            if (dif_lvl >= 10) {
                endgameFinal = true;
                break;
            }
            if (endgameFinal) {
                break;
            }
            system("CLS");
            WindowFunctions::DrawBox();
            MenuFunctions::DisplayEndLevel(dif_lvl, cities, dif_count);
            Sleep(2000);
            while (1) {
                if (_kbhit()) {
                    int key = _getch();
                    if (key == 32) {
                        break;
                    }
                }
            }
            endgameLevel = false;
            dif_lvl++;
            dif_ms -= 25;
            dif_count += 2;
            dif_freq -= 200;
        }
        system("CLS");
        MenuFunctions::DisplayEndGame(dif_lvl);
        Sleep(2000);
        while (1) {
            if (_kbhit()) {
                int key = _getch();
                if (key == 32) {
                    break;
                }
                if (key == 81 || key == 113) {
                    exit = true;
                    break;
                }
            }
        }
        if (exit) {
            break;
        }
    }
    system("CLS");
    cout << "ok bye :(";
    return 0;
}