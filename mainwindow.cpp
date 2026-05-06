#include "mainwindow.h"
#include "worker.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QTimer>
#include <QFile>
#include <QThread>
#include <QCoreApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{   ui->setupUi(this);
    ui->radioButton_count->setChecked(true);
    ui->radioButton_once->setChecked(true);
    ui->progressBar->setValue(0);
    ui->label_state->setText("Выберите параметры и укажите пути");
    timer = new QTimer(this);
    isProcessing = false;

    connect(timer, &QTimer::timeout, this, &MainWindow::on_pushButton_using_XOR_clicked);
    connect(this, &MainWindow::progressUpdated, ui->progressBar, &QProgressBar::setValue);
    connect(this, &MainWindow::statusMessage, ui->label_state, &QLabel::setText);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::ProcessingFile(){
    QString inputKey = ui->lineEdit_XOR_key->text();
    QString uiOutputPath= ui->lineEdit_output_path->text();
    QString mask = ui->lineEdit_mask->text();
    bool remove = ui->checkBox_delete_input_File->isChecked();
    bool counter = ui->radioButton_count->isChecked();
    if (inputKey.length() != 16) {
        ui->label_state->setText("Ключ должен быть ровно 16 hex-символов (8 байт)");
        return;
    }


    QThread* thread = new QThread(this);
    Worker* worker = new Worker();
    worker->setData(inputKey, uiOutputPath, mask, remove, counter);
    worker->moveToThread(thread);

    connect(worker, &Worker::finished, thread, &QThread::quit);
    connect(worker, &Worker::finished, worker, &QObject::deleteLater);
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);
    connect(thread, &QThread::started, worker, &Worker::process);

    connect(worker, &Worker::progressUpdated, this, &MainWindow::progressUpdated);
    connect(worker, &Worker::statusMessage, this, &MainWindow::statusMessage);

    thread->start();

    connect(worker, &Worker::processingFinished, this, [this]() {
        isProcessing = false;
    });
}

void MainWindow::on_pushButton_using_XOR_clicked()
{
    if (isProcessing) {
        return;
    }
    isProcessing = true;
    if (ui->radioButton_timer->isChecked()) {
        int seconds = ui->spinBox_period->value();
        timer->start(seconds * 1000);
        ProcessingFile();
    } else {
        timer->stop();
        ProcessingFile();
    }
}


