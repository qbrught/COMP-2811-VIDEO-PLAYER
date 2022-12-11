#include "the_bar.h"

void TheBar::update() {
    QProgressBar::setValue(this->seconds);
    this->seconds++;
}

void TheBar::pause(){
    this->mTimer->stop();
}

void::TheBar::resume(){
    this->mTimer->start();
}
