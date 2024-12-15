#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QFileSystemWatcher>
#include <QFile>
#include <QSocketNotifier>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void updateClock();
    void on_wifiCodeButton_clicked();
    void on_accessUrlButton_clicked();
    void on_backlight10_clicked();
    void on_backlight100_clicked();
    int fifoChanged();
    int rxKeyFifoChanged();
    int txKeyFifoChanged();


    void on_gateway_1_clicked();

    void on_gateway_2_clicked();

    void on_gateway_3_clicked();

    void readFromFifo();

    void on_ethernetDhcpButton_clicked();

    void on_ethernetDhcpButton_2_clicked();

    void on_ethernetDhcpButton_3_clicked();

    void on_securePttButton_clicked();

    void on_securePttButton_2_clicked();

private:
    Ui::MainWindow *ui;
    QTimer *clockUpdateTimer;
    /* FIFO */
    QFile fifoIn;
    QFileSystemWatcher *watcher;

    QFile rxKeyFifoIn;
    QFileSystemWatcher *rxKeyFifoWatcher;

    QFile txKeyFifoIn;
    QFileSystemWatcher *txKeyFifoWatcher;

    QSocketNotifier *socketNotifier = nullptr;
    QFile *fifoFile = nullptr;

};
#endif // MAINWINDOW_H
