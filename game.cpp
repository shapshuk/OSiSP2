#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <chrono>
using namespace std;

#include <cstdio>
#include <Windows.h>
#include <cmath>
#include "game.h"



int nScreenWidth = 120;			// Console Screen Size X (columns)
int nScreenHeight = 40;			// Console Screen Size Y (rows)

short dirX[4] = { -1, 0, 1, 0 };
short dirY[4] = { 0, -1, 0, 1 };


string getMap(int& nMapSize, int seed) {
    srand(seed);

    bool bMap[30][30];
    for (int i = 0; i < nMapSize; i++)
        for (int j = 0; j < nMapSize; j++)
            bMap[i][j] = false;

    int nPosX = 1;
    int nPosY = 1;
    bMap[nPosX][nPosY] = true;
    bool bLabEnded = false;
    while (!bLabEnded) {
        short dirVal = rand() % 4;

        if ((nPosX + dirX[dirVal] < nMapSize - 1) && (nPosX + dirX[dirVal] > 0) &&
            (nPosY + dirY[dirVal] < nMapSize - 1) && (nPosY + dirY[dirVal] > 0) &&
            ((bMap[nPosX + dirX[dirVal] * 2][nPosY + dirY[dirVal] * 2] == false) ||
                (bMap[nPosX + dirX[dirVal]][nPosY + dirY[dirVal]] == true))) {

            bMap[nPosX + dirX[dirVal]][nPosY + dirY[dirVal]] = true;
            nPosX += dirX[dirVal] * 2;
            nPosY += dirY[dirVal] * 2;
            bMap[nPosX][nPosY] = true;
        }

        bLabEnded = true;
        for (int i = 1; i < nMapSize; i += 2) {
            for (int j = 1; j < nMapSize; j += 2) {
                if (bMap[i][j] == false) {
                    bLabEnded = false;
                    break;
                }
                if (bLabEnded == false)
                    break;
            }
        }
    }

    string map = "";
    bMap[nMapSize - 2][nMapSize - 1] = true;
    for (int i = 0; i < nMapSize; i++) {
        for (int j = 0; j < nMapSize; j++) {
            if (bMap[i][j] == false)
                map += '#';
            else
                map += '.';
        }
    }
    return map;

}


struct gameStats {
    float fPlayerX;			// Player Start Position
    float fPlayerY;
    float fPlayerAngle;		// Player Start Rotation
    float fFOV;	            // Field of View
    float fDepth;			// Maximum rendering distance
    float fSpeed;			// Walking Speed
};


void initialization (gameStats *parameters ){

    
    parameters->fPlayerX = 1.5f;			// Player Start Position
    parameters->fPlayerY = 1.5f;
    parameters->fPlayerAngle = 0.0f;		// Player Start Rotation
    parameters->fFOV = 3.14159f / 5.0f;	    // Field of View
    parameters->fDepth = 16.0f;			    // Maximum rendering distance
    parameters->fSpeed = 3.0f;			    // Walking Speed
}


