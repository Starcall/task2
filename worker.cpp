#include "worker.h"
#include <qdiriterator.h>
#include "QTextStream"
#include "QThread"
worker::worker() {
    directory = QVariant(QString());
}

worker::worker(QString const& directory) {
    this->directory = QVariant(directory);
}

worker::worker(worker& c_worker) {
    this->watcher.addPaths(c_worker.watcher.directories());
    this->directory = c_worker.directory;
    this->indexed_directory = c_worker.indexed_directory;

}
worker::~worker() {
   // std::cerr << "hey, wait a minute.....\n";

}
void worker::initiate(QString const& directory) {
    this->directory = QVariant(directory);
    watcher.removePaths(watcher.directories());
    indexed_directory.clear();
}

void worker::take_control() {

    QDirIterator it(directory.toString(), QDir::Files, QDirIterator::Subdirectories);
    QVector<trigram_file> directory_list;
   // std::cerr << directory.toString().toStdString() << std::endl;
   //std::cerr << directory.toString().toStdString() << std::endl;
    emit send_status(0);
    while (it.hasNext()) {
       // std::cerr << directory.toString().toStdString() << std::endl;
        directory_list.push_back(trigram_file(it.next()));
    }
   // std::cerr << "Point 3 passed\n";
    qint64 cnt = 0;
   // std::cerr << "LMAO";
    emit send_status(1);
    for (auto& i : directory_list) {
        cnt++;
        emit send_status(cnt * 100 / directory_list.size());
        if (QThread::currentThread()->isInterruptionRequested()) {
          //  std::cerr << "NOOOO";
            emit indexing_finished();
            return;
        }
        i.update();
        if (i.get_file_path().isNull()) continue;

        //std::cerr << i.get_file_path().toString().toStdString() << std::endl;
        indexed_directory.push_back(i);
    }
  //  std::cerr << cnt << std::endl;
  //  std::cerr << "IAMDIYING" << std::endl;
    emit send_status(100);
   // std::cerr << "Point 3.1 passed\n";
    emit indexing_finished();
   // std::cerr << "Point 3.2 passed\n";
    //std::cerr << QThread::currentThread()->isRunning() << std::endl;
}

void worker::find_substring(QString const& pattern) {
    trigram_file helper;
    helper.process_trigram(pattern);
    emit send_status(1);
    qint64 cnt = 0;
  //  std::cerr << "i am there" << std::endl;
    for (auto& i : indexed_directory) {
   //     std::cerr << i.get_file_path().toString().toStdString() << std::endl;
        cnt++;
        emit send_status(cnt * 100 / indexed_directory.size());
        if (QThread::currentThread()->isInterruptionRequested()) {
            emit finish();
            return;
        }
        qint64 pos = -1;
        //for (auto j : helper.get_set_trigrams()) {
         //   std::cerr << j << " ";
        //}
       // std::cerr << std::endl << std::endl;
        if (i.get_set_trigrams().contains(helper.get_set_trigrams()) && ((pos = ensure(i.get_file_path().toString(), pattern)) != -1)) {
            emit send_position(i.get_file_path().toString(), pos);
        }
    }
    emit send_status(100);
  //  std::cerr << "AAAAAAA\n" << std::endl;
    emit finish();
}

qint64 worker::ensure(const QString &file_path, const QString &pattern) {
    QFile fileinfo(file_path);

    //std::cerr << file_path.toStdString() << " " << std::endl << pattern.toStdString() << std::endl;
    if(!fileinfo.open(QIODevice::ReadOnly | QIODevice::Text)) {
        //fileinfo.close();
        return false;
    }
    QTextStream in(&fileinfo);
    QString buffer;
    //std::cerr << file_path.toStdString() << " " << std::endl << pattern.toStdString() << std::endl;
    while (buffer.append(in.read(MAX_BUFFER)).size() >= pattern.size()) {
        qint64 pos = buffer.indexOf(pattern);
        if (pos != -1) {
            fileinfo.close();
            return pos;
        }
        buffer = buffer.right(pattern.size() - 1);
    }
    fileinfo.close();
    return -1;
}

//void worker::update_indexed_file(QString const& file_path) {

//}



