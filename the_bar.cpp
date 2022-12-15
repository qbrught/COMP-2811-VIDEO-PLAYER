#include "the_bar.h"

void TheBar::update() {
    QProgressBar::setValue(this->seconds/10);
    this->seconds++;
}

void::TheBar::pause() {
    this->mTimer->stop();
}

void::TheBar::resume() {
    this->mTimer->start();
}
void::TheBar::restart() {
    this->seconds = 0;
}
