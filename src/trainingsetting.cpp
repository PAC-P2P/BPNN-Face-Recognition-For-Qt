#include "trainingsetting.h"
#include "ui_trainingsetting.h"
#include "facetrain.h"

#define MAXTRAININGTIMES 100000

TrainingSetting::TrainingSetting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TrainingSetting)
{
    ui->setupUi(this);
}

TrainingSetting::~TrainingSetting()
{
    delete ui;
}

void TrainingSetting::on_buttonBox_accepted()
{
    int trainingTimes = ui->spinBox->value();
    if(trainingTimes <= 0)
    {
        QMessageBox::warning(NULL, "Warning", "Please input a appropriate training times!", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    }
    else if(trainingTimes > MAXTRAININGTIMES)
    {
        QMessageBox::warning(NULL, "Warning", "Training times is too much!", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    }
    else
    {
        facetrain(trainingTimes);
        QMessageBox::about(NULL, "Completed", "Training is completed!");
    }
}

void TrainingSetting::on_buttonBox_rejected()
{
    close();
}
