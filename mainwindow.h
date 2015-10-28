#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileInfo>
#include <QMediaPlayer>
#include <QTextBrowser>
#include <QTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void onBrowseClicked();
    void onPlayClicked();
    void onStopClicked();
    void onVolumeSliderChanged(int volume);
    void onTimerTick();

protected:
    void keyPressEvent(QKeyEvent * event) override;

private:
    void setDirectory(QString musicDirectory);
    void launchTimer();
    void nextMusic();
    void updateScores();

    Ui::MainWindow *ui;
    QVector<QFileInfo> musicsList;
    QVector<bool> playedMusicsList;
    QMediaPlayer *mediaPlayer;
    QVector<QTextBrowser*> answerWidgetsList;
    QTimer *timer;
    QString rightAnswerName;
    QString announcementText;
    int rightAnswerPosition;
    int scoreP1, scoreP2;
    int playedMusicsCount;
    int remainingTime;
    bool currentlyPlaying;
};

#endif // MAINWINDOW_H
