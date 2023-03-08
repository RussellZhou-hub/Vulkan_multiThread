#include"config.h"
#include"view/engine.h"

int main(){

    std::cout<<"hello, vulkan\n";

    Engine* engine = new Engine(640,480);

    engine->run();

    delete engine;

    return 0;
}