#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
#include <stdio.h>

#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 400
#define FONT_SIZE 20


typedef enum { false, true } bool;
typedef enum { horizontal, vertical } orientation;

typedef struct  {
    char * msg;
    orientation d;
    int rect_width;
    int rect_height;
    SDL_Texture * texture;
    SDL_Color color_fg;
    SDL_Color color_bg;
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
void change_text_to(char * msg, char * o); 
void draw_text();
size_t len_UTF8char(const char * c); 
SDL_Surface * make_surface_text_horizontal(char * text, SDL_Color fg, SDL_Color bg, int wrap_length); 
size_t strcpy_with_newline_after_each_char(char * dst, const char * src, size_t len_to_copy); 
size_t calc_bytes_in_wrapped_line(const char * src, int len_wrap); 
int calc_last_line_width(const char * src, int len_unwrap, int len_wrap); 
void put_horizontal_text_on_surface(const char * text, SDL_Surface * surface, int x, int y);
void put_next_horizontal_text_on_surface(const char * src, SDL_Surface * surface, int start_x, int start_y, int continue_x, int continue_y );
void put_vertical_text_on_surface(const char * src, SDL_Surface * surface, int start_x, int start_y, int line_height, size_t len_src);

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
            "Draw",
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

    app.font = TTF_OpenFont("resources/DejaVuSans.ttf", FONT_SIZE);

    app.text = malloc(sizeof(TextArgs));
    app.text->rect_width = 0;
    app.text->rect_height = 0;
    app.text->texture = NULL;
    SDL_Color color_fg = { 200, 228, 142 };
    SDL_Color color_bg = { 0, 0, 0 };
    app.text->color_fg = color_fg;
    app.text->color_bg = color_bg;
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

    int colon_width, colon_height;
    TTF_SizeUTF8(app.font, ":", &colon_width, &colon_height);

    int len_msg = strlen(msg);
    char prompt[len_msg+2];
    sprintf(prompt, "%s:", msg);

    int prompt_wrap_len = WINDOW_WIDTH;

    int prompt_unwrapped_width, prompt_unwrapped_height;
    TTF_SizeUTF8(app.font, prompt, &prompt_unwrapped_width, &prompt_unwrapped_height);

    int prompt_last_line_width = prompt_unwrapped_width;
    if (prompt_wrap_len < prompt_unwrapped_width) {
        prompt_last_line_width = calc_last_line_width(
                prompt,
                prompt_unwrapped_width,
                prompt_wrap_len);
    }

    SDL_Surface * surface = SDL_CreateRGBSurface(
            0,
            WINDOW_WIDTH,
            WINDOW_HEIGHT,
            32,
            0,0,0,0);
    SDL_Surface * prompt_surface = make_surface_text_horizontal(
            prompt,
            app.text->color_fg,
            app.text->color_bg,
            prompt_wrap_len
            );

    SDL_BlitSurface(prompt_surface, NULL, surface, NULL);
    
    if (orient == horizontal) {
        put_next_horizontal_text_on_surface(
                msg,
                surface,
                prompt_last_line_width,
                prompt_surface->h - prompt_unwrapped_height,
                0,
                prompt_surface->h);
    } else if (orient == vertical) {
        put_vertical_text_on_surface(
                msg,
                surface,
                prompt_last_line_width,
                prompt_surface->h - prompt_unwrapped_height,
                prompt_unwrapped_height,
                len_msg);
    } else {
        printf("ORIENTATION %s IS NOT IMPLEMENTD", o);
        app.is_running = false;
        deinit_app(1);
    }

    app.text->rect_width = surface->w;
    app.text->rect_height = surface->h;
    SDL_Texture * texture = SDL_CreateTextureFromSurface(app.renderer, surface);
    SDL_FreeSurface(prompt_surface);
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

size_t len_UTF8char(const char * c) {
    if (c == NULL)
        return 0;

    if (((c[0] & 0xF8) == 0xF0) 
        && ((c[1] & 0xC0) == 0x80)
        && ((c[2] & 0xC0) == 0x80)
        && ((c[3] & 0xC0) == 0x80)) {
                return 4;
    } else if (((c[0] & 0xF0) == 0xE0) 
            && ((c[1] & 0xC0) == 0x80)
            && ((c[2] & 0xC0) == 0x80)) {
                return 3;
    } else if (((c[0] & 0xE0) == 0xC0)
            && ((c[1] & 0xC0) == 0x80)) {
                return 2;
    } else if ((c[0] & 0x80) == 0x00) {
        return 1;
    }
    return 0;
}

size_t strcpy_with_newline_after_each_char(char * dst, const char * src, size_t len_to_copy) {
    char vertical_text[len_to_copy*2 + 1];
    size_t i = 0;
    size_t j = 0;
    size_t k = 0;
    while (i < len_to_copy) {
        size_t len_char = len_UTF8char(&src[i]);
        if (len_char == 0) {
            printf("BAD CHAR %c\n", src[i]);
            i++;
            continue;
        }
        for (k = 0; k < len_char; k++, j++) {
            vertical_text[j] = src[i+k];
        }
        vertical_text[j] = '\n';
        j++;
        i += len_char;
    }
    vertical_text[j] = '\0';
    strcpy(dst, vertical_text);
    return j;
}

int calc_last_line_width(const char * src, int len_unwrap, int len_wrap) {
    int last_line_width = len_unwrap;
    char * last_line = (char *) src;

    int line_width = len_unwrap;
    int k;
    size_t len_ch, bytes_after_space;
    int chars_in_line = 0;
    while (line_width != 0) {
        last_line_width = line_width;
        bytes_after_space = -1;
        for (k = 0; k < chars_in_line; k++) {
            len_ch = len_UTF8char(last_line);
            if (len_ch == 0) {
                last_line++;
                continue;
            }
            if (last_line[0] == ' ') {
                bytes_after_space = 0;
            } else if (bytes_after_space != -1) {
                bytes_after_space += len_ch;
            }
            last_line += len_ch;
        }
        if (last_line[0] != ' ' && last_line[0] != '\0' && bytes_after_space != -1) {
            last_line -= bytes_after_space;
        }
        TTF_MeasureUTF8(app.font, last_line, len_wrap, &line_width, &chars_in_line);
    }
    return last_line_width;
}

void put_horizontal_text_on_surface(const char * text, SDL_Surface * surface, int x, int y) {
    if (*text == '\0') return;
    SDL_Surface * text_surface = TTF_RenderUTF8_Shaded_Wrapped(
            app.font,
            text,
            app.text->color_fg,
            app.text->color_bg,
            WINDOW_WIDTH-x
            );
    SDL_Rect dstrect = {
        x,
        y,
        text_surface->w,
        text_surface->h
    };
    SDL_BlitSurface(text_surface, NULL, surface, &dstrect);
    SDL_FreeSurface(text_surface);
}

size_t calc_bytes_in_wrapped_line(const char * src, int len_wrap) {
    size_t bytes_after_space = -1;
    size_t bytes_in_line = 0;
    size_t len_ch;
    int line_width, chars_in_line;
    TTF_MeasureUTF8(app.font, src, len_wrap, &line_width, &chars_in_line);

    for (int k = 0; k < chars_in_line; k++) {
        len_ch = len_UTF8char(src);
        if (len_ch == 0) {
            src++;
            bytes_in_line++;
            continue;
        }
        if (src[0] == ' ') {
            bytes_after_space = 0;
        } else if (bytes_after_space != -1) {
            bytes_after_space += len_ch;
        }
        src += len_ch;
        bytes_in_line += len_ch;
    }
    if (src[0] != ' ' && src[0] != '\0' && bytes_after_space != -1) {
        src -= bytes_after_space;
        bytes_in_line -= bytes_after_space;
    }
    return bytes_in_line;
}

void put_next_horizontal_text_on_surface(const char * src, SDL_Surface * surface, int start_x, int start_y, int continue_x, int continue_y ) {
    if (*src == '\0') return;
    char * text = (char *)src;
    int line_width, chars_in_line;
    TTF_MeasureUTF8(
            app.font,
            text,
            WINDOW_WIDTH - start_x,
            &line_width,
            &chars_in_line);
    
    int bytes_on_line_after_prompt = calc_bytes_in_wrapped_line(
            text,
            WINDOW_WIDTH - start_x
            );

    char text_after_prompt[bytes_on_line_after_prompt+1];
    strncpy(text_after_prompt, text, bytes_on_line_after_prompt);
    text_after_prompt[bytes_on_line_after_prompt] = '\0';
    put_horizontal_text_on_surface(
            text_after_prompt,
            surface,
            start_x,
            start_y);

    text += bytes_on_line_after_prompt;
    put_horizontal_text_on_surface(
            text,
            surface,
            continue_x,
            continue_y);
}

void put_vertical_text_on_surface(const char * src, SDL_Surface * surface, int start_x, int start_y, int line_height, size_t len_src) {
    if (*src == '\0') return;
    char * msg = (char *) src;
    size_t vertical_lines = (WINDOW_HEIGHT - start_y) / line_height;
    size_t len_ch, bytes_to_cpy;
    int bytes_after_space;
    int text_width, text_height, j; 
    char text[len_src+1];
    char vertical_text[vertical_lines*2+1];
    size_t printed_bytes = 0;
    for (int i = 0; i < len_src; i++) {
        if (start_x >= WINDOW_WIDTH || *msg == '\0') {
            break;
        }
        // delete ' ' from start of a column
        while (msg[0] == ' ') {
            msg++;
            printed_bytes++;
        }
        bytes_to_cpy = 0;
        bytes_after_space = -1;
        for (j = 0; j < vertical_lines && printed_bytes + bytes_to_cpy <= len_src; j++) {
            len_ch = len_UTF8char(&msg[bytes_to_cpy]);
            if (msg[bytes_to_cpy] == ' ') {
                bytes_after_space = 0;
            } else if (bytes_after_space != -1) {
                bytes_after_space += len_ch; 
            }
            bytes_to_cpy += len_ch;
        }
        if (bytes_after_space > 0) {
            bytes_to_cpy -= bytes_after_space;
        }
        strncpy(text, msg, bytes_to_cpy);
        text[bytes_to_cpy] = '\0';
        msg += bytes_to_cpy;
        printed_bytes += bytes_to_cpy;
        int n_bytes = strcpy_with_newline_after_each_char(vertical_text, text, bytes_to_cpy);
        vertical_text[n_bytes] = '\0';
        put_horizontal_text_on_surface(
                vertical_text,
                surface,
                start_x,
                start_y);
        start_x += FONT_SIZE;
    }
}
