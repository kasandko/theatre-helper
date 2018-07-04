#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    process = new QProcess(this);
    connect(process, SIGNAL(readyReadStandardOutput()), SLOT(slotReadReady()));
    connect(process, SIGNAL(started()), SLOT(slotReadyToWrite()));

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
    delete ui;
}

void MainWindow::on_cmd_speak_clicked()
{
    process->start(
        "RHVoice-client",
        QStringList({"-s", "Elena"}),
        QIODevice::ReadWrite | QIODevice::Unbuffered
        );
}

void MainWindow::slotReadReady()
{
    audio->start(process);
    audio->setVolume(0.0);
}

void MainWindow::slotReadyToWrite()
{
    process->write(ui->txt_text->toPlainText().toUtf8().data());
    process->closeWriteChannel();
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
        break;

    default:
        break;
    }
}