void startGame(int nMapSize, int seed) {
    char* screen = new char[nScreenWidth * nScreenHeight];


    gameStats *parameters = new gameStats();

    initialization(parameters);

    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);
    DWORD dwBytesWritten = 0;

    string map = getMap(nMapSize, seed);

    auto tp1 = chrono::system_clock::now();
    auto tp2 = chrono::system_clock::now();

    while (parameters->fPlayerY < nMapSize - 0.5) {

        tp2 = chrono::system_clock::now();
        chrono::duration<float> elapsedTime = tp2 - tp1;
        tp1 = tp2;
        float fElapsedTime = elapsedTime.count();

        if (GetAsyncKeyState((unsigned short)'A') & 0x8000)
            parameters->fPlayerAngle -= (1.0f) * fElapsedTime;

        if (GetAsyncKeyState((unsigned short)'D') & 0x8000)
            parameters->fPlayerAngle += (1.0f) * fElapsedTime;


        if (GetAsyncKeyState((unsigned short)'W') & 0x8000) {
            parameters->fPlayerX += sinf(parameters->fPlayerAngle) * parameters->fSpeed * fElapsedTime;
            parameters->fPlayerY += cosf(parameters->fPlayerAngle) * parameters->fSpeed * fElapsedTime;
            if (map[(int)parameters->fPlayerX * nMapSize + (int)parameters->fPlayerY] == '#')
            {
                parameters->fPlayerX -= sinf(parameters->fPlayerAngle) * parameters->fSpeed * fElapsedTime;
                parameters->fPlayerY -= cosf(parameters->fPlayerAngle) * parameters->fSpeed * fElapsedTime;
            }
        }

        if (GetAsyncKeyState((unsigned short)'S') & 0x8000) {
            parameters->fPlayerX -= sinf(parameters->fPlayerAngle) * parameters->fSpeed * fElapsedTime;
            parameters->fPlayerY -= cosf(parameters->fPlayerAngle) * parameters->fSpeed * fElapsedTime;
            if (map[(int)parameters->fPlayerX * nMapSize + (int)parameters->fPlayerY] == '#')
            {
                parameters->fPlayerX += sinf(parameters->fPlayerAngle) * parameters->fSpeed * fElapsedTime;
                parameters->fPlayerY += cosf(parameters->fPlayerAngle) * parameters->fSpeed * fElapsedTime;
            }
        }


        for (int x = 0; x < nScreenWidth; x++) {
            float fRayAngle = (parameters->fPlayerAngle - parameters->fFOV / 2.0f) + ((float)x / (float)nScreenWidth) * parameters->fFOV;
            float fDistanceToWall = 0.0f;
            bool bHitWall = false;
            bool bBoundary = false;
            float fStepSize = 0.1f;

            float fEyeX = sinf(fRayAngle);
            float fEyeY = cosf(fRayAngle);

            while (!bHitWall && fDistanceToWall < parameters->fDepth) {
                fDistanceToWall += fStepSize;

                int nTestX = (int)(parameters->fPlayerX + fEyeX * fDistanceToWall);
                int nTestY = (int)(parameters->fPlayerY + fEyeY * fDistanceToWall);

                if (nTestX < 0 || nTestX >= nMapSize || nTestY < 0 || nTestY >= nMapSize) {
                    bHitWall = true;
                    fDistanceToWall = parameters->fDepth;
                }
                else {
                    if (map.c_str()[nTestX * nMapSize + nTestY] == '#') {
                        bHitWall = true;

                        vector<pair<float, float>> p; // distance, dot
                        for (int tx = 0; tx < 2; tx++) {
                            for (int ty = 0; ty < 2; ty++) {
                                float vy = (float)nTestY + ty - parameters->fPlayerY;
                                float vx = (float)nTestX + ty - parameters->fPlayerX;
                                float d = sqrt(vx * vx + vy * vy);
                                float dot = (fEyeX * vx / d) + (fEyeY * vy / d);
                                p.push_back(make_pair(d, dot));
                            }
                        }
                        sort(p.begin(), p.end(), [](const pair<float, float>& left, const pair<float, float>& right) {return left.first < right.first; });
                        //First two/three are closest (we will never see all four)
                        float fBound = 0.01;
                        if (acos(p.at(0).second) < fBound) bBoundary = true;
                        if (acos(p.at(1).second) < fBound) bBoundary = true;
                        if (acos(p.at(2).second) < fBound) bBoundary = true;

                    }
                }
            }

            int nCeiling = (float)(nScreenHeight / 2.0) - nScreenHeight / ((float)fDistanceToWall);
            int nFloor = nScreenHeight - nCeiling;

            char nShade = ' ';

            if (fDistanceToWall <= parameters->fDepth / 8.0f)
                nShade = 219;   // close
            else if (fDistanceToWall < parameters->fDepth / 4.0f)
                nShade = 178;
            else if (fDistanceToWall < parameters->fDepth / 2.0f)
                nShade = 177;
            else if (fDistanceToWall < parameters->fDepth)
                nShade = 176;
            else
                nShade = ' ';  // too far

            if (bBoundary)
                nShade = '|';

            for (int y = 0; y < nScreenHeight; y++) {
                if (y <= nCeiling)
                    screen[y * nScreenWidth + x] = ' ';
                else  if (y > nCeiling && y <= nFloor)
                    screen[y * nScreenWidth + x] = nShade;
                else {
                    float b = 1.0f - (((float)y - nScreenHeight / 2.0f) / ((float)nScreenHeight / 2.0f));
                    if (b < 0.25)
                        nShade = '#';
                    else if (b < 0.5)
                        nShade = 'x';
                    else if (b < 0.75)
                        nShade = '-';
                    else if (b < 0.9)
                        nShade = '.';
                    else
                        nShade = ' ';
                    screen[y * nScreenWidth + x] = nShade;
                }

            }
        }

        // Display Stats
        sprintf(screen, "X=%3.2f, Y=%3.2f, A=%3.2f FPS=%3.2f ", parameters->fPlayerX, parameters->fPlayerY, parameters->fPlayerAngle, 1.0f / fElapsedTime);

        //Display Map
        for (int nx = 0; nx < nMapSize; nx++)
            for (int ny = 0; ny < nMapSize; ny++)
            {
                screen[(ny + 1) * nScreenWidth + nx] = map[ny * nMapSize + nx];
            }
        screen[((int)parameters->fPlayerX + 1) * nScreenWidth + (int)parameters->fPlayerY] = 'P';




        screen[nScreenWidth * nScreenHeight - 1] = '\0';
        WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
    }

    char* finalString = "... .  . ......     ......   ......     .....    ......               .....   .   .     . .....     ......   ......     .....  .     . .....";


    int k = 0;
    for (int i = 0; i < nScreenWidth; i++)
        for (int j = 0; j < nScreenHeight; j++)
            if ((i >= 46 && i < 74) && (j >= 17 && j < 22)) {
                if (finalString[k] == '.')
                    screen[j * nScreenWidth + i] = 219;
                else
                    screen[j * nScreenWidth + i] = ' ';
                k++;
            }
            else
                screen[j * nScreenWidth + i] = ' ';

    WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);

    Sleep(3000);
    system("pause");
    return;
}