#include "AppContainer/AppContainer.hpp"
namespace App {
void RUN() {
    CS::init();
    CM::init();
    RM::init();
    IM::init();
    REHL::rehl().run();
    IM::exit();
    RM::exit();
    CM::exit();
    CS::exit();
}
}