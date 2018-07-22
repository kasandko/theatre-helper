#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAudioOutput>
#include <QProcess>
#include <QList>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void slotReadReady();
    void slotReadyToWrite();
    void slotRestoreVolume();
    void handleStateChanged(QAudio::State newState);

    void on_cmd_next_clicked();

    void on_cmd_select_file_clicked();

protected:
    void openFile();

private:
    Ui::MainWindow *ui;

    QProcess* client_process;
    QProcess* server_process;
    QAudioOutput* audio;

    QList<QString> text;
};

#endif // MAINWINDOW_H
