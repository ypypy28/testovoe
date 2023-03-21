# Задача 0

Написать приложение WebAssembly отображающее горизонтальную или вертикальную текстовую строку.

Исходные данные:
1.	Средства разработки https://kripken.github.io/emscripten-site/docs/getting_started/downloads.html 
2.	Вывод на экран SDL2/OpenGL 
3.	Обработка шрифта SDL_TTF 2.x
4.	в качестве http сервера python -m http.server (входит в emscripten sdk)
5.	Параметры передаются http get запросом
6.	Основные действия должны быть выполнены на C/C++ на JS только настройка wasm модуля и передача параметров

Что должно получиться:
1.	wasm приложение отвечающее на http get запрос с двумя параметрами
2.	Пример запроса http://localhost/draw.html?o=v&t=Текст
3.	O - ориентация текста (h - горизонтально v - вертикально)
4.	T - текст для отображения
5.	В результате в браузере рисуется тест T горизонтально далее символ ':' далее текст T вертикально или горизонтально в зависимости от O.

## Использование

### В контейнере
Проще всего, понадобится только установленный Docker.
```bash
docker build . -t task0
docker run --rm -p 8000:8000 task0
```
Далее в браузере открывайте ссылку http://localhost:8000/draw.html?o=h&t=Текст

### Без контейнера
- Чтобы скомпилировать локально в wasm, минимально достаточно иметь установленный [emsdk](https://github.com/emscripten-core/emsdk):
```bash
emcc draw.c -sUSE_SDL=2 -sUSE_SDL_TTF=2 --preload-file resources --shell-file shell.html -o draw.html
```
Готовые файлы будут в корневом каталоге проекта, запустить можно:
```bash
emrun draw.html
```
- При наличии `cmake`, скомпилировать wasm можно следующими командами
```bash
emcmake cmake .
cmake --build .
```

Готовые файлы будут располагаться в папке `./build/public`. Запустить проще всего:
```bash
emrun build/public/draw.html
```

- Проект также может быть скомпилирован в исполняемый нативный код, для этого понадобится предустановить библиотеки `SDL2` и `SDL2_TTF`. В вашей операционной системе их установка может отличаться. Для Debian-based, например:
```bash
sudo apt update ; sudo apt install cmake libsdl2-dev libsdl2-ttf-dev
cmake .
cmake --build .
```
Готоый к запуску исполняемый файл будет находиться в папке `./build/bin/`.

- Однострочный рецепт [just](https://github.com/casey/just) скомпилирует и запустит просмотр wasm-проекта с переданными в кавычках параметрами:
```bash
just wasm build run-html "?t=Привет, мир&o=v"
```
