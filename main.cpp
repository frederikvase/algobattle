/*
* To compile and run, execute the following command:
g++ *.cpp passo/*.cpp -o main -lsfml-graphics -lsfml-window -lsfml-system && ./main
*/

#include "app.h"
#include "passo/passo.h"

#include "passo/randomagent.h"
#include "passo/slowrandom.h"

#include <vector>

int main() {
    std::vector<Agent<Passo>*> agents = {
        new RandomAgent(),
        new SlowRandom(),
    };

    App<Passo> app(agents,  4500ms);
    app.run();

    return 0;
}
