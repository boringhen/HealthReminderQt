#pragma once
#include <QObject>

class Reminder : public QObject
{
    Q_OBJECT

signals:
    void reminderClosed();

public:
    explicit Reminder(QObject *parent = nullptr);

    void show(const QString &title, const QString &text);
};

