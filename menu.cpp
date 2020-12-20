#include <iostream>
#include "menu.h"

int getVariant(int count) {
    int variant;
    char s[100];
    scanf("%s", s);

    while (sscanf(s, "%d", &variant) != 1 || variant < 1 || variant > count) {
        printf("Incorrect input. Try again: "); // выводим сообщение об ошибке
        scanf("%s", s); // считываем строку повторно
    }

    return variant;
}


void printMenu() {
    system("cls");  // очищаем экран
    printf("What do you want to do?\n");
    printf("1. Start\n");
    printf("2. Settings\n");
    printf("3. Exit\n");
    printf(">");
}

void settings(int &nMapSize, int &seed) {
    int variant = 0;
    do {
        system("cls");
        printf("1. Map size = %i \n", nMapSize);
        printf("2. Map seed = %i \n", seed);
        //printf("3. Player's speed = %f \n", fSpeed);
        printf("3. Close\n");
        printf(">");

        variant = getVariant(3);

        switch (variant) {
        case 1:
            int mapSize;
            printf("\n\n\nEnter new value for map size (odd only) - ");
            scanf("%i", &mapSize);
            nMapSize = mapSize;
            //nMapWidth = mapSize;
            break;
        case 2:
            int mapSeed;
            printf("\n\n\nEnter new value for map seed  - ");
            scanf("%i", &mapSeed);
            seed = mapSeed;
            break;

            /*case 3:
                int playersSpeed;
                printf("\n\n\nEnter new value for player's speed  - ");
                scanf("%f", &playersSpeed);
                fSpeed = playersSpeed;
                break;*/
        }

        if (variant != 3)
            system("pause");

    } while (variant != 3);

    //return;
}