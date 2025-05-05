/*
* To compile and run, execute the following command:
g++ *.cpp passo/*.cpp -o main -lsfml-graphics -lsfml-window -lsfml-system && ./main
*/

#include "app.h"
#include "passo/passo.h"

#include "passo/randomagent.h"

#include <vector>

int main() {
    std::vector<Agent<Passo>> agents = {
        RandomAgent(),
        RandomAgent(),
    };

    App<Passo> app(agents);
    app.run();

    return 0;
}
