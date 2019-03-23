#ifndef TRIGRAM_FILE_H
#define TRIGRAM_FILE_H

#include "QString"
#include "QSet"
#include "QVariant"

#endif // TRIGRAM_FILE_H

const int MAX_TRIGRAMS = 20000;
const int MAX_BUFFER = 10 * 1024 * 1024;

class trigram_file {
public:
    trigram_file();
    trigram_file(QString const& file_path);
    QVariant get_file_path();
    QSet<qint64>& get_set_trigrams();
    void update();
    void process_trigram(QString const& data);

private:
    QVariant file_path;
    QSet<qint64> set_trigrams;
};
