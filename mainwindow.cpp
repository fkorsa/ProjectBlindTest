#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QDirIterator>
#include <QDebug>
#include <QKeyEvent>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QObject::connect(ui->browseButton, SIGNAL(clicked()), this, SLOT(onBrowseClicked()));
    QObject::connect(ui->playButton, SIGNAL(clicked()), this, SLOT(onPlayClicked()));
    QObject::connect(ui->stopButton, SIGNAL(clicked()), this, SLOT(onStopClicked()));
    QObject::connect(ui->volumeSlider, SIGNAL(sliderMoved(int)), this, SLOT(onVolumeSliderChanged(int)));

    mediaPlayer = new QMediaPlayer;
    mediaPlayer->setVolume(50);

    answerWidgetsList.append(ui->answerWidget1);
    answerWidgetsList.append(ui->answerWidget2);
    answerWidgetsList.append(ui->answerWidget3);
    answerWidgetsList.append(ui->answerWidget4);

    grabKeyboard();
}

MainWindow::~MainWindow()
{
    delete ui;

    delete mediaPlayer;
}

void MainWindow::onBrowseClicked()
{
    QString musicDirectory;
    ui->playButton->setEnabled(true);
    ui->stopButton->setEnabled(false);
    musicsList.clear();
    musicDirectory = QFileDialog::getExistingDirectory();
    QDirIterator it(musicDirectory, QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        QFileInfo musicPath(it.next());
        if(musicPath.isFile() && musicPath.completeSuffix().compare("mp3") == 0)
        {
            musicsList.append(musicPath);
        }
    }
    ui->musicDirectoryWidget->setText(QFileInfo(musicDirectory).baseName());
}

void MainWindow::onPlayClicked()
{
    if(musicsList.size() > 0)
    {
        scoreP1 = 0;
        scoreP2 = 0;

        nextMusic();
    }
}

void MainWindow::onStopClicked()
{
    mediaPlayer->stop();
    ui->stopButton->setEnabled(false);
    ui->playButton->setEnabled(true);
}

void MainWindow::onVolumeSliderChanged(int volume)
{
    mediaPlayer->setVolume(volume);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    int player;
    int playerAnswer;
    switch(event->key())
    {
    case Qt::Key_1:
        playerAnswer = 0;
        player = 1;
        break;
    case Qt::Key_2:
        playerAnswer = 1;
        player = 1;
        break;
    case Qt::Key_3:
        playerAnswer = 2;
        player = 1;
        break;
    case Qt::Key_4:
        playerAnswer = 3;
        player = 1;
        break;
    case Qt::Key_F1:
        playerAnswer = 0;
        player = 2;
        break;
    case Qt::Key_F2:
        playerAnswer = 1;
        player = 2;
        break;
    case Qt::Key_F3:
        playerAnswer = 2;
        player = 2;
        break;
    case Qt::Key_F4:
        playerAnswer = 3;
        player = 2;
        break;
    default:
        break;
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
        nextMusic();
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

void MainWindow::nextMusic()
{
    int musicIndex = qrand() % musicsList.size();
    mediaPlayer->setMedia(QUrl::fromLocalFile(musicsList[musicIndex].absoluteFilePath()));

    mediaPlayer->play();
    ui->stopButton->setEnabled(true);
    ui->playButton->setEnabled(false);

    QVector<bool> musicsSelected(musicsList.size(), false);
    int randomAnswersCount = 0;

    rightAnswerPosition = qrand() % 4;
    answerWidgetsList[rightAnswerPosition]->setText(musicsList[musicIndex].baseName());
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
            answerWidgetsList[i]->setText(musicsList[musicIndex].baseName());
        }
    }
}

void MainWindow::updateScores()
{
    ui->labelScore1->setText(QString::number(scoreP1));
    ui->labelScore2->setText(QString::number(scoreP2));
}
