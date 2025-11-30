#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QCloseEvent>
#include "reminder.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onStart();
    void onStop();
    void updateWater();
    void updateEye();
    void startNextCycle();
    void loadSettings();
    void saveSettings();

    void onTrayActivated(QSystemTrayIcon::ActivationReason reason);

private:
    Ui::MainWindow *ui;

    QTimer *timerWater    = nullptr;
    QTimer *timerEye      = nullptr;

    Reminder *reminder   = nullptr;

    QSystemTrayIcon *tray = nullptr;
    QMenu *trayMenu       = nullptr;

    int waterRemaining = 0;
    int eyeRemaining   = 0;

    int waterInterval = 45 * 60;
    int eyeInterval   = 20 * 60;

    bool minimizeToTray = true;

    void showReminder(const QString &title, const QString &text);

    void updateConfigFromUI();

protected:
    void closeEvent(QCloseEvent *event) override;
};
#endif // MAINWINDOW_H
