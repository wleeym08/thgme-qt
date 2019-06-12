#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QThread>
#include "extractorWorker.h"

namespace Ui {
class Window;
}

class Window : public QMainWindow
{
    Q_OBJECT

public:
    explicit Window(QWidget *parent = nullptr);
    ~Window();
    void start_extract();

private slots:
    void on_pushButton_fmt_clicked();
    void on_pushButton_dat_clicked();
    void on_pushButton_out_clicked();
    void on_pushButton_start_clicked();
    void on_message(QString);
    void on_error(QString);
    void on_finished();
    void on_actionAbout_ThGME_triggered();

    void on_actionStart_extracting_files_triggered();

    void on_actionExit_program_triggered();

private:
    Ui::Window *ui;
    QString fmt_path;
    QString dat_path;
    QString out_path;
    QThread* thread;
    ExtractorWorker* worker;
};

#endif // WINDOW_H
