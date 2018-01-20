#include <iostream>
#include <fstream>
#include <string>
#include "game.h"

int main() {

    game = new Game();

    game->addDirectionalLight(1.0f, glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 0.0f, -1.0f), glm::vec3(1.0f));
    game->addDirectionalLight(0.5f, glm::vec3(1.0f, 0.5f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f));

    game->addPointLight(5.0f, glm::vec3(1.0f, 0.5f, 0.5f), glm::vec3(-0.8f, 1.5f, 1.0f), 1.0f, 1.0f, 1.0f, glm::vec3(0.0f));
    game->addPointLight(3.0f, glm::vec3(1.0f, 1.0f, 0.2f), glm::vec3(-1.0f, 1.5f, -10.0f), 1.0f, 1.0f, 1.0f, glm::vec3(0.0f));
    
    Model& nanosuit = game->addModel("models/nanosuit/nanosuit.obj", glm::vec3(0.0f, 0.0f, -10.0f), glm::vec3(0.0f), glm::vec3(1.0f));
    Model& plane = game->addModel("models/plane/plane.obj", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(4.0f));

    game->start();

    return 0;

}