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

typedef struct  {
    char * msg;
    orientation d;
    int rect_width;
    int rect_height;
    SDL_Texture * texture;
} TextArgs;

typedef struct app {
    SDL_Renderer * renderer;
    SDL_Window * window;
    SDL_Event * event;
    TTF_Font * font;
    TextArgs * text;
    bool is_running;
} App;

static App app;


void init_app();
void deinit_app(int error);
void process_event();
void change_text_to(char *, char*);
void draw_text();
void draw_text_horizontal(const char *, SDL_Color , SDL_Color);
void draw_text_vertical(const char *, SDL_Color, SDL_Color);

int main(int argc, char * argv[]) {

    init_app();
    
    SDL_Surface * screen = SDL_GetWindowSurface(app.window);
        
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(process_event, 0, 1);
#else
    while (app.is_running) {
        process_event();
        SDL_Delay(16);
    }
    change_text_to("BYE!!!", "h");
    draw_text();
    SDL_Delay(1000);
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

    app.text = malloc(sizeof(TextArgs));
    app.text->rect_width = 0;
    app.text->rect_height = 0;
    app.text->texture = NULL;
    change_text_to("Hello", "h");
}

void deinit_app(int error) {

    if (app.text->texture != NULL) SDL_DestroyTexture(app.text->texture);
    if (app.text != NULL) free(app.text);

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
    draw_text();
}

void change_text_to(char * msg, char * o) {
    orientation orient = *o == 'v' ? vertical : horizontal;
    app.text->msg = msg;
    app.text->d = orient;

    int width, height;
    TTF_SizeUTF8(app.font, msg, &width, &height);
    
    app.text->rect_width = width;
    app.text->rect_height = height;

    SDL_Rect text_rect = {0, 0, width, height};

    SDL_Color color_fg = { 200, 228, 142 };
    SDL_Color color_bg = { 0, 0, 0 };

    SDL_Surface * surface = TTF_RenderUTF8_Shaded(
            app.font,
            msg,
            color_fg,
            color_bg
            );
    SDL_Texture * texture = SDL_CreateTextureFromSurface(app.renderer, surface);
    SDL_FreeSurface(surface);

    if (app.text->texture != NULL) {
        SDL_DestroyTexture(app.text->texture);
    }
    app.text->texture = texture;

}

void draw_text() {
    SDL_Rect rect = {0, 0, app.text->rect_width, app.text->rect_height};
    SDL_RenderCopy(app.renderer, app.text->texture, &rect, &rect);
    SDL_RenderPresent(app.renderer);
}

// TODO change to make text_horizontal
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

    SDL_FreeSurface(surface);
    /* SDL_RenderCopy(app.renderer, texture, &text_rect, &text_rect); */
    SDL_RenderCopy(app.renderer, texture, &text_rect, &text_rect);
    SDL_RenderPresent(app.renderer);
}

// TODO implement function
void draw_text_vertical(const char * text, SDL_Color color_fg, SDL_Color color_bg) {
}
