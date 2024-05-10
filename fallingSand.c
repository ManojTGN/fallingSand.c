#include <windows.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define SAND_LIMIT 1000

typedef struct singleSandParticle{
    bool isPlaced;
    char* color;
    char particle;
    COORD prevPos;
    COORD currPos;
} sand;

sand* createSand(uint8_t x, uint8_t y, uint8_t color){
    sand* new = (sand*) calloc(1, sizeof(sand));
    new->currPos.X = x;
    new->currPos.Y = y;

    char particles[] = "#$%&*N.*-";
    new->particle = particles[rand() % 10]; 

    new->color = (char*) calloc(12,sizeof(char));
    snprintf(new->color,12,"\x1b[38;5;%dm",color); //rand()%(255+1));

    return new;
}

bool addSand(sand* new, sand** sands, uint8_t* sandLength){
    if(new == NULL || *sandLength == SAND_LIMIT) return false;

    for(uint8_t i = 0; i < *sandLength; i++){
        if(sands[i] == NULL){
            sands[i] = new;
            return true;
        }
    }

    sands[*sandLength] = new;
    *sandLength += 1;

    return true;
}

void removeSand(uint8_t index, sand** sands, uint8_t* sandLength){
    if(index == *sandLength-1) *sandLength--;
    free(sands[index]);
    sands[index] = NULL;
}

void handleSand(sand** sands, uint8_t* sandLength, HANDLE hConsole, uint8_t* sandHeight, uint8_t WIDTH){
    for(uint8_t i = 0; i < *sandLength; i++){
        if(sands[i] == NULL || sands[i]->isPlaced) continue;
        
        if(sands[i]->currPos.Y+1 >= sandHeight[sands[i]->currPos.X]){
            sands[i]->prevPos = sands[i]->currPos;

            bool left,right;
            left = sands[i]->currPos.X != 0 && sandHeight[sands[i]->currPos.X-1] > sandHeight[sands[i]->currPos.X];
            right = sands[i]->currPos.X != WIDTH-1 && sandHeight[sands[i]->currPos.X+1] > sandHeight[sands[i]->currPos.X];

            if(left && right){
                if(rand()%2==0) left = false;
                else right = false;
            }

            if(left){
                sands[i]->currPos.Y += 1;
                sands[i]->currPos.X -= 1;
            }else if(right){
                sands[i]->currPos.Y += 1;
                sands[i]->currPos.X += 1;
            }else{
                sandHeight[sands[i]->currPos.X]-=1;
                // removeSand(i,sands,sandLength);
                sands[i]->isPlaced = true;
                continue;
            }

            continue;
        }
        
        sands[i]->prevPos = sands[i]->currPos;
        sands[i]->currPos.Y += 1;
    }
    
}

void renderSand(sand** sands, uint8_t sandLength, HANDLE hConsole){
    for(uint8_t i = 0; i < sandLength; i++){
        if(sands[i] == NULL || sands[i]->isPlaced) continue;

        SetConsoleCursorPosition(hConsole, sands[i]->prevPos);
        printf(" ");

        SetConsoleCursorPosition(hConsole, sands[i]->currPos);
        printf("%s%c",sands[i]->color,sands[i]->particle);
    }
}

int main() {
    system("cls");
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole == INVALID_HANDLE_VALUE) return EXIT_FAILURE;

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    uint8_t HEIGHT,WIDTH;
  
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    WIDTH = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    HEIGHT = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

    uint8_t colorIndex = 1;
    COORD cursorPos = {0,0};
    COORD infoPos = {WIDTH+1,0};

    uint8_t sandLength = 0;
    sand** sands = (sand**) calloc(SAND_LIMIT, sizeof(sand*));

    uint8_t* sandHeight = (uint8_t*) calloc(WIDTH, sizeof(uint8_t));
    memset(sandHeight,HEIGHT,sizeof(uint8_t)*WIDTH);
    
    while(1){
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) break;

        if(GetAsyncKeyState(VK_LEFT) & 0x8000 && cursorPos.X != 0    ){
            SetConsoleCursorPosition(hConsole,cursorPos);
            printf(" ");
            cursorPos.X -= 1;
        }

        if(GetAsyncKeyState(VK_RIGHT) & 0x8000 && cursorPos.X != WIDTH-1){
            SetConsoleCursorPosition(hConsole,cursorPos);
            printf(" ");
            cursorPos.X += 1;
        }
        
        if (GetAsyncKeyState(VK_RETURN) & 0x8000 && sandHeight[cursorPos.X] > 1) {
            sand* new  = createSand(cursorPos.X,cursorPos.Y, colorIndex);
            addSand(new, sands, &sandLength);
            colorIndex++;
        }
        
        SetConsoleCursorPosition(hConsole,cursorPos);
        printf("");



        handleSand(sands, &sandLength, hConsole, sandHeight, WIDTH);
        renderSand(sands, sandLength, hConsole);
        Sleep(2);

    }

    return 0;
}
