/*
 * Hyperpixel 4.0 QT example for RPi4 with QT 5.15.8 on buildroot
 * Copyright (C) 2023  Resilience Theatre
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; If not, see <http://www.gnu.org/licenses/>.
 *
 * Includes qrencode example from https://github.com/ftylitak/qzxing
 *
 * Remember to set font directory and debug console:
 *
 * QT_QPA_FONTDIR = /usr/share/fonts/dejavu/
 * QT_ASSUME_STDERR_HAS_CONSOLE = 1
 *
 * Display:
 *
 * https://shop.pimoroni.com/products/hyperpixel-4?variant=12569485443155
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QZXing.h"
#include "QTime"
#include "QPainter"
#include <QColor>
#include <QDebug>
#include <QFile>
#include <QFileSystemWatcher>
#include <fcntl.h>
#include <QSocketNotifier>

#define TELEMETRY_FIFO_OUT      "/tmp/secureptt"
#define TX_KEY_FIFO             "/tmp/udpproxy-tx-key-presentage"
#define RX_KEY_FIFO             "/tmp/udpproxy-rx-key-presentage"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    clockUpdateTimer = new QTimer(this);
    connect(clockUpdateTimer, SIGNAL(timeout()), this, SLOT(updateClock()) );
    clockUpdateTimer->start(1000);

    // WORKING BUT BLOCKING COMBO:
    watcher = new QFileSystemWatcher();
    watcher->addPath(TELEMETRY_FIFO_OUT);
    QObject::connect(watcher, SIGNAL(fileChanged(QString)), this, SLOT(fifoChanged()));
    // Remember to create fifo first (mkfifo /tmp/secureptt)
    // Before opening the FIFO for reading, make sure a writer process opens
    // the FIFO for writing. For example, you can start the writer process first.
    fifoIn.setFileName(TELEMETRY_FIFO_OUT);
    if(!fifoIn.open(QIODevice::ReadOnly | QIODevice::Unbuffered | QIODevice::Text)) {
        qDebug() << "error " << fifoIn.errorString();
    }
    qDebug() << "Moving on";

    // Key status fifo's are disabled, because udpproxy needs modification before
    // this is useful in PTT use case.
    if ( 0 ) {
        // RX_KEY_FIFO
        rxKeyFifoWatcher = new QFileSystemWatcher();
        rxKeyFifoWatcher->addPath(RX_KEY_FIFO);
        QObject::connect(rxKeyFifoWatcher, SIGNAL(fileChanged(QString)), this, SLOT(rxKeyFifoChanged()));

        rxKeyFifoIn.setFileName(RX_KEY_FIFO);
        if(!rxKeyFifoIn.open(QIODevice::ReadOnly | QIODevice::Unbuffered | QIODevice::Text)) {
            qDebug() << "error " << fifoIn.errorString();
        }

        // TX_KEY_FIFO
        txKeyFifoWatcher = new QFileSystemWatcher();
        txKeyFifoWatcher->addPath(TX_KEY_FIFO);
        QObject::connect(txKeyFifoWatcher, SIGNAL(fileChanged(QString)), this, SLOT(txKeyFifoChanged()));

        txKeyFifoIn.setFileName(RX_KEY_FIFO);
        if(!txKeyFifoIn.open(QIODevice::ReadOnly | QIODevice::Unbuffered | QIODevice::Text)) {
            qDebug() << "error " << fifoIn.errorString();
        }
    }

    ui->tabWidget->setCurrentIndex(0);
    ui->gatewayLabel_1->setStyleSheet("color:#00FF00;background-color:#00FF00;");
    ui->gatewayLabel_2->setStyleSheet("color:#000000;background-color:#000000;");
    ui->gatewayLabel_3->setStyleSheet("color:#000000;background-color:#000000;");

    ui->ethernetLabel->setStyleSheet("color:#00FF00;background-color:#00FF00;");
    ui->ethernetLabel_2->setStyleSheet("color:#000000;background-color:#000000;");
    ui->ethernetLabel_3->setStyleSheet("color:#000000;background-color:#000000;");

    ui->securePttLabel->setStyleSheet("color:#00FF00;background-color:#00FF00;");
    ui->securePttLabel_2->setStyleSheet("color:#00FF00;background-color:#000000;");
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::readFromFifo() {

        while (!fifoIn.atEnd()) {
            QByteArray line = fifoIn.readLine();
            if (!line.isEmpty()) {
                qDebug() << "Read from FIFO:" << line.trimmed();
            }
        }
    }

void MainWindow::on_wifiCodeButton_clicked()
{
    QDateTime now = QDateTime::currentDateTime();
    QString timestamp = now.toString(QLatin1String("hhmmss.zzz"));
    QString width = "200";
    QString height = "200";
    QString wifiConnectString = "WIFI:T:WPA;S:edgemapy;P:abc12345;;";
    QImage barcode = QZXing::encodeData(wifiConnectString, QZXing::EncoderFormat_QR_CODE,
                                        QSize(width.toInt(), height.toInt()),
                                        QZXing::EncodeErrorCorrectionLevel_H);

    // Green QR code
    QPixmap source_image = QPixmap::fromImage(barcode);
    QRgb base_color = 0x0000FF00;
    QPixmap new_image = source_image;
    QPainter painter(&new_image);
    painter.setCompositionMode(QPainter::CompositionMode_Multiply);
    painter.fillRect(new_image.rect(), base_color);
    painter.end();
    ui->qrLabel->setPixmap(new_image);
    ui->qrLabel->show();
}

void MainWindow::on_accessUrlButton_clicked()
{
    QString width = "200";
    QString height = "200";
    QString edgemapUrlString = "http://edgemapy/";
    QImage barcode = QZXing::encodeData(edgemapUrlString, QZXing::EncoderFormat_QR_CODE,
                                        QSize(width.toInt(), height.toInt()),
                                        QZXing::EncodeErrorCorrectionLevel_H);
    // Green QR code
    QPixmap source_image = QPixmap::fromImage(barcode);
    QRgb base_color = 0x0000FF00;
    QPixmap new_image = source_image;
    QPainter painter(&new_image);
    painter.setCompositionMode(QPainter::CompositionMode_Multiply);
    painter.fillRect(new_image.rect(), base_color);
    painter.end();
    ui->qrLabel->setPixmap(new_image);
    ui->qrLabel->show();
}

void MainWindow::updateClock()
{
    // ui->timeLabel->setText(QTime::currentTime().toString("hh:mm:ss"));
    QDateTime date = QDateTime::currentDateTime();
    QString formattedTime = date.toString("hh:mm:ss"); // .zzz
    QByteArray formattedTimeMsg = formattedTime.toLocal8Bit();
    ui->timeLabel->setText(formattedTimeMsg);
    // on_wifiCodeButton_clicked();
}


void MainWindow::on_backlight10_clicked()
{
    QFile brightnessFile("/sys/class/backlight/10-0045/brightness");
    if (!brightnessFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Failed to open brightness file:" << brightnessFile.errorString();
        return;
    }
    QTextStream out(&brightnessFile);
    out << "1";
    brightnessFile.close();
}

void MainWindow::on_backlight100_clicked()
{
    QFile brightnessFile("/sys/class/backlight/10-0045/brightness");
    if (!brightnessFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Failed to open brightness file:" << brightnessFile.errorString();
        return;
    }
    QTextStream out(&brightnessFile);
    out << "255";
    brightnessFile.close();
}

int MainWindow::fifoChanged()
{
    QTextStream in(&fifoIn);
    QString line = in.readAll();

    qDebug() << "FIFO Received: " << line;

    if ( line.length() == 0 ) {
        qDebug() << "EMPTY FIFO Received";
        return 0;
    }

    if (line.contains("rx-on")) {
        ui->fifoStatusRx->setText("RX");
        ui->fifoStatusRx->setStyleSheet("background-color: #00FF00; border:2px solid #00FF00;");
        ui->tabWidget->setCurrentIndex(1);
        return 0;
    }

    if (line.contains("rx-off")) {
        ui->fifoStatusRx->setText("");
        ui->fifoStatusRx->setStyleSheet("background-color: #000000; border:2px solid #00FF00;");
        return 0;
    }
    if (line.contains("tx-on")) {
        ui->fifoStatusTx->setText("TX");
        ui->fifoStatusTx->setStyleSheet("background-color: #00FF00; border:2px solid #00FF00;");
        ui->tabWidget->setCurrentIndex(1);
        return 0;
    }
    if (line.contains("tx-off")) {
        ui->fifoStatusTx->setText("");
        ui->fifoStatusTx->setStyleSheet("background-color: #000000; border:2px solid #00FF00;");
        return 0;
    }
    return 0;
}

int MainWindow::rxKeyFifoChanged()
{
    QTextStream in(&rxKeyFifoIn);
    QString line = in.readAll();
    if ( line.length() == 0 ) {
        qDebug() << "EMPTY FIFO Received";
        return 0;
    }
    qDebug() << "RX KEY: " << line;
    return 0;
}

int MainWindow::txKeyFifoChanged()
{
    QTextStream in(&txKeyFifoIn);
    QString line = in.readAll();
    if ( line.length() == 0 ) {
        qDebug() << "EMPTY FIFO Received";
        return 0;
    }
    qDebug() << "TX KEY: " << line;
    return 0;
}




void MainWindow::on_gateway_1_clicked()
{
    ui->gatewayLabel_1->setStyleSheet("color:#00FF00;background-color:#00FF00;");
    ui->gatewayLabel_2->setStyleSheet("color:#000000;background-color:#000000;");
    ui->gatewayLabel_3->setStyleSheet("color:#000000;background-color:#000000;");
}


void MainWindow::on_gateway_2_clicked()
{
    ui->gatewayLabel_1->setStyleSheet("color:#000000;background-color:#000000;");
    ui->gatewayLabel_2->setStyleSheet("color:#00FF00;background-color:#00FF00;");
    ui->gatewayLabel_3->setStyleSheet("color:#000000;background-color:#000000;");
}


void MainWindow::on_gateway_3_clicked()
{
    ui->gatewayLabel_1->setStyleSheet("color:#000000;background-color:#000000;");
    ui->gatewayLabel_2->setStyleSheet("color:#000000;background-color:#000000;");
    ui->gatewayLabel_3->setStyleSheet("color:#00FF00;background-color:#00FF00;");
}



void MainWindow::on_ethernetDhcpButton_clicked()
{
    ui->ethernetLabel->setStyleSheet("color:#00FF00;background-color:#00FF00;");
    ui->ethernetLabel_2->setStyleSheet("color:#000000;background-color:#000000;");
    ui->ethernetLabel_3->setStyleSheet("color:#000000;background-color:#000000;");
}


void MainWindow::on_ethernetDhcpButton_2_clicked()
{
    ui->ethernetLabel->setStyleSheet("color:#00FF00;background-color:#000000;");
    ui->ethernetLabel_2->setStyleSheet("color:#000000;background-color:#00FF00;");
    ui->ethernetLabel_3->setStyleSheet("color:#000000;background-color:#000000;");
}


void MainWindow::on_ethernetDhcpButton_3_clicked()
{
    ui->ethernetLabel->setStyleSheet("color:#00FF00;background-color:#000000;");
    ui->ethernetLabel_2->setStyleSheet("color:#000000;background-color:#000000;");
    ui->ethernetLabel_3->setStyleSheet("color:#000000;background-color:#00FF00;");
}


void MainWindow::on_securePttButton_clicked()
{
    ui->securePttLabel->setStyleSheet("color:#00FF00;background-color:#00FF00;");
    ui->securePttLabel_2->setStyleSheet("color:#00FF00;background-color:#000000;");
}


void MainWindow::on_securePttButton_2_clicked()
{
    ui->securePttLabel->setStyleSheet("color:#00FF00;background-color:#000000;");
    ui->securePttLabel_2->setStyleSheet("color:#00FF00;background-color:#00FF00;");
}

