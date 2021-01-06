#include "listitem.h"
#include "ui_listitem.h"

ListItem::ListItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ListItem)
{
    ui->setupUi(this);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &ListItem::setIcmpDelay);
    startTimer();
}

void ListItem::setName(const QString &vpnName)
{
    ui->nameLabel->setText(vpnName);
}

void ListItem::setIP(const QString &ip)
{
    IP = ip;
}

void ListItem::startTimer()
{
    timer->start(3000);
}

void ListItem::stopTimer()
{
    timer->stop();
}

ListItem::~ListItem()
{
    timer->stop();
    disconnect(winNotifier, &QWinEventNotifier::activated, this, &ListItem::setDelay);
    delete timer;
    delete winNotifier;
    delete ui;
}

void ListItem::on_connectButton_clicked()
{
    emit clicked(ui->nameLabel->text(), this);
}


int ListItem::setIcmpDelay()
{

    // Declare and initialize variables

    HANDLE hIcmpFile;
    unsigned long ipaddr = INADDR_NONE;
    DWORD dwRetVal = 0;
    DWORD dwError = 0;
    char SendData[] = "Data Buffer";
    ReplyBuffer = NULL;
    DWORD ReplySize = 0;


    ipaddr = inet_addr(IP.toStdString().c_str());


    hIcmpFile = IcmpCreateFile();
    if (hIcmpFile == INVALID_HANDLE_VALUE) {
        printf("\tUnable to open handle.\n");
        printf("IcmpCreatefile returned error: %ld\n", GetLastError());
        return 1;
    }
    // Allocate space for at a single reply
    ReplySize = sizeof (ICMP_ECHO_REPLY) + sizeof (SendData) + 8;
    ReplyBuffer = (VOID *) malloc(ReplySize);
    if (ReplyBuffer == NULL) {
        printf("\tUnable to allocate memory for reply buffer\n");
        return 1;
    }

    // setup icmp reply event
    HANDLE icmpEvent = CreateEvent(
                NULL,               // default security attributes
                FALSE,               // auto-reset event
                FALSE,              // initial state is nonsignaled
                TEXT("icmp_replied")  // object name
                );

    if (icmpEvent == NULL) {
        printf("CreateEvent failed (%d)\n", GetLastError());
        return false;
    }

    //called asynchronously
    dwRetVal = IcmpSendEcho2(hIcmpFile, icmpEvent, NULL, NULL,
                             ipaddr, SendData, sizeof (SendData), NULL,
                             ReplyBuffer, ReplySize, 1000);

    winNotifier = new QWinEventNotifier(icmpEvent);

    connect(winNotifier, &QWinEventNotifier::activated, this, &ListItem::setDelay);


    if (dwRetVal != 0) {
        PICMP_ECHO_REPLY pEchoReply = (PICMP_ECHO_REPLY) ReplyBuffer;
        struct in_addr ReplyAddr;
        ReplyAddr.S_un.S_addr = pEchoReply->Address;
        printf("\tSent icmp message to \n");
        if (dwRetVal > 1) {
            printf("\tReceived %ld icmp message responses\n", dwRetVal);
            printf("\tInformation from the first response:\n");
        } else {
            printf("\tReceived %ld icmp message response\n", dwRetVal);
            printf("\tInformation from this response:\n");
        }
        printf("\t  Received from %s\n", inet_ntoa(ReplyAddr));
        printf("\t  Status = %ld  ", pEchoReply->Status);
        switch (pEchoReply->Status) {
        case IP_DEST_HOST_UNREACHABLE:
            printf("(Destination host was unreachable)\n");
            break;
        case IP_DEST_NET_UNREACHABLE:
            printf("(Destination Network was unreachable)\n");
            break;
        case IP_REQ_TIMED_OUT:
            printf("(Request timed out)\n");
            break;
        default:
            printf("\n");
            break;
        }

        printf("\t  Roundtrip time = %ld milliseconds\n",
               pEchoReply->RoundTripTime);

    } else {
        printf("Call to IcmpSendEcho2 failed.\n");

        dwError = GetLastError();
        switch (dwError) {
        case IP_BUF_TOO_SMALL:
            printf("\tReplyBufferSize to small\n");
            break;
        case IP_REQ_TIMED_OUT:
            printf("\tRequest timed out\n");
            break;
        default:
            printf("\tExtended error returned: %ld\n", dwError);
            break;
        }
        return 1;
    }
    return 0;

}

void ListItem::setDelay()
{
    PICMP_ECHO_REPLY pEchoReply = (PICMP_ECHO_REPLY) ReplyBuffer;

    if( pEchoReply->Status == IP_SUCCESS) {

        if( pEchoReply->RoundTripTime < 100) {
            ui->speedLabel->setPixmap(QPixmap(QString::fromUtf8(":/images/icon_signl_three.svg")));
            emit speed(1);
        } else if (pEchoReply->RoundTripTime < 200 ) {
            ui->speedLabel->setPixmap(QPixmap(QString::fromUtf8(":/images/icon_signl_two.svg")));
            emit speed(2);
        } else {
            ui->speedLabel->setPixmap(QPixmap(QString::fromUtf8(":/images/icon_signl_one.svg")));
            emit speed(3);
        }
        ui->label->setText(QString::number(pEchoReply->RoundTripTime) + "ms");
    } else {

        ui->speedLabel->setPixmap(QPixmap(QString::fromUtf8(":/images/icon_signl_no.svg")));
        ui->label->setText(tr("NA"));
        emit speed(0);
    }
}

