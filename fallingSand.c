#include <windows.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define SAND_LIMIT 1000

typedef struct singleSandParticle{
    char* color;
    bool isPlaced;
    char particle;

    COORD prevPos;
    COORD currPos;

    struct singleSandParticle* _nextProcess;
} sand;

bool isSandOverlaps(sand* overlappingSand, sand* sands){
    if(sands==NULL) return false;

    for(sand* sandItr = sands; sandItr!=NULL; sandItr=sandItr->_nextProcess){
        if(overlappingSand != sandItr && overlappingSand->currPos.X==sandItr->currPos.X && overlappingSand->currPos.Y==sandItr->currPos.Y)
            return true;
    }

    return false;
}

sand* createSand(uint8_t x, uint8_t y, uint8_t color){
    sand* new = (sand*) calloc(1, sizeof(sand));
    new->currPos.X = x;
    new->currPos.Y = y;

    char particles[] = "#$%&*N.*-\"";
    new->particle = particles[rand() % 10]; 

    new->color = (char*) calloc(12,sizeof(char));
    snprintf(new->color,12,"\x1b[38;5;%dm",color); //rand()%(255+1));

    return new;
}

bool addSand(sand* new, sand* sands, uint8_t* sandLength){
    if(new == NULL || *sandLength == SAND_LIMIT) return false;

    if(sands==NULL){
        sands=new;
        return true;
    }

    sand* tmpSand;
    for(tmpSand = sands;tmpSand->_nextProcess!=NULL; tmpSand=tmpSand->_nextProcess);

    tmpSand->_nextProcess = new;
    *sandLength += 1;

    return true;
}

void removeSand(sand* rsand, sand* sands, uint8_t* sandLength){
    sand* sandItr;
    sand* prevSand = NULL;
    for(sandItr = sands;sandItr!=rsand; sandItr=sandItr->_nextProcess){
        prevSand = sandItr;
    }

    if(sandItr == NULL || prevSand == NULL) return;
    prevSand->_nextProcess = sandItr->_nextProcess;
    free(rsand);
}

void handleSand(sand* sands, uint8_t* sandLength, HANDLE hConsole, uint8_t* sandHeight, uint8_t WIDTH){
    for(sand* sandItr = sands; sandItr!=NULL; sandItr = sandItr->_nextProcess){
        if(sandItr->isPlaced) continue;
        
        if(sandItr->currPos.Y+1 >= sandHeight[sandItr->currPos.X]){
            sandItr->prevPos = sandItr->currPos;

            bool left,right;
            left = sandItr->currPos.X != 0 && sandHeight[sandItr->currPos.X-1] > sandHeight[sandItr->currPos.X];
            right = sandItr->currPos.X != WIDTH-1 && sandHeight[sandItr->currPos.X+1] > sandHeight[sandItr->currPos.X];

            if(left && right){
                if(rand()%2==0) left = false;
                else right = false;
            }

            if(left){
                sandItr->currPos.Y += 1;
                sandItr->currPos.X -= 1;

                if(isSandOverlaps(sandItr,sands)){
                    sandItr->currPos.Y -= 1;
                    sandItr->currPos.X += 1;
                }
            }else if(right){
                sandItr->currPos.Y += 1;
                sandItr->currPos.X += 1;
                if(isSandOverlaps(sandItr,sands)){
                    sandItr->currPos.Y -= 1;
                    sandItr->currPos.X -= 1;
                }
            }else{
                sandHeight[sandItr->currPos.X]-=1;
                // removeSand(i,sands,sandLength);
                sandItr->isPlaced = true;
                continue;
            }

            continue;
        }
        
        sandItr->prevPos = sandItr->currPos;
        sandItr->currPos.Y += 1;
    }
    
}

void renderSand(sand* sands, uint8_t sandLength, HANDLE hConsole){
    for(sand* sandItr = sands; sandItr!=NULL; sandItr = sandItr->_nextProcess){
        if(sandItr->isPlaced) continue;

        SetConsoleCursorPosition(hConsole, sandItr->prevPos);
        printf(" ");

        SetConsoleCursorPosition(hConsole, sandItr->currPos);
        printf("%s%c",sandItr->color,sandItr->particle);
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

    uint8_t sandLength = 0;
    sand* sands = NULL;

    uint8_t* sandHeight = (uint8_t*) calloc(WIDTH, sizeof(uint8_t));
    memset(sandHeight,HEIGHT,sizeof(uint8_t)*WIDTH);
    
    while(1){
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) break;

        if(GetAsyncKeyState(VK_LEFT) & 0x8000 && cursorPos.X != 0 )             cursorPos.X -= 1;
        else if(GetAsyncKeyState(VK_RIGHT) & 0x8000 && cursorPos.X != WIDTH-1)  cursorPos.X += 1;
        
        if (GetAsyncKeyState(VK_RETURN) & 0x8000 && sandHeight[cursorPos.X] > 1) {
            sand* new  = createSand(cursorPos.X,cursorPos.Y, colorIndex);
            if(sands==NULL){
                sands = new;
            }else{
                addSand(new, sands, &sandLength);
            }
            colorIndex++;
        }
        
        SetConsoleCursorPosition(hConsole,cursorPos);printf("");

        handleSand(sands, &sandLength, hConsole, sandHeight, WIDTH);
        renderSand(sands, sandLength, hConsole);
        Sleep(1);
    }

    return 0;
}
