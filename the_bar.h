//
// Created by twak on 11/11/2019.
//

#ifndef CW2_THE_BAR_H
#define CW2_THE_BAR_H

#include <QPushButton>
#include <QProgressBar>
#include <QTimer>

class TheBar : public QProgressBar {
    Q_OBJECT

public:
    QTimer* mTimer;

    TheBar(): QProgressBar() {
        mTimer = new QTimer(NULL);
        mTimer->setInterval(100); // 1000ms is one second between ...
        mTimer->start();
        connect( mTimer, SIGNAL (timeout()), SLOT ( update() ) ); // ...running shuffle method
    }
    int seconds = 0;

private slots:
    void update();
    void pause();
    void resume();
    void restart();
};

#endif
