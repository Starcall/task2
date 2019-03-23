#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QCommonStyle>
#include <QDesktopWidget>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include "QThread"

main_window::main_window(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), qApp->desktop()->availableGeometry()));

    ui->treeWidget->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->treeWidget->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
  //  ui->treeWidget->
    ui->treeWidget->setUniformRowHeights(true);
    this->setMinimumSize(QSize(300, 50));
    this->setMaximumSize(QSize(300000, 500000));
    QCommonStyle style;
    ui->actionScan_Directory->setIcon(style.standardIcon(QCommonStyle::SP_DialogOpenButton));
    ui->actionExit->setIcon(style.standardIcon(QCommonStyle::SP_DialogCloseButton));
    ui->actionAbout->setIcon(style.standardIcon(QCommonStyle::SP_DialogHelpButton));

    connect(ui->actionScan_Directory, &QAction::triggered, this, &main_window::select_directory);
    connect(ui->actionExit, &QAction::triggered, this, &QWidget::close);
    connect(ui->actionAbout, &QAction::triggered, this, &main_window::show_about_dialog);
    connect(ui->findButton, &QPushButton::released, this, &main_window::find);
    connect(ui->stopButton, &QPushButton::released, this, &main_window::stop);
    qRegisterMetaType<trigram_file>("trigram_file");
    qRegisterMetaType<qint64>("qint64");
    ui->findButton->setHidden(true);
    ui->patternLabel->setHidden(true);
    ui->patternEdit->setHidden(true);
    ui->stopButton->setHidden(true);
    ui->progressBar->setHidden(true);
   //std::cerr << "wtf";
    thread = new QThread();
    thread->start();
    thread->quit();
    thread->wait();
}

main_window::~main_window()
{
    //std::cerr << "COLLAPSE!" << std::endl;
    stop();
    thread->quit();
    thread->wait();
    delete thread;
}

void main_window::select_directory()
{

    QString dir = QFileDialog::getExistingDirectory(this, "Select Directory for Scanning",
                                                    QString(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    scan_directory(dir);
}

void main_window::searching_finish() {
    //std::cerr << "kek";
    ui->findButton->setHidden(false);
    ui->patternLabel->setHidden(false);
    ui->patternEdit->setHidden(false);
    ui->progressBar->setHidden(true);
    ui->stopButton->setHidden(true);
   // std::cerr << "and what\n" << std::endl;
    //thread->quit();
    //thread->wait();
}

void main_window::scan_directory(QString const& dir)
{

    //std::cerr << "Point 0 passed\n";
    if (!(thread->isFinished())) {

        counter->blockSignals(true);
        //disconnect(this, 0, counter, 0);
        //counter->deleteLater();
        thread->quit();
        thread->wait();
        delete thread;
    }
    thread = new QThread();
    ui->progressBar->setHidden(false);
    ui->stopButton->setHidden(false);
    ui->treeWidget->clear();
    counter = new worker(dir);
    //thread->start();
//    std::cerr << counter->directory.toString().toStdString() << std::endl;
   // std::cerr << "Point 1 passed\n";
    counter->moveToThread(thread);

    std::cerr << connect(thread, &QThread::finished, counter, &QObject::deleteLater)<< std::endl;
    std::cerr << connect(this, &main_window::index_files, counter, &worker::take_control) << std::endl;
    std::cerr <<connect(counter, &worker::indexing_finished, this, &main_window::indexing_finish)<< std::endl;
    std::cerr <<connect(counter, &worker::send_status, this, &main_window::show_progress)<< std::endl;
    //std::cerr << "Point 2 passed\n";
    thread->start();
    emit index_files();


}

void main_window::show_progress(qint64 val) {
    //std::cerr<< "there" <<std::endl;
    ui->progressBar->setValue(val);
}
void main_window::indexing_finish() {
   // std::cerr << "Point 4 passed\n";
    ui->progressBar->setHidden(true);
    ui->stopButton->setHidden(true);
    ui->findButton->setHidden(false);
    ui->patternLabel->setHidden(false);
    ui->patternEdit->setHidden(false);

    connect(this, &main_window::init, counter, &worker::find_substring);
    connect(counter, &worker::finish, this, &main_window::searching_finish);
    connect(counter, &worker::send_position, this, &main_window::show_index);

   // std::cerr << "Point 5 passed\n";
}

void main_window::find() {
    //std::cerr << "something wrong";
    QString pattern = ui->patternEdit->text();
    ui->treeWidget->clear();
    ui->findButton->setHidden(true);
    ui->patternLabel->setHidden(true);
    ui->patternEdit->setHidden(true);
    ui->progressBar->setHidden(false);
  //  ui->stopButton->setHidden(f);

    emit init(pattern);

}
void main_window::show_about_dialog()
{
    QMessageBox::aboutQt(this);
}



void main_window::show_index(QString const& file_path, qint64 pos) {
    QTreeWidgetItem* item = new QTreeWidgetItem(ui->treeWidget);
    item->setText(0, file_path);
    item->setText(1, QString::number(pos));
    ui->treeWidget->addTopLevelItem(item);
}

void main_window::stop() {
    //std::cerr << "kek";
    thread->requestInterruption();
}

