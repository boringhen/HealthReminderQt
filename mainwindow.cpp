#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QApplication>
#include <QScreen>
#include <QStyle>
#include <QSettings>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    loadSettings();

    ui->setupUi(this);
    ui->spinDrink->setValue(waterInterval / 60);
    ui->spinEye->setValue(eyeInterval / 60);
    ui->chkMinimizeToTray->setChecked(minimizeToTray);

    timerWater = new QTimer(this);
    timerEye = new QTimer(this);

    reminder = new Reminder(this);

    connect(reminder, &Reminder::reminderClosed,this, &MainWindow::startNextCycle);

    connect(timerWater, &QTimer::timeout, this, &MainWindow::updateWater);
    connect(timerEye, &QTimer::timeout, this, &MainWindow::updateEye);

    connect(ui->btnStart, &QPushButton::clicked, this, &MainWindow::onStart);
    connect(ui->btnStop, &QPushButton::clicked, this, &MainWindow::onStop);

    //托盘相关
    tray = new QSystemTrayIcon(this);
    tray->setIcon(
        style()->standardIcon(QStyle::SP_ComputerIcon)
    );


    trayMenu = new QMenu(this);

    trayMenu->addAction("打开设置", this, &MainWindow::show);

    trayMenu->addSeparator();
    trayMenu->addAction("退出", qApp, &QApplication::quit);

    tray->setContextMenu(trayMenu);
    tray->show();

    connect(tray, &QSystemTrayIcon::activated,
            this, &MainWindow::onTrayActivated);

    tray->setVisible(true);
}


void MainWindow::onStart()
{
    updateConfigFromUI();

    waterRemaining = waterInterval;
    eyeRemaining = eyeInterval;

    timerWater->start(1000);
    timerEye->start(1000);

    ui->lbDrink->setText("喝水倒计时：启动");
    ui->lbEye->setText("远眺倒计时：启动");
}


void MainWindow::onStop()
{
    timerWater->stop();
    timerEye->stop();

    ui->lbDrink->setText("喝水倒计时：已停止");
    ui->lbEye->setText("远眺倒计时：已停止");
}

void MainWindow::onTrayActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger ||
        reason == QSystemTrayIcon::DoubleClick)
    {
        showNormal();
        activateWindow();
        raise();
    }
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    hide();
    event->ignore();
    updateConfigFromUI();
    saveSettings();

    if (minimizeToTray) {
        hide();
        event->ignore();
    } else {
        event->accept();
    }
}

void MainWindow::startNextCycle()
{
    waterRemaining = waterInterval;
    eyeRemaining   = eyeInterval;

    timerWater->start(1000);
    timerEye->start(1000);

    ui->lbDrink->setText("喝水倒计时：启动");
    ui->lbEye->setText("远眺倒计时：启动");
}


void MainWindow::loadSettings()
{
    QSettings s("config.ini", QSettings::IniFormat);

    waterInterval   = s.value("waterInterval", 45*60).toInt();
    eyeInterval     = s.value("eyeInterval", 20*60).toInt();
    minimizeToTray  = s.value("minimizeToTray", true).toBool();
}


void MainWindow::saveSettings()
{
    QSettings s("config.ini", QSettings::IniFormat);

    s.setValue("waterInterval",  waterInterval);
    s.setValue("eyeInterval",    eyeInterval);
    s.setValue("minimizeToTray", minimizeToTray);
}


void MainWindow::updateWater()
{
    if (waterRemaining > 0)
    {
        waterRemaining--;

        int m = waterRemaining / 60;
        int s = waterRemaining % 60;

        ui->lbDrink->setText(
            QString("喝水倒计时：%1:%2")
                .arg(m, 2, 10, QChar('0'))
                .arg(s, 2, 10, QChar('0'))
            );
    }
    else
    {
        // 防止重复触发
        if (timerWater->isActive())
            timerWater->stop();

        ui->lbDrink->setText("该喝水了");

        reminder->show("喝水提醒", "工作一段时间了，记得喝水");
    }
}


void MainWindow::updateEye()
{
    if (eyeRemaining > 0) {
        eyeRemaining--;
        int m = eyeRemaining / 60;
        int s = eyeRemaining % 60;
        ui->lbEye->setText(
            QString("远眺倒计时：%1:%2")
                .arg(m, 2, 10, QChar('0'))
                .arg(s, 2, 10, QChar('0'))
            );
    } else {
        timerEye->stop();
        ui->lbEye->setText("该远眺了");

        reminder->show("远眺提醒", "工作一段时间了，记得远眺");
    }
}

void MainWindow::updateConfigFromUI()
{
    waterInterval = ui->spinDrink->value() * 60;
    eyeInterval   = ui->spinEye->value()   * 60;

    minimizeToTray = ui->chkMinimizeToTray->isChecked();
}


void MainWindow::showReminder(const QString &title, const QString &text)
{
    auto *msg = new QMessageBox(nullptr);

    msg->setWindowTitle(title);
    msg->setText(text);
    msg->resize(500, 250);

    msg->setIcon(QMessageBox::NoIcon);
    msg->setStandardButtons(QMessageBox::Ok);

    msg->setWindowFlags(msg->windowFlags() | Qt::WindowStaysOnTopHint);

    QRect screen = QApplication::primaryScreen()->availableGeometry();
    msg->move(
        screen.center() - msg->rect().center()
        );

    QTimer::singleShot(3000, msg, &QMessageBox::close);

    QApplication::beep();

    msg->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}
