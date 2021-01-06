#include "changepassword.h"
#include "ui_changepassword.h"
#include <QPushButton>
#include <QLabel>

ChangePassword::ChangePassword(QWidget *parent) :
    QDialog(parent, Qt::WindowCloseButtonHint | Qt::Dialog),
    ui(new Ui::ChangePassword)
{
    ui->setupUi(this);
    setWindowModality(Qt::WindowModal);

    font.setPointSize(12);

    labelError = new QLabel(this);
    labelError->setFont(font);

    ui->verticalLayout->addWidget(labelError);
    labelError->hide();

    ui->buttonBox->button(QDialogButtonBox::Save)->setEnabled(false);

//    connect(ui -> buttonBox, SIGNAL(accepted()),
//            this, SLOT(accept()));
    connect(ui -> buttonBox, SIGNAL(rejected()),
            this, SLOT(reject()));

    connect(ui -> buttonBox, SIGNAL(rejected()),
            this, SLOT(clear()));

}

void ChangePassword::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    } else {
        QDialog::changeEvent(event);
    }
}

void ChangePassword::clear()
{
    ui->originalPassword->setText("");
    ui->newPassword->setText("");
    ui->inputAgain->setText("");
    labelError->setVisible(false);
}

ChangePassword::~ChangePassword()
{
    delete ui;
}


void ChangePassword::on_originalPassword_textChanged(const QString &arg1)
{
    originalPassword = arg1;
    inputFinished();
}

void ChangePassword::on_newPassword_textChanged(const QString &arg1)
{
    newPassword = arg1;
    inputFinished();
}

void ChangePassword::on_inputAgain_textChanged(const QString &arg1)
{
    inputAgain = arg1;

    //clear current message
    labelError->setText("");
    //ui->verticalLayout->addWidget(labelError);
    labelError->hide();
    ui->buttonBox->button(QDialogButtonBox::Save)->setEnabled(false);

    if (inputAgain.length() >= newPassword.length()) {
        inputFinished();
    }
}

//void ChangePassword::on_inputAgain_editingFinished()
//{
//    inputAgain = ui->inputAgain->text();
//}


void ChangePassword::inputFinished()
{
    labelError->setText("");
    //ui->verticalLayout->addWidget(labelError);
    labelError->hide();
    ui->buttonBox->button(QDialogButtonBox::Save)->setEnabled(false);

    if (!originalPassword.isEmpty() &&
            !newPassword.isEmpty() &&
            !inputAgain.isEmpty())
    {
        if (newPassword != inputAgain) {
            labelError->setText(tr("two inputs don't match"));
            //ui->verticalLayout->addWidget(labelError);
            //labelError->setVisible(true);
            labelError->show();
            ui->buttonBox->button(QDialogButtonBox::Save)->setEnabled(false);
        } else if (originalPassword == newPassword) {
            labelError->setText(tr("original password is same as new password"));
            //ui->verticalLayout->addWidget(labelError);
            //labelError->setVisible(true);
            labelError->show();
            ui->buttonBox->button(QDialogButtonBox::Save)->setEnabled(false);
        } else {
            ui->buttonBox->button(QDialogButtonBox::Save)->setEnabled(true);
        }
    }
}

void ChangePassword::errorInfo(const QString &err)
{
    if (err.isEmpty()) {
        labelError->setText(err);
        ui->verticalLayout->addWidget(labelError);
        labelError->setVisible(false);
    } else {
        labelError->setText(err);
        ui->verticalLayout->addWidget(labelError);
        labelError->setVisible(true);
    }
}

void ChangePassword::on_buttonBox_accepted()
{
    emit buttonSave(originalPassword, inputAgain);
}
