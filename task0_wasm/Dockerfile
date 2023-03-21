FROM docker.io/emscripten/emsdk:3.1.34 AS builder
RUN apt update \
    && apt upgrade -y  \
    && emsdk activate 3.1.34 \
    && embuilder build sdl2 sdl2_ttf
WORKDIR /src
RUN mkdir -p build/public
COPY . ./
RUN emcc draw.c -sUSE_SDL=2 -sUSE_SDL_TTF=2 --preload-file resources --shell-file shell.html -o build/public/draw.html

FROM docker.io/python:3.11-alpine
WORKDIR /var/www
COPY --from=builder /src/build/public .
EXPOSE 8000
CMD ["python3", "-m", "http.server"]
