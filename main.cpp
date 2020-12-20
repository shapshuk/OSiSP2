//#include <iostream>
//#include <vector>
//#include <utility>
//#include <algorithm>
//#include <chrono>
//using namespace std;
//#include <cmath>
//#include <cstdio>


#include <Windows.h>
#include "game.h"
#include "menu.h"

int nMapSize = 17;
int seed = 1;


int main()
{

    int variant;

    do {
        printMenu();
        variant = getVariant(3);
        switch (variant) {
            case 1:
                startGame(nMapSize, seed);
                break;
            case 2:
                settings(nMapSize, seed);
                break;
        }

        if (variant != 3)
            system("pause");

    } while (variant != 3);
    return 0;
}