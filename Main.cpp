#include "dyno.h"

int main() {
    dyno::server server;

    server.run();

    while (true);
}