#include "trigram_file.h"
#include "QFile"
#include "QTextStream"
trigram_file::trigram_file() {
    this->file_path = QVariant(QString());
}
trigram_file::trigram_file(QString const& file_path) {
    this->file_path = QVariant(file_path);
}
QVariant trigram_file::get_file_path() {
    return this->file_path;
}
QSet<qint64>& trigram_file::get_set_trigrams() {
    return this->set_trigrams;
}

void trigram_file::process_trigram(const QString &data) {
    if (data.size() < 3) {
        return;
    }
    qint64 trigram = (qint64(data[2].unicode()) << 48) + (qint64(data[1].unicode()) << 32) + (qint64(data[0].unicode()) << 16);
    for (int i = 3; i < data.size(); i++) {
        set_trigrams.insert(trigram);
        if (set_trigrams.size() > MAX_TRIGRAMS) return;
        trigram = (trigram >> 16) + (qint64(data[i].unicode()) << 48);
    }
}

void trigram_file::update() {
    if (file_path.isNull()) return;
    QFile file(file_path.toString());
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QString buffer;
        while (buffer.append(in.read(MAX_BUFFER)).size() >= 3) {
           process_trigram(buffer);
           if (set_trigrams.size() > MAX_TRIGRAMS) {
               file.close();
               this->file_path = QVariant(QString());
               return;
           }
           QString tmp = QString(buffer[buffer.size() - 2]);
           tmp += buffer[buffer.size() - 1];
           buffer.clear();
           buffer = tmp;
        }
    }
    file.close();
}
