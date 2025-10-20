#pragma once
#include <memory>

class PageControlBlock;

void setupSignalHandlers(std::shared_ptr<PageControlBlock> pcb);