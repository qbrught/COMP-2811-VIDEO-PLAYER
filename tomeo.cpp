//    ______
//   /_  __/___  ____ ___  ___  ____
//    / / / __ \/ __ `__ \/ _ \/ __ \
//   / / / /_/ / / / / / /  __/ /_/ /
//  /_/  \____/_/ /_/ /_/\___/\____/
//              video for sports enthusiasts...
//
//

#include <iostream>
#include <QApplication>
#include <QtMultimediaWidgets/QVideoWidget>
#include <QMediaPlaylist>
#include <string>
#include <vector>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QHBoxLayout>
#include <QtCore/QFileInfo>
#include <QtWidgets/QFileIconProvider>
#include <QDesktopServices>
#include <QImageReader>
#include <QMessageBox>
#include <QtCore/QDir>
#include <QtCore/QDirIterator>
#include "the_player.h"
#include "the_button.h"
#include "the_bar.h"

#include <QtWidgets>

// read in videos and thumbnails to this directory
std::vector<TheButtonInfo> getInfoIn (std::string loc) {

    std::vector<TheButtonInfo> out =  std::vector<TheButtonInfo>();
    QDir dir(QString::fromStdString(loc) );
    QDirIterator it(dir);

    while (it.hasNext()) { // for all files

        QString f = it.next();

            if (f.contains("."))

#if defined(_WIN32)
            if (f.contains(".wmv"))  { // windows
#else
            if (f.contains(".mp4") || f.contains("MOV"))  { // mac/linux
#endif

            QString thumb = f.left( f .length() - 4) +".png";
            if (QFile(thumb).exists()) { // if a png thumbnail exists
                QImageReader *imageReader = new QImageReader(thumb);
                    QImage sprite = imageReader->read(); // read the thumbnail
                    if (!sprite.isNull()) {
                        QIcon* ico = new QIcon(QPixmap::fromImage(sprite)); // voodoo to create an icon for the button
                        QUrl* url = new QUrl(QUrl::fromLocalFile( f )); // convert the file location to a generic url
                        out . push_back(TheButtonInfo( url , ico  ) ); // add to the output list
                    }
                    else
                        qDebug() << "warning: skipping video because I couldn't process thumbnail " << thumb << endl;
            }
            else
                qDebug() << "warning: skipping video because I couldn't find thumbnail " << thumb << endl;
        }
    }

    return out;
}

int main(int argc, char *argv[]) {

    // let's just check that Qt is operational first
    qDebug() << "Qt version: " << QT_VERSION_STR << endl;
    // create the Qt Application
    QApplication app(argc, argv);

    // collect all the videos in the folder
    std::vector<TheButtonInfo> videos;

    if (argc == 2)
        videos = getInfoIn( std::string(argv[1]) );

    if (videos.size() == 0) {

        const int result = QMessageBox::information(
                    NULL,
                    QString("Tomeo"),
                    QString("no videos found! Add command line argument to \"quoted\" file location."));
        exit(-1);
    }

    //The main window

    QPalette p = QPalette();


    QWidget window;
    window.setStyleSheet("background-color: #f5e2df;");
    QHBoxLayout *topH = new QHBoxLayout();
    QVBoxLayout *topV = new QVBoxLayout();
    window.setMinimumSize(960, 640);
    if (window.height() <= window.width()) {
        window.setLayout(topH);
    }
    else {
        window.setLayout(topV);
    }
    window.setWindowTitle("tomeo");

    // the widget that will show the video
    QVideoWidget *videoWidget = new QVideoWidget;

    // the QMediaPlayer which controls the playback
    ThePlayer *player = new ThePlayer;
    TheBar* bar = new TheBar();
    bar->setMaximumHeight(20);
    bar->setTextVisible(false);
    player->setVideoOutput(videoWidget);

    QPushButton* pause = new QPushButton();
    pause->connect(pause, SIGNAL(clicked()), player, SLOT (pause()));
    pause->setIcon(QIcon(":/icons/pause.png"));
    pause->setIconSize(QSize(20,20));
    QPushButton* play = new QPushButton();
    play->connect(play, SIGNAL(clicked()), player, SLOT (play()));
    play->setIcon(QIcon(":/icons/play.png"));
    play->setIconSize(QSize(20,20));
    QPushButton* restart = new QPushButton();
    restart->connect(restart, SIGNAL(clicked()), player, SLOT (stop()));
    restart->connect(restart, SIGNAL(clicked()), bar, SLOT (restart()));
    restart->setIcon(QIcon(":/icons/stop.png"));
    restart->setIconSize(QSize(20,20));

    QComboBox* settings = new QComboBox();
    QStringList text;
    text << "settings" << "resolution" << "playback speed" << "subtiles";
    settings->addItems(text);

    pause->connect(pause, SIGNAL(clicked()), bar, SLOT (pause()));
    play->connect(play, SIGNAL(clicked()), bar, SLOT (resume()));



    QTextEdit* commentSection = new QTextEdit();
    commentSection->setPlaceholderText("enter a comment ...");



    QLabel* volumeLabel = new QLabel("volume");
    QSlider *volume = new QSlider(Qt::Horizontal);
    volume->setRange(0,100);
    QHBoxLayout* volumeHolder = new QHBoxLayout();
    volumeHolder->addWidget(volumeLabel);
    volumeHolder->addWidget(volume);
    QWidget* volumeWidget = new QWidget();
    volumeWidget->setLayout(volumeHolder);


    QHBoxLayout* buttonHolder = new QHBoxLayout();
    buttonHolder->addWidget(pause);
    buttonHolder->addWidget(play);
    buttonHolder->addWidget(restart);
    buttonHolder->addWidget(volumeWidget);
    buttonHolder->addWidget(settings);



    QWidget* buttonHolderWidget = new QWidget();
    buttonHolderWidget->setLayout(buttonHolder);

    // a row of buttons
    QWidget *buttonWidget = new QWidget();
    // a list of the buttons
    std::vector<TheButton*> buttons;
    // the buttons are arranged horizontally
    //changed to be vertically
    QVBoxLayout *layout = new QVBoxLayout();
    buttonWidget->setLayout(layout);

    QScrollArea *thumbArea = new QScrollArea();
    QWidget *buttonsToScrollArea = new QWidget();
    QGridLayout *buttonLayoutGrid = new QGridLayout();
    QVBoxLayout *buttonLayoutVert = new QVBoxLayout();
    if (window.height() >= window.width()) {
        buttonsToScrollArea->setLayout(buttonLayoutGrid);
    }
    else {
        buttonsToScrollArea->setLayout(buttonLayoutVert);
    }

    int count = 0;
    // create the four buttons
    for ( int i = 0; i < 16; i++ ) {
        TheButton *button = new TheButton(buttonWidget);
        button->connect(button, SIGNAL(jumpTo(TheButtonInfo* )), player, SLOT (jumpTo(TheButtonInfo*)));
        button->connect(button, SIGNAL(jumpTo(TheButtonInfo* )), bar, SLOT (restart()));// when clicked, tell the player to play.
        buttons.push_back(button);
        if (window.height() >= window.width()) {
            buttonLayoutGrid->addWidget(button,int(i/3),i % 3,1,1);
        }
        else {
            buttonLayoutVert->addWidget(button);
        }
        //layout->addWidget(button);
        if(i%4 == 0)
            count = 0;
        button->init(&videos.at(count));
        count++;
    }

    thumbArea->setWidget(buttonsToScrollArea);
    thumbArea->horizontalScrollBar()->setEnabled(false);


    QVBoxLayout* vidAndButt = new QVBoxLayout();
    vidAndButt->addWidget(videoWidget);
    vidAndButt->addWidget(bar);
    vidAndButt->addWidget(buttonHolderWidget);
    vidAndButt->addWidget(commentSection);
    if (window.height() >= window.width()) {
        vidAndButt->addWidget(thumbArea);
    }

    QWidget* vidButt = new QWidget();
    vidButt->setLayout(vidAndButt);
    // tell the player what buttons and videos are available
    player->setContent(&buttons, & videos);
    // create the main window and layout

    QVBoxLayout *loginAndThumbs = new QVBoxLayout();

    QPushButton* login = new QPushButton("login");
       loginAndThumbs->addWidget(login);

    QPushButton* uploadButton = new QPushButton("upload");
    loginAndThumbs->addWidget(uploadButton);

    loginAndThumbs->addWidget(thumbArea);



     QWidget *loginAndThumbsWidget = new QWidget();
     loginAndThumbsWidget->setLayout(loginAndThumbs);








    //set the width of the thumbnails to be a suitable width for the viewing experience
    if (window.height() <= window.width()) {
        buttonWidget->setMaximumWidth(window.height() * 0.5);
        buttonHolderWidget->setMaximumHeight(0.1 * window.height());
        thumbArea->setMinimumWidth(0.35*window.height());
        commentSection->setMaximumHeight(window.height() * 0.2);
    }
    else {
        videoWidget->setMinimumHeight(window.height() * .4);
        videoWidget->setMaximumHeight(window.height() * .4);
        buttonWidget->setMaximumWidth(window.width());
        buttonWidget->setMinimumWidth(window.width());
        buttonHolderWidget->setMaximumHeight(0.05 * window.height());
        commentSection->setMaximumHeight(window.height() * 0.03);
        commentSection->setMinimumHeight(window.height() * 0.03);
    }

    // add the video and the buttons to the top level widget
    if (window.height() <= window.width()) {
            topH->addWidget(vidButt);
            topH->addWidget(buttonWidget);
            //topH->addWidget(thumbArea);
            topH->addWidget(loginAndThumbsWidget);
            window.show();
        }
    else {
            topV->addWidget(vidButt);
            topV->addWidget(buttonWidget);
            topV->addWidget(thumbArea);
            window.show();
        }
    // showtime

    // wait for the app to terminate
    return app.exec();
}
