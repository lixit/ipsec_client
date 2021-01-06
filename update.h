#ifndef UPDATE_H
#define UPDATE_H

#include <QDialog>

namespace Ui {
class Update;
}

class Update : public QDialog
{
    Q_OBJECT

public:
    explicit Update(QWidget *parent, const int &vsmApi, const int &clientApi, const QString &url);
    ~Update();

private slots:
    void on_pushButton_clicked();

private:
    Ui::Update *ui;
    QString updateUrl;
};

#endif // UPDATE_H
