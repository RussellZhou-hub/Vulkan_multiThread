#include"config.h"
#include"view/engine.h"

int main(){

    std::cout<<"hello, vulkan\n";

    Engine* engine = new Engine(render_width,render_height);

    engine->run();

    delete engine;

    return 0;
}