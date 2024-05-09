#include <windows.h>
#include <stdint.h>

#define WIDTH 100
// @todo: make it dynamic

typedef struct singleSandParticle{
    COORD pos;
    struct singleSandParticle* _next;
} sand;

int main() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    // HWND consoleWindow = GetConsoleWindow();

    if (hConsole == INVALID_HANDLE_VALUE) return EXIT_FAILURE;

    POINT cursorPos;
    uint8_t* sandHeight = (uint8_t*) calloc(WIDTH, sizeof(uint8_t));
    sand* head = NULL;//(sand*) calloc(1, sizeof(sand));
    sand* end = head;

    while(1){
        if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
            GetCursorPos(&cursorPos);

            if(head == NULL){
                head = (sand*) calloc(1, sizeof(sand));
                head->pos.X = cursorPos.x;
                head->pos.Y = cursorPos.y;
                end = head;
            }else{
                end->_next = (sand*) calloc(1, sizeof(sand));
                end->_next->pos.X = cursorPos.x;
                end->_next->pos.Y = cursorPos.y;
            }
        }

        SetConsoleCursorPosition(hConsole, newPosition);
        WriteConsoleA(hConsole, "New content in the cleared region!\n", 34, NULL, NULL);


        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) break;
    }

    return 0;
}
