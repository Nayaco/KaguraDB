#include "AppContainer/AppContainer.hpp"
namespace App {
void RUN() {
    CS::init();
    CM::init();
    RM::init();
    REHL::rehl().run();
    RM::exit();
    CM::exit();
    CS::exit();
}
}