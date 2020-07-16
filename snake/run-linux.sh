tcc -o snake food.c snake.c game.c main.c `sdl2-config --cflags --libs` -lSDL2_ttf && ./snake
