#ifndef WORKER_H
#define WORKER_H
#include <QVector>
#include <QObject>
#include <QFileSystemWatcher>
#include "trigram_file.h"
#include "iostream"
#endif // WORKER_H


class worker : public QObject {
    Q_OBJECT

public:
    worker();
    worker(QString const& directory);
  //  worker(worker& c_worker);
    ~worker();

    QVariant directory;
public slots:
    void take_control();
    void find_substring(QString const& pattern);
  //  void update_indexed_file(QString const& file_path);
    void initiate(QString const& directory);
signals:
    void send_position(QString const& file_path, qint64 index);
    void indexing_finished();
    void finish();
    void send_status(qint64 val);

private:
    qint64 ensure(QString const& file_path, QString const& pattern);

    QVector<trigram_file> indexed_directory;
    QFileSystemWatcher watcher;
};
