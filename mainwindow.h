#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>
#include <QThread>
#include "worker.h"

namespace Ui {
class MainWindow;
}

class main_window : public QMainWindow
{
    Q_OBJECT
    QThread* thread;

public:
    explicit main_window(QWidget *parent = 0);
    ~main_window();

private slots:
    void select_directory();
    void scan_directory(QString const& dir);
    void show_about_dialog();
    void find();
    void indexing_finish();
    void stop();
    void searching_finish();
    void show_index(QString const& file_path, qint64 pos);
    void show_progress(qint64);
signals:
    void index_files();
    void init(QString const& pattern);
    void initiate(QString const& dir);
private:
    std::unique_ptr<Ui::MainWindow> ui;
    bool is_first_indexing = 1;
    bool is_first_search = 1;
    worker* counter;
};

#endif // MAINWINDOW_H
