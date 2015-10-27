#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileInfo>
#include <QMediaPlayer>
#include <QTextBrowser>

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

protected:
    void keyPressEvent(QKeyEvent * event) override;

private:
    void nextMusic();
    void updateScores();

    Ui::MainWindow *ui;
    QVector<QFileInfo> musicsList;
    QMediaPlayer *mediaPlayer;
    QVector<QTextBrowser*> answerWidgetsList;
    int rightAnswerPosition;
    int scoreP1, scoreP2;
};

#endif // MAINWINDOW_H
