#include <SDL2/SDL.h>
#include <stdio.h>

#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 400


typedef enum { false, true } bool;

typedef struct app {
    SDL_Renderer * renderer;
    SDL_Window * window;
    SDL_Event * event;
    bool is_running;
} App;

static App app;

void init_app();
void deinit_app(int error);
void process_event();

int main(int argc, char * argv[]) {

    init_app();
    
    SDL_Surface * screen = SDL_GetWindowSurface(app.window);
    
    int i = 0;
    while (app.is_running) {
        process_event();
        SDL_Delay(16);
        SDL_SetRenderDrawColor(app.renderer, i, 0, i, 255);
        SDL_RenderClear(app.renderer);
        SDL_SetRenderDrawColor(app.renderer, 255, 255, 255, 255);
        SDL_RenderDrawLine(app.renderer, i+10, i, 100, 200);

        SDL_RenderPresent(app.renderer);
        i = (i+1)%255;
    }

            
    if (screen != NULL) {
        SDL_FreeSurface(screen);
    }
    printf("Have a nice day!");

    deinit_app(0);
    return 0;
}

void init_app() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Can not initialize SDL ( %s )\n", SDL_GetError());
        exit(1);
    }

    app.window = SDL_CreateWindow(
            "Text writer",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            WINDOW_WIDTH,
            WINDOW_HEIGHT,
            SDL_WINDOW_OPENGL
            );

    if (app.window == NULL) {
        printf("Can not open window ( %s )\n", SDL_GetError());
        deinit_app(1);
    }

    app.renderer = SDL_CreateRenderer(
            app.window,
            -1,
            SDL_RENDERER_ACCELERATED
            );
    
    if (app.renderer == NULL) {
        printf("Can not create renderer ( %s )", SDL_GetError());
        deinit_app(1);
    }

    SDL_Event * event = malloc(sizeof(SDL_Event));
    if (event == NULL) {
        printf("Can not initialize event");
        deinit_app(1);
    }

    app.event = event;
    app.is_running = true;
}

void deinit_app(int error) {
    if (app.event != NULL) {
        free(app.event);
    }
    
    if (app.renderer != NULL) {
        SDL_DestroyRenderer(app.renderer);
    }

    if (app.window != NULL) {
        SDL_DestroyWindow(app.window);
    }
    SDL_Quit();
    exit(error);
}

void process_event() {
    while (SDL_PollEvent(app.event)) {
        switch (app.event->type) {
            case SDL_QUIT:
                app.is_running = false;
                break;
            default:
                break;
        }
    }
}
