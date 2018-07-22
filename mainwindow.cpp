#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <QTextStream>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    server_process = new QProcess(this);
    server_process->start("RHVoice-service");

    client_process = new QProcess(this);
    connect(client_process, SIGNAL(readyReadStandardOutput()), SLOT(slotReadReady()));
    connect(client_process, SIGNAL(started()), SLOT(slotReadyToWrite()));

    QAudioFormat format;
    format.setSampleRate(24000);
    format.setChannelCount(1);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::SignedInt);

    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    if (!info.isFormatSupported(format)) {
        qWarning("Raw audio format not supported by backend, cannot play audio.");
        return;
    }

    audio = new QAudioOutput(format, this);
    audio->setNotifyInterval(1);
    connect(audio, SIGNAL(stateChanged(QAudio::State)), SLOT(handleStateChanged(QAudio::State)));
    connect(audio, SIGNAL(notify()), SLOT(slotRestoreVolume()));
}

MainWindow::~MainWindow()
{
    server_process->kill();
    client_process->kill();
    audio->stop();

    delete ui;
}

void MainWindow::slotReadReady()
{
    audio->start(client_process);
    audio->setVolume(0.0);
}

void MainWindow::slotReadyToWrite()
{
    QString str = text.first();
    text.pop_front();
    client_process->write(str.toUtf8().data());
    client_process->closeWriteChannel();
}

void MainWindow::slotRestoreVolume()
{
    audio->setVolume(1.0);
}

void MainWindow::handleStateChanged(QAudio::State newState)
{
    switch (newState) {
    case QAudio::IdleState:
        audio->stop();
        break;

    case QAudio::StoppedState:
        if (audio->error() != QAudio::NoError) {
             qWarning("Cannot play audio.");
        } 
        ui->cmd_next->setEnabled(true);
        break;

    default:
        break;
    }
}

void MainWindow::on_cmd_next_clicked()
{
    if (text.empty())
        openFile();

    if (text.empty())
        return;

    client_process->start(
        "RHVoice-client",
        QStringList({"-s", "Elena"}),
        QIODevice::ReadWrite | QIODevice::Unbuffered
        );

    ui->cmd_next->setEnabled(false);
}

void MainWindow::on_cmd_select_file_clicked()
{
    QString str_file = QFileDialog::getOpenFileName(ui->cmd_select_file);
    if (str_file.count() == 0)
        return;

    ui->txt_file->setText(str_file);
}

void MainWindow::openFile()
{
    QString file_path = ui->txt_file->text();
    if (file_path.count() == 0)
        return;

    QFile file;
    file.setFileName(file_path);
    file.open(QIODevice::ReadOnly);
    text.clear();

    if (!file.exists())
        return;

    QTextStream text_stream(&file);
    while (!text_stream.atEnd())
        text.append(text_stream.readLine());

    if (text_stream.status() == QTextStream::Ok)
    {
        ui->lbl_openned_file->setText(file_path);
        ui->cmd_next->setEnabled(true);
    }
    else
    {
        ui->cmd_next->setEnabled(false);
    }

    file.close();
}

void MainWindow::on_cmd_reset_clicked()
{
    openFile();
}
