#include <iostream>
#include <fstream>
#include <string>
#include "game.h"

int main() {

    game = new Game();

    game->addDirectionalLight(1.0f, glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 0.0f, -1.0f), glm::vec3(1.0f));
    game->addDirectionalLight(0.5f, glm::vec3(1.0f, 0.5f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f));

    game->addPointLight(5.0f, glm::vec3(1.0f, 0.5f, 0.5f), glm::vec3(-3.0f, 3.0f, 3.0f), 0.0f, 0.0f, 1.0f, glm::vec3(0.0f));   //red
    game->addPointLight(5.0f, glm::vec3(1.0f, 1.0f, 0.2f), glm::vec3(-6.0f, 1.0f, -10.0f), 0.0f, 0.0f, 1.0f, glm::vec3(0.0f)); //yellow

    //Model& sphere = game->addModel("models/sphere/sphere.obj", glm::vec3(3.0f, 1.2f, 2.0f), glm::vec3(0.0f), glm::vec3(1.0f));

    Model& nanosuit = game->addModel("models/nanosuit/nanosuit.obj", glm::vec3(0.0f, 0.0f, -10.0f), glm::vec3(0.0f), glm::vec3(1.0f));
    Model& digits = game->addModel("models/digits/digits.dae", glm::vec3(0.0f, 3.0f, 0.0f), glm::vec3(0.0f), glm::vec3(1.0f));

    Model& sphere2 = game->addModel("models/sphere2/sphere2.dae", glm::vec3(3.0f, 1.2f, 2.0f), glm::vec3(0.0f), glm::vec3(1.0f));
    Model& mirrorCube = game->addModel("models/mirrorcube/mirrorcube.dae", glm::vec3(0.0f, 1.001f, 3.0f), glm::vec3(0.0f), glm::vec3(1.0f));

    Model& plane = game->addModel("models/plane/plane.obj", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(4.0f));
    Model& cube = game->addModel("models/cube/cube.obj", glm::vec3(0.0f, 1.001f, 0.0f), glm::vec3(0.0f), glm::vec3(1.0f));
    Model& grass = game->addModel("models/grass/grass.obj", glm::vec3(3.0f, 4.0f, 0.0f), glm::vec3(0.0f), glm::vec3(4.0f), true, GL_CLAMP_TO_EDGE);

    Model& window1 = game->addModel("models/window/window.obj", glm::vec3(-3.0f, 1.0f, 0.0f), glm::vec3(0.0f), glm::vec3(1.0f), true);
    Model& window3 = game->addModel("models/window/window.obj", glm::vec3(-3.0f, 1.0f, 2.0f), glm::vec3(0.0f), glm::vec3(1.0f), true);
    Model& window2 = game->addModel("models/window/window.obj", glm::vec3(-3.0f, 1.0f, 1.0f), glm::vec3(0.0f), glm::vec3(1.0f), true);
    Model& windowcube = game->addModel("models/windowcube/windowcube.obj", glm::vec3(-3.0f, 1.001f, -3.0f), glm::vec3(0.0f), glm::vec3(1.0f), true);



    game->start();

    return 0;

}