#include "update.h"
#include "ui_update.h"

#include <QDesktopServices>
#include <QUrl>

Update::Update(QWidget *parent, const int &vsmApi, const int &clientApi, const QString &url) :
    QDialog(parent, Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint),
    ui(new Ui::Update),
    updateUrl(url)
{
    ui->setupUi(this);
    ui->vsmApiLabel->setText(QString::number(vsmApi));
    ui->clientApiLabel->setText(QString::number(clientApi));
}

Update::~Update()
{
    delete ui;
}

void Update::on_pushButton_clicked()
{
    QDesktopServices::openUrl(QUrl(updateUrl));
    this->parentWidget()->close();
    delete this;
}
