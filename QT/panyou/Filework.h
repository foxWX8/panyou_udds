#ifndef FILEWORKER_H
#define FILEWORKER_H
#pragma execution_character_set("utf-8")
#include <QObject>
#include <QFile>
#include <QThread>
#include <QTextStream>
#include <QMessageBox>
#include <QThread>
#include <QString>

class FileWriterThread : public QThread
{
    Q_OBJECT
public:
    explicit FileWriterThread(const QString& path, QObject* parent = nullptr)
        : QThread(parent), m_path(path) {
    }

signals:
    void progressChanged(int value);
    void finished();
    void errorOccurred(const QString& msg);

protected:
    void run() override {
        QFile file(m_path + "/hello.txt");
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            emit errorOccurred(tr("无法创建文件: %1").arg(file.errorString()));
            return;
        }

        QTextStream out(&file);
        out.setCodec("UTF-8");
        out.setGenerateByteOrderMark(true);

        try {
            for (int i = 0; i < 100; ++i) {
                if (isInterruptionRequested()) {
                    file.remove();
                    throw std::runtime_error(u8"操作被用户中止");
                }

                out << tr("你好，“文件”\n");

                if (out.status() != QTextStream::Ok) {
                    throw std::runtime_error(u8"写入文件失败");
                }

                emit progressChanged(i + 1);
                msleep(500);
            }
        }
        catch (const std::exception& e) {
            emit errorOccurred(QString::fromLocal8Bit(e.what()));
            return;
        }

        file.close();
        emit finished();
    }

private:
    QString m_path;
};

#endif // FILEWORKER_H

