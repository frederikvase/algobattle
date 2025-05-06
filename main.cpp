/*
* To compile and run, execute the following command:
g++ *.cpp passo/*.cpp passo/bots/*.cpp -o main -lsfml-graphics -lsfml-window -lsfml-system && ./main
*/

#include "app.h"
#include "passo/passo.h"
#include "passo/passoboard.h"

#include "passo/bots/randomagent.h"
#include "passo/bots/slowrandom.h"
#include "passo/bots/defenceagent.h"

#include <vector>

int main() {
    std::vector<Agent<Passo>*> agents = {
        new RandomAgent(),
        new SlowRandom(),
        new DefenceAgent()
    };

    App<Passo, PassoBoard> app(agents,  30s);
    app.run();

    return 0;
}
