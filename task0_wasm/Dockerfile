FROM emscripten/emsdk:3.1.34 AS builder
RUN apt update \
    && apt upgrade -y 
RUN emsdk install 3.1.34
WORKDIR /src
RUN mkdir -p build/public
COPY . ./
RUN emcc draw.c -sUSE_SDL=2 -sUSE_SDL_TTF=2 --preload-file resources -o build/public/draw.html

FROM python:3.11-alpine
WORKDIR /var/www
COPY --from=builder /src/build/public .
EXPOSE 8000
CMD ["python3", "-m", "http.server"]
