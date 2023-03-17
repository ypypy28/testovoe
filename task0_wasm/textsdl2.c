#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
#include <stdio.h>

#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 400


typedef enum { false, true } bool;
typedef enum { horizontal, vertical } orientation;

typedef struct app {
    SDL_Renderer * renderer;
    SDL_Window * window;
    SDL_Event * event;
    TTF_Font * font;
    bool is_running;
} App;

static App app;

struct TextArgs {
    char * msg;
    orientation d;
};

void init_app();
void deinit_app(int error);
void process_event();
void draw_text(struct TextArgs*);
void draw_text_horizontal(const char *, SDL_Color , SDL_Color);
void draw_text_vertical(const char *, SDL_Color, SDL_Color);

int main(int argc, char * argv[]) {

    init_app();
    
    SDL_Surface * screen = SDL_GetWindowSurface(app.window);
        
#ifdef __EMSCRIPTEN__
    struct TextArgs args = {"Hello", horizontal};
    emscripten_set_main_loop_arg((em_arg_callback_func) draw_text, &args, 0, 1);
#else
    struct TextArgs args = {"Hello, мир", horizontal};
    while (app.is_running) {
        process_event();
        SDL_Delay(16);
        draw_text(&args);
    }
#endif
            
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
    
    if (TTF_Init() < 0) {
        printf("Can not initialize SDL_TTF ( %s )", TTF_GetError());
        deinit_app(1);
    }

    app.font = TTF_OpenFont("resources/DejaVuSans.ttf", 28);
}

void deinit_app(int error) {

    TTF_CloseFont(app.font);
    TTF_Quit();

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


void draw_text(struct TextArgs * args) {
    SDL_Color color_fg = { 200, 228, 142 };
    SDL_Color color_bg = { 0, 0, 0 };

    switch (args->d) {
        case horizontal:
            draw_text_horizontal(args->msg, color_fg, color_bg);
            break;
        case vertical:
            draw_text_vertical(args->msg, color_fg, color_bg);
            break;
    }
}

void draw_text_horizontal(const char * text, SDL_Color color_fg, SDL_Color color_bg) {
    int width, height;
    TTF_SizeUTF8(app.font, text, &width, &height);

    SDL_Rect text_rect = {0, 0, width, height};
    SDL_Surface * surface = TTF_RenderUTF8_Shaded(
            app.font,
            text,
            color_fg,
            color_bg
            );
    SDL_Texture * texture = SDL_CreateTextureFromSurface(app.renderer, surface);

    SDL_RenderCopy(app.renderer, texture, &text_rect, &text_rect);
    SDL_RenderPresent(app.renderer);

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

// TODO implement function
void draw_text_vertical(const char * text, SDL_Color color_fg, SDL_Color color_bg) {
}
