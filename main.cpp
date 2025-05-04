/*
* To compile and run, execute the following command:
g++ *.cpp passo/*.cpp -o main -lsfml-graphics -lsfml-window -lsfml-system && ./main
*/

#include "app.h"
#include "passo/passo.h"

int main() {
    Passo game;

    App app(game);
    app.run();

    return 0;
}
