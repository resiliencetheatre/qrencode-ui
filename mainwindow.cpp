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
 * Remember to set font directory:
 *
 * QT_QPA_FONTDIR=/usr/share/fonts/dejavu/
 *
 * Display:
 *
 * https://shop.pimoroni.com/products/hyperpixel-4?variant=12569485443155
 *
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QZXing.h"
#include "QTime"
#include "QPainter"
#include <QColor>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    clockUpdateTimer = new QTimer(this);
    connect(clockUpdateTimer, SIGNAL(timeout()), this, SLOT(updateClock()) );
    clockUpdateTimer->start(500);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QDateTime now = QDateTime::currentDateTime();
    QString timestamp = now.toString(QLatin1String("hhmmss.zzz"));
    QString width = "400";
    QString height = "400";
    QImage barcode = QZXing::encodeData(timestamp, QZXing::EncoderFormat_QR_CODE,
                                        QSize(width.toInt(), height.toInt()),
                                        QZXing::EncodeErrorCorrectionLevel_H);
    /* White QR code
    ui->qrLabel->setPixmap(QPixmap::fromImage(barcode));
    ui->qrLabel->show();
    */

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
    QString formattedTime = date.toString("hhmmss.zzz");
    QByteArray formattedTimeMsg = formattedTime.toLocal8Bit();
    ui->timeLabel->setText(formattedTimeMsg);
    on_pushButton_clicked();
}

