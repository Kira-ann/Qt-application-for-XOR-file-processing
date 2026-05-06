#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    QTimer* timer;
    bool isProcessing;

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

signals:
    void progressUpdated(int percent);
    void statusMessage(const QString& msg);

private slots:
    void on_pushButton_using_XOR_clicked();
    void ProcessingFile();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
