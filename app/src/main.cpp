/**
 * @file main.cpp
 * @brief Application entry point.
 */

#include <qualquer/app/application.h>

int main() {
    qualquer::app::Application app;
    app.init();
    app.run();
    app.destroy();
    return 0;
}
