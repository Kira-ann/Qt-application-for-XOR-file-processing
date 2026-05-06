#include "worker.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QByteArray>

void Worker::process() {
    QDir dir = QDir::current();
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
    QStringList fileNames = dir.entryList(QStringList() << mask);
    QByteArray key_bytes = QByteArray::fromHex(inputKey.toUtf8());
    int currentFile = 1;
    const size_t bufferSize = 16384;
    int totalFiles = fileNames.size();

    if (outputPath.isEmpty()){
        outputPath = dir.path();
    }
    for (const QString& fileName : fileNames) {
        emit statusMessage(QString("Обработка файла %1 из %2: %3").arg(currentFile).arg(totalFiles).arg(fileName));
        currentFile++;

        emit progressUpdated(0);
        unsigned int count = 1;
        qint64 bufferRead;
        int percent;

        QFileInfo fileInfo(fileName);
        QString baseName = fileInfo.completeBaseName();
        QString extension = fileInfo.suffix();
        QString outputFile = outputPath + "/" + baseName + "_xor." + extension;

        if (counter){
            while(QFile::exists(outputFile)){
                outputFile = outputPath + "/" + baseName + "_xor_" + QString::number(count) + "." + extension;
                count++;
            }
        }

        QFile file(fileName);
        QFile outFile(outputFile);
        if (!file.open(QIODevice::ReadOnly)) {
            emit statusMessage("Не удалось открыть исходный файл: " + fileName);
            continue;
        }
        if (!outFile.open(QIODevice::WriteOnly)) {
            emit statusMessage("Проверьте корректность выходного пути: " + outputPath);
            continue;
        }
        qint64 fileSize = file.size();
        std::array<unsigned char, bufferSize> buffer;
        size_t totalProcessed = 0;


        while (true) {
            bufferRead = file.read(reinterpret_cast<char*>(buffer.data()), bufferSize);
            if (bufferRead <= 0){
                emit progressUpdated(100);
                break;
            }
            for (size_t i = 0; i < bufferRead; ++i){
                buffer[i] ^= key_bytes[(totalProcessed + i) % key_bytes.size()];
            }
            outFile.write(reinterpret_cast<const char*>(buffer.data()), bufferRead);
            totalProcessed += bufferRead;
            percent = 100 * totalProcessed/fileSize;
            emit progressUpdated(percent);
        }

        file.close();
        outFile.close();

        if (remove){
            QFile::remove(fileName);
        }
    }
    emit statusMessage("Обработка завершена");
    emit processingFinished();
    emit finished();
}
void Worker::setData(QString keys, QString inputOutputPath, QString inputMask, bool removeFile, bool inputCount){
    inputKey = keys;
    outputPath = inputOutputPath;
    mask = inputMask;
    remove = removeFile;
    counter = inputCount;
}
