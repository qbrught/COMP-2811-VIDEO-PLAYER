#ifndef THE_BAR_H
#define THE_BAR_H


#include <QProgressBar>
#include <QTimer>

class TheBar: public QProgressBar
{
    Q_OBJECT
public:
    QTimer* mTimer;

    TheBar(): QProgressBar() {
        mTimer = new QTimer(NULL);
        mTimer->setInterval(1000);
        mTimer->start();
        connect(mTimer, SIGNAL(timeout()), SLOT(update()));
    }
    int seconds;
private slots:
    void update();
    void pause();
    void resume();
};

#endif // THE_BAR_H
