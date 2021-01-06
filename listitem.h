#ifndef LISTITEM_H
#define LISTITEM_H

#include <QWidget>

#include <winsock2.h>
#include <iphlpapi.h>
#include <icmpapi.h>
#include <stdio.h>

#include <QTimer>

#include <QWinEventNotifier>


namespace Ui {
class ListItem;
}

class ListItem : public QWidget
{
    Q_OBJECT

public:
    explicit ListItem(QWidget *parent = nullptr);
    void setName(const QString &vpnName);
    void setIP(const QString &ip);

    //must setIP first before call this function.
    int setIcmpDelay();

    void startTimer();
    void stopTimer();

    ~ListItem();

signals:
    void clicked(QString name, ListItem *item);
    void speed(int level);

private slots:
    void setDelay();
    void on_connectButton_clicked();

private:
    QTimer *timer;

    QWinEventNotifier *winNotifier;

    LPVOID ReplyBuffer;

    QString IP;
    Ui::ListItem *ui;
};

#endif // LISTITEM_H
