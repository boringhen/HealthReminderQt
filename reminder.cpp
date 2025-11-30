#include "reminder.h"
#include <QMessageBox>
#include <QApplication>
#include <QScreen>
#include <QTimer>

Reminder::Reminder(QObject *parent)
    : QObject(parent)
{
}

void Reminder::show(const QString &title, const QString &text)
{
    auto *msg = new QMessageBox(nullptr);

    msg->setWindowTitle(title);
    msg->setText(text);
    msg->resize(500,250);
    msg->setIcon(QMessageBox::NoIcon);
    msg->setStandardButtons(QMessageBox::Ok);
    msg->setWindowFlags(msg->windowFlags() | Qt::WindowStaysOnTopHint);

    QRect screen = QApplication::primaryScreen()->availableGeometry();
    msg->move(screen.center() - msg->rect().center());

    QTimer::singleShot(3000, msg, &QMessageBox::close);

    QApplication::beep();

    connect(msg, &QMessageBox::finished, this, &Reminder::reminderClosed);

    msg->show();
}
