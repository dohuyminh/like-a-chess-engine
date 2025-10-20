#pragma once

#include "../resources/resources.h"
#include "../resources/pcb.h"

class App {
public:

    App(Resources&& resources, std::shared_ptr<PageControlBlock> pcb);

    void render();  

private:

    // collection of pages         
    Resources _resources;

    std::shared_ptr<PageControlBlock> _pcb;

};
