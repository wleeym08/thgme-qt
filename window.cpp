#include "window.h"
#include "ui_window.h"
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>
#include "extractorWorker.h"

Window::Window(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Window) {
    ui->setupUi(this);
    this->setFixedSize(390, 250);
}

Window::~Window() {
    delete ui;
}

void Window::start_extract() {
    this->fmt_path = ui->lineEdit_fmt->text();
    this->dat_path = ui->lineEdit_dat->text();
    this->out_path = ui->lineEdit_out->text();

    int length = this->out_path.length();

    if (length != 0 && this->out_path[length - 1] == '/') {
        this->out_path = (this->out_path).left(length - 1);
    }

    if (this->fmt_path != "" && this->dat_path != "" && this->out_path != "") {
        ui->pushButton_fmt->setEnabled(false);
        ui->pushButton_dat->setEnabled(false);
        ui->pushButton_out->setEnabled(false);
        ui->pushButton_start->setEnabled(false);
        ui->lineEdit_fmt->setEnabled(false);
        ui->lineEdit_dat->setEnabled(false);
        ui->lineEdit_out->setEnabled(false);

        this->thread = new QThread;
        this->worker = new ExtractorWorker(this->fmt_path, this->dat_path, this->out_path);
        worker->moveToThread(thread);

        connect(worker, SIGNAL(message(QString)), this, SLOT(on_message(QString)));
        connect(worker, SIGNAL(error(QString)), this, SLOT(on_error(QString)));
        connect(worker, SIGNAL(finished()), this, SLOT(on_finished()));
        connect(thread, SIGNAL(started()), worker, SLOT(process()));
        connect(worker, SIGNAL(finished()), thread, SLOT(quit()));
        connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
        connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

        thread->start();

    } else {
        QMessageBox::warning(this, "Warning", "Please specify valid paths for data files and output directory!");
    }
}

void Window::on_pushButton_fmt_clicked() {
    ui->lineEdit_fmt->setText(QFileDialog::getOpenFileName(this, "Open File", QDir::currentPath(), "FMT files (*.fmt)"));
}

void Window::on_pushButton_dat_clicked() {
    ui->lineEdit_dat->setText(QFileDialog::getOpenFileName(this, "Open File", QDir::currentPath(), "DAT files (*.dat)"));
}

void Window::on_pushButton_out_clicked() {
    ui->lineEdit_out->setText(QFileDialog::getExistingDirectory(
                                  this, "Open Directory", QDir::currentPath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks));
}

void Window::on_pushButton_start_clicked() {
    this->start_extract();
}

void Window::on_message(QString msg) {
    ui->statusBar->showMessage(msg, 2000);
}

void Window::on_error(QString err) {
    QMessageBox::warning(this, "Warning", err);
}

void Window::on_finished() {
    ui->statusBar->showMessage("All done.", 5000);
    ui->pushButton_fmt->setEnabled(true);
    ui->pushButton_dat->setEnabled(true);
    ui->pushButton_out->setEnabled(true);
    ui->pushButton_start->setEnabled(true);
    ui->lineEdit_fmt->setEnabled(true);
    ui->lineEdit_dat->setEnabled(true);
    ui->lineEdit_out->setEnabled(true);
}

void Window::on_actionAbout_ThGME_triggered() {
    QString about_str = QString("Touhou General Music Extractor (ThGME) v0.20\n") +
                        "Running against Qt 5,\n" +
                        "designed and published with LOVE for Touhou.";
    QMessageBox::information(this, "About ThGME", about_str);
}

void Window::on_actionStart_extracting_files_triggered() {
    this->start_extract();
}

void Window::on_actionExit_program_triggered() {
    QApplication::quit();
}
