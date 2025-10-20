#include "signal_handler.h"
#include <signal.h>
#include <memory>
#include "pcb.h"

// Global pointer to the PageControlBlock for the signal handler to access
static std::shared_ptr<PageControlBlock> g_pcb;

static void sigintHandler(int sig) {
    if (g_pcb) {
        g_pcb->stopApp();
    }
}

void setupSignalHandlers(std::shared_ptr<PageControlBlock> pcb) {
    g_pcb = pcb;
    signal(SIGINT, sigintHandler);
}