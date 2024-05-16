#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>


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

private:
    Ui::MainWindow *ui;
    QTimer *clockUpdateTimer;
};
#endif // MAINWINDOW_H
