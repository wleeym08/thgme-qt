#ifndef EXTRACTORWORKER_H
#define EXTRACTORWORKER_H

#include <QObject>

class ExtractorWorker : public QObject
{
    Q_OBJECT
public:
    ExtractorWorker(QString, QString, QString);
    ~ExtractorWorker();
signals:
    void message(QString);
    void error(QString);
    void finished();

public slots:
    void process();
private:
    QString fmt_path;
    QString dat_path;
    QString out_path;
};

#endif // EXTRACTORWORKER_H
