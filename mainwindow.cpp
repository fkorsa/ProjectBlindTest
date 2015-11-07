#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QDirIterator>
#include <QDebug>
#include <QKeyEvent>
#include <QTime>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    remainingTime(0),
    currentlyPlaying(false)
{
    ui->setupUi(this);

    ui->playerLabel1->setText("Player 1");
    ui->playerLabel2->setText("Player 2");
    ui->announcementDisplay->setFontPointSize(15);

    QObject::connect(ui->browseButton, SIGNAL(clicked()), this, SLOT(onBrowseClicked()));
    QObject::connect(ui->playButton, SIGNAL(clicked()), this, SLOT(onPlayClicked()));
    QObject::connect(ui->stopButton, SIGNAL(clicked()), this, SLOT(onStopClicked()));
    QObject::connect(ui->volumeSlider, SIGNAL(sliderMoved(int)), this, SLOT(onVolumeSliderChanged(int)));

    mediaPlayer = new QMediaPlayer;
    mediaPlayer->setVolume(50);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimerTick()));

    answerWidgetsList.append(ui->answerWidget1);
    answerWidgetsList.append(ui->answerWidget2);
    answerWidgetsList.append(ui->answerWidget3);
    answerWidgetsList.append(ui->answerWidget4);

    QFile file("settings.pbt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&file);
        QString line = in.readLine();
        setDirectory(line);
        file.close();
    }

    qsrand(QTime::currentTime().msec());

    grabKeyboard();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete timer;

    delete mediaPlayer;
}

void MainWindow::onBrowseClicked()
{
    QString directory = QFileDialog::getExistingDirectory();
    setDirectory(directory);
    QFile file("settings.pbt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&file);
        out << directory;
        file.close();
    }
}

void MainWindow::onPlayClicked()
{
    if(musicsList.size() > 0)
    {
        scoreP1 = 0;
        scoreP2 = 0;
        playedMusicsCount = 0;
        playedMusicsList.fill(false, musicsList.size());
        currentlyPlaying = true;

        nextMusic();
    }
}

void MainWindow::onStopClicked()
{
    mediaPlayer->stop();
    ui->stopButton->setEnabled(false);
    ui->playButton->setEnabled(true);
    currentlyPlaying = false;
}

void MainWindow::onVolumeSliderChanged(int volume)
{
    mediaPlayer->setVolume(volume);
}

void MainWindow::onTimerTick()
{
    remainingTime--;
    if(remainingTime == 1)
    {
        ui->announcementDisplay->setText(announcementText + "1 second before the next music...");
    }
    else
    {
        ui->announcementDisplay->setText(announcementText + QString::number(remainingTime) + " seconds before the next music...");
    }

    if(remainingTime <= 0)
    {
        ui->announcementDisplay->setText("");
        timer->stop();
        nextMusic();
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    int player;
    int playerAnswer;
    if(remainingTime > 0 || !currentlyPlaying)
    {
        return;
    }
    switch(event->key())
    {
    case Qt::Key_1:
        playerAnswer = 0;
        player = 2;
        break;
    case Qt::Key_2:
        playerAnswer = 1;
        player = 2;
        break;
    case Qt::Key_3:
        playerAnswer = 2;
        player = 2;
        break;
    case Qt::Key_4:
        playerAnswer = 3;
        player = 2;
        break;
    case Qt::Key_F1:
        playerAnswer = 0;
        player = 1;
        break;
    case Qt::Key_F2:
        playerAnswer = 1;
        player = 1;
        break;
    case Qt::Key_F3:
        playerAnswer = 2;
        player = 1;
        break;
    case Qt::Key_F4:
        playerAnswer = 3;
        player = 1;
        break;
    default:
        break;
    }
    if(playerAnswer > musicsList.size() - 1)
    {
        return;
    }
    if(playerAnswer == rightAnswerPosition)
    {
        if(player == 1)
        {
            scoreP1++;
        }
        else
        {
            scoreP2++;
        }
        updateScores();
        announcementText = "Player " + QString::number(player) + " won!\n"
                + "The answer was: " + rightAnswerName + "\n"
                + "Please wait ";
        ui->announcementDisplay->setText(announcementText + "3 seconds before the next music...");
        launchTimer();
    }
    else
    {
        if(player == 1)
        {
            scoreP1--;
        }
        else
        {
            scoreP2--;
        }
        updateScores();
    }
}

void MainWindow::setDirectory(QString musicDirectory)
{
    ui->playButton->setEnabled(true);
    ui->stopButton->setEnabled(false);
    musicsList.clear();

    QDirIterator it(musicDirectory, QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        QFileInfo musicPath(it.next());
        if(musicPath.isFile() && musicPath.suffix().compare("mp3") == 0)
        {
            musicsList.append(musicPath);
        }
    }
    ui->musicDirectoryWidget->setText(QFileInfo(musicDirectory).baseName());
}

void MainWindow::launchTimer()
{
    remainingTime = 3;
    timer->start(1000);
}

void MainWindow::nextMusic()
{
    int musicIndex = qrand() % musicsList.size();
    while(playedMusicsList[musicIndex])
    {
        musicIndex++;
        if(musicIndex >= playedMusicsList.size())
        {
            musicIndex = 0;
        }
    }
    playedMusicsList[musicIndex] = true;
    playedMusicsCount++;
    if(playedMusicsCount >= musicsList.size())
    {
        playedMusicsList.fill(false, musicsList.size());
        playedMusicsCount = 0;
    }
    mediaPlayer->setMedia(QUrl::fromLocalFile(musicsList[musicIndex].absoluteFilePath()));

    mediaPlayer->play();
    ui->stopButton->setEnabled(true);
    ui->playButton->setEnabled(false);

    QVector<bool> musicsSelected(musicsList.size(), false);
    int randomAnswersCount = 0;

    rightAnswerPosition = qrand() % 4;
    rightAnswerName = musicsList[musicIndex].completeBaseName();
    answerWidgetsList[rightAnswerPosition]->setText(musicsList[musicIndex].completeBaseName());
    musicsSelected[musicIndex] = true;

    for (int i = 0; i < 4; ++i)
    {
        if(i != rightAnswerPosition && randomAnswersCount + 1 < musicsList.size())
        {
            musicIndex = qrand() % musicsList.size();
            while(musicsSelected[musicIndex])
            {
                musicIndex++;
                if(musicIndex >= musicsSelected.size())
                {
                    musicIndex = 0;
                }
            }
            randomAnswersCount++;
            musicsSelected[musicIndex] = true;
            answerWidgetsList[i]->setText(musicsList[musicIndex].completeBaseName());
        }
    }
}

void MainWindow::updateScores()
{
    ui->playerScore1->display(scoreP1);
    ui->playerScore2->display(scoreP2);
}
