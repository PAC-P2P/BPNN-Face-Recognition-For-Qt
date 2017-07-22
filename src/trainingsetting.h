#ifndef TRAININGSETTING_H
#define TRAININGSETTING_H

#include <QDialog>
#include <QMessageBox>

namespace Ui {
class TrainingSetting;
}

class TrainingSetting : public QDialog
{
    Q_OBJECT

public:
    explicit TrainingSetting(QWidget *parent = 0);
    ~TrainingSetting();

private:
    Ui::TrainingSetting *ui;

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
};

#endif // TRAININGSETTING_H
