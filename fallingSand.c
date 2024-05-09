#include <windows.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define SAND_LIMIT 50
#define HEIGHT 20
#define WIDTH 20
// @todo: make it dynamic

typedef struct singleSandParticle{
    char particle;
    COORD prevPos;
    COORD currPos;
} sand;

sand* createSand(uint8_t x, uint8_t y){
    sand* new = (sand*) calloc(1, sizeof(sand));
    new->currPos.X = x;
    new->currPos.Y = y;

    char particles[] = "#$%&*N.*-";
    new->particle = particles[rand() % 10]; 

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

void handleSand(sand** sands, uint8_t* sandLength, HANDLE hConsole, uint8_t* sandHeight){
    for(uint8_t i = 0; i < *sandLength; i++){
        if(sands[i] == NULL) continue;
        
        if(sands[i]->currPos.Y+1 >= sandHeight[sands[i]->currPos.X]){
            sands[i]->prevPos = sands[i]->currPos;

            if( sands[i]->currPos.X != 0 && sandHeight[sands[i]->currPos.X-1] > sandHeight[sands[i]->currPos.X] ){
                sands[i]->currPos.Y += 1;
                sands[i]->currPos.X -= 1;
            }else if( sands[i]->currPos.X != WIDTH-1 && sandHeight[sands[i]->currPos.X+1] > sandHeight[sands[i]->currPos.X] ){
                sands[i]->currPos.Y += 1;
                sands[i]->currPos.X += 1;
            }else{
                sandHeight[sands[i]->currPos.X]-=1;
                removeSand(i,sands,sandLength);
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
        if(sands[i] == NULL) continue;

        SetConsoleCursorPosition(hConsole, sands[i]->prevPos);
        printf(" ");

        SetConsoleCursorPosition(hConsole, sands[i]->currPos);
        printf("%c",sands[i]->particle);
    }
}

int main() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole == INVALID_HANDLE_VALUE) return EXIT_FAILURE;

    COORD cursorPos = {0,0};
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

        if(GetAsyncKeyState(VK_RIGHT) & 0x8000 && cursorPos.X != WIDTH){
            SetConsoleCursorPosition(hConsole,cursorPos);
            printf(" ");
            cursorPos.X += 1;
        }
        
        if (GetAsyncKeyState(VK_RETURN) & 0x8000 && sandHeight[cursorPos.X] > 1) {
            sand* new  = createSand(cursorPos.X,cursorPos.Y);
            addSand(new, sands, &sandLength);
            // printf("%d",sandLength);
        }
        
        SetConsoleCursorPosition(hConsole,cursorPos);
        printf("");


        handleSand(sands, &sandLength, hConsole, sandHeight);
        renderSand(sands, sandLength, hConsole);
        Sleep(100);

    }

    return 0;
}
