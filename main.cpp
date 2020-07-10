#include <SDL2/SDL.h>
#include <iostream>
#include <time.h>
#include <stdlib.h>

#define SCRWIDTH 800
#define SCRHEIGHT 600

int col, row, col2, row2;
int counter = 0;
int filecounter = 0;
SDL_Surface* atw_image;

Uint32 ColorToUint(int R, int G, int B)
{
    return (Uint32)((R << 16) + (G << 8) + (B << 0));
}

bool inrange(int col, int row, Uint32* atwpixels)
{
    return false;
    int range = 3;
    int i, j;
    bool found = false;
    for (i = col - range; i < col + range; i++) {
        for (j = row - range; j < row + range; j++) {
            if (atwpixels[j * 800 + i] == -1)
                found = true;
        }
    }
    return found;
}

void putpixel_white(int col, int row, Uint32* pixels)
{
    pixels[row * 800 + col] = ColorToUint(255, 255, 255);
}

void putpixel_pink(int col, int row, Uint32* pixels)
{
    pixels[row * 800 + col] = ColorToUint(255, 11, 207);
}

void putpixel_blue(int col, int row, Uint32* pixels)
{
    pixels[row * 800 + col] = ColorToUint(60, 11, 255);
}

bool emptyinmask(int col, int row, Uint32* mask)
{
    return (mask[row * 800 + col] == 0) ? true : false;
}

Uint32 my_callbackfunc2(Uint32 interval, void* pixels)
{
    SDL_Event event;
    SDL_UserEvent userevent;

    Uint32* actual_pixels = (Uint32*)pixels;

    /* In this example, our callback pushes an SDL_USEREVENT event
    into the queue, and causes our callback to be called again at the
    same interval: */

    userevent.type = SDL_USEREVENT;
    userevent.code = 1;
    userevent.data1 = NULL;
    userevent.data2 = NULL;

    event.type = SDL_USEREVENT;
    event.user = userevent;
    printf("%s", "Fired\n");
    counter += 1;

    if ((counter > 1) && !(counter % 15)) {
        printf("%s", "Time to change slides\n");
        filecounter += 1;
        filecounter = filecounter % 6;
        char filename[256];
        memset(filename, 0, sizeof(filename));

        col = 800 / 2;
        row = 600 / 2;
        col2 = 800 / 2;
        row2 = 600 / 2;

        sprintf(filename, "img%d.bmp", filecounter);
        printf("%s\n", filename);
        atw_image = SDL_LoadBMP(filename);
        memset(actual_pixels, 0, 800 * 600 * sizeof(Uint32));
    }
    SDL_PushEvent(&event);
    return (interval);
}
Uint32 my_callbackfunc(Uint32 interval, void* param)
{
    SDL_Event event;
    SDL_UserEvent userevent;

    /* In this example, our callback pushes an SDL_USEREVENT event
    into the queue, and causes our callback to be called again at the
    same interval: */

    userevent.type = SDL_USEREVENT;
    userevent.code = 0;
    userevent.data1 = NULL;
    userevent.data2 = NULL;

    event.type = SDL_USEREVENT;
    event.user = userevent;

    SDL_PushEvent(&event);
    return (interval);
}

int main(int argc, char** argv)
{
    bool leftMouseButtonDown = false;
    bool quit = false;
    int width = SCRWIDTH;
    int height = SCRHEIGHT;

    srand(time(NULL));
    SDL_Event event;

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("Patricia",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, 0);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_Texture* texture = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, width, height);
    Uint32* pixels = new Uint32[width * height];
    Uint32 delay = 1; /* To round it down to the nearest 10 ms */
    SDL_TimerID my_timer_id = SDL_AddTimer(delay, my_callbackfunc, (void*)0xdeadbeef);
    SDL_TimerID my_timer_id2 = SDL_AddTimer(100 * 40, my_callbackfunc2, pixels);
    atw_image = SDL_LoadBMP("img0.bmp");

    Uint32* mainmask = new Uint32[width * height];

    memset(mainmask, 0, width * height * sizeof(Uint32));
    memset(pixels, 0, width * height * sizeof(Uint32));

    col = height / 2;
    row = width / 2;

    col2 = height / 2;
    row2 = width / 2;

    int subpixc = 0;
    int subpixr = 0;

    int rc = 0;
    int rr = 0;
    int rc2 = 0;
    int rr2 = 0;
    while (!quit) {
        SDL_UpdateTexture(texture, NULL, pixels, width * sizeof(Uint32));

        SDL_WaitEvent(&event);

        switch (event.type) {
        case SDL_QUIT:
            quit = true;
            break;
        case SDL_MOUSEBUTTONUP:
            if (event.button.button == SDL_BUTTON_LEFT)
                leftMouseButtonDown = false;
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (event.button.button == SDL_BUTTON_LEFT)
                leftMouseButtonDown = true;
        case SDL_MOUSEMOTION:
            if (leftMouseButtonDown) {
                int mouseX = event.motion.x;
                int mouseY = event.motion.y;
                pixels[mouseY * width + mouseX] = 0;
            }
            break;
        }

        do {
            rc = rand();
            rr = rand();
            col += (rc % 15) - 7;
            row += (rr % 15) - 7;
            col = col % height;
            row = row % width;
        } while (!emptyinmask(col, row, mainmask));

        if (col > 0 && row > 0) {
            Uint32* atwpixels2 = (Uint32*)atw_image->pixels;
            putpixel_pink(row, col, pixels);
            if (inrange(row, col, atwpixels2))
                putpixel_white(row, col, pixels);
        }
        else {
            col -= (rc % 15) - 7;
            row -= (rr % 15) - 7;
        }
        do {
            rc2 = rand();
            rr2 = rand();
            col2 += (rc2 % 15) - 7;
            row2 += (rr2 % 15) - 7;
            col2 = col2 % height;
            row2 = row2 % width;
        } while (!emptyinmask(col2, row2, mainmask));

        if (col2 > 0 && row2 > 0) {
            Uint32* atwpixels = (Uint32*)atw_image->pixels;
            putpixel_blue(row2, col2, pixels);
            if (inrange(row, col, atwpixels))
                putpixel_white(row2, col2, pixels);
        }
        else {
            col2 -= (rc2 % 15) - 7;
            row2 -= (rr2 % 15) - 7;
        }

        Uint32* picpixels = (Uint32*)atw_image->pixels;
        for (int k = 0; k < 21; k++) {
            do {
                subpixc = rand() % width;
                subpixr = rand() % height;
            } while (!emptyinmask(subpixc, subpixr, mainmask));

            pixels[subpixr * 800 + subpixc] = picpixels[subpixr * 800 + subpixc];
        }

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

    delete[] pixels;
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

