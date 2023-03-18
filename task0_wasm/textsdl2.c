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
SDL_Surface * make_surface_text_horizontal(char *, SDL_Color, SDL_Color, int);

#ifdef __EMSCRIPTEN__
char * get_param(char * param) {
    
    return (char *)EM_ASM_PTR({
            let p = UTF8ToString($0);
            let jsString = Module.URLParams.get(p);
            if (jsString == null) {
                jsString = p == "o" ?  "h" : "";
            }
            const lengthBytes = lengthBytesUTF8(jsString) + 1;
            let stringOnWasmHeap = _malloc(lengthBytes);
            stringToUTF8(jsString, stringOnWasmHeap, lengthBytes);
            return stringOnWasmHeap;
            }, param);
}
#endif

int main(int argc, char * argv[]) {
#ifdef __EMSCRIPTEN__
    EM_ASM(Module.URLParams = new URLSearchParams(window.location.search););
#endif

    init_app();
    
#ifdef __EMSCRIPTEN__
    change_text_to(get_param("t"), get_param("o"));
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

    SDL_Color color_fg = { 200, 228, 142 };
    SDL_Color color_bg = { 0, 0, 0 };

    int colon_width, colon_height;
    TTF_SizeUTF8(app.font, ":", &colon_width, &colon_height);

    int len_msg = strlen(msg);
    char prompt[len_msg+2];
    sprintf(prompt, "%s:", msg);

    int prompt_wrap_len = WINDOW_WIDTH/2 - colon_width;

    SDL_Surface * surface = make_surface_text_horizontal(
            prompt,
            color_fg,
            color_bg,
            prompt_wrap_len
            );
    app.text->rect_width = surface->w;
    app.text->rect_height = surface->h;
    SDL_Texture * texture = SDL_CreateTextureFromSurface(app.renderer, surface);
    SDL_FreeSurface(surface);

    if (app.text->texture != NULL) {
        SDL_DestroyTexture(app.text->texture);
    }
    app.text->texture = texture;

}

SDL_Surface * make_surface_text_horizontal(char * text, SDL_Color fg, SDL_Color bg, int wrap_length) {
    SDL_Surface * surface = TTF_RenderUTF8_Shaded_Wrapped(
            app.font,
            text,
            fg,
            bg,
            wrap_length
            );
    return surface;
}

void draw_text() {
    SDL_Rect rect = {0, 0, app.text->rect_width, app.text->rect_height};
    SDL_RenderCopy(app.renderer, app.text->texture, &rect, &rect);
    SDL_RenderPresent(app.renderer);
}
