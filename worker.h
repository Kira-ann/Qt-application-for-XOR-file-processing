#ifndef WORKER_H
#define WORKER_H

#include <QString>
#include <QObject>

class Worker : public QObject {
    Q_OBJECT
private:
    QString inputKey;
    QString outputPath;
    QString mask;

    bool remove;
    bool counter;
public:
    void setData(QString keys, QString inputOutputPath, QString InputMask, bool removeFile, bool inputCount);
public slots:
    void process();
signals:
    void progressUpdated(int percent);
    void statusMessage(const QString& msg);
    void finished();
    void processingFinished();
};

#endif // WORKER_H
