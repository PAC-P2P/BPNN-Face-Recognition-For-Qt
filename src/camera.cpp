#include "camera.h"
#include "ui_camera.h"
#include "imagesettings.h"
#include "trainingsetting.h"
#include "facetrain.h"
#include "dir.h"

#include <QCameraInfo>
#include <QMessageBox>
#include <QtWidgets>
#include <QDir>

// 路径
#define DATA "data"
#define IMAGESREC "data/images/imagesRec"
#define IMAGESTRAIN "data/images/imagesTrain"
#define INPUTDIR "images/imagesTrain"

// 训练集名
#define TRAINNAME "/imagesSet_train.list"

// 测试集名
#define TESTALLNAME "/imagesSet_testAll.list"

// 网络名
#define NETNAME "/facenet.net"

// 识别集图片名
#define IMAGESNAMEREC "/rec_01.pgm"

// 识别集名
#define RECLISTNAME "/imagesSet_rec.list"

// 最大录入次数
#define INPUTTIMES 20

//最大训练次数10万次
#define MAXTRAINTIMES 100000

Q_DECLARE_METATYPE(QCameraInfo)

Camera::Camera(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Camera),
    camera(0),
    imageCapture(0),
    isCapturingImage(false),
    applicationExiting(false),
    inputTimes(0),
    trainTimes(0),
    userId()
{
    ui->setupUi(this);

    //Camera devices:

    QActionGroup *videoDevicesGroup = new QActionGroup(this);
    videoDevicesGroup->setExclusive(true);
    foreach (const QCameraInfo &cameraInfo, QCameraInfo::availableCameras()) {
        QAction *videoDeviceAction = new QAction(cameraInfo.description(), videoDevicesGroup);
        videoDeviceAction->setCheckable(true);
        videoDeviceAction->setData(QVariant::fromValue(cameraInfo));
        if (cameraInfo == QCameraInfo::defaultCamera())
            videoDeviceAction->setChecked(true);

        ui->menuDevices->addAction(videoDeviceAction);
    }

    connect(videoDevicesGroup, SIGNAL(triggered(QAction*)), SLOT(updateCameraDevice(QAction*)));
    connect(ui->captureWidget, SIGNAL(currentChanged(int)), SLOT(updateCaptureMode()));

    setCamera(QCameraInfo::defaultCamera());
}

Camera::~Camera()
{
    delete imageCapture;
    delete camera;
}

void Camera::setCamera(const QCameraInfo &cameraInfo)
{
    delete imageCapture;
    delete camera;

    camera = new QCamera(cameraInfo);

    connect(camera, SIGNAL(stateChanged(QCamera::State)), this, SLOT(updateCameraState(QCamera::State)));
    connect(camera, SIGNAL(error(QCamera::Error)), this, SLOT(displayCameraError()));

    imageCapture = new QCameraImageCapture(camera);

    connect(ui->exposureCompensation, SIGNAL(valueChanged(int)), SLOT(setExposureCompensation(int)));

    camera->setViewfinder(ui->viewfinder);

    updateCameraState(camera->state());
    updateLockStatus(camera->lockStatus(), QCamera::UserRequest);

    connect(imageCapture, SIGNAL(readyForCaptureChanged(bool)), this, SLOT(readyForCapture(bool)));
    connect(imageCapture, SIGNAL(imageCaptured(int,QImage)), this, SLOT(processCapturedImage(int,QImage)));
    connect(imageCapture, SIGNAL(imageSaved(int,QString)), this, SLOT(imageSaved(int,QString)));
    connect(imageCapture, SIGNAL(error(int,QCameraImageCapture::Error,QString)), this,
            SLOT(displayCaptureError(int,QCameraImageCapture::Error,QString)));

    connect(camera, SIGNAL(lockStatusChanged(QCamera::LockStatus,QCamera::LockChangeReason)),
            this, SLOT(updateLockStatus(QCamera::LockStatus,QCamera::LockChangeReason)));

    ui->captureWidget->setTabEnabled(0, (camera->isCaptureModeSupported(QCamera::CaptureStillImage)));
    ui->captureWidget->setTabEnabled(1, (camera->isCaptureModeSupported(QCamera::CaptureVideo)));

    updateCaptureMode();
    camera->start();
}

void Camera::keyPressEvent(QKeyEvent * event)
{
    if (event->isAutoRepeat())
        return;

    switch (event->key()) {
    case Qt::Key_CameraFocus:
        displayViewfinder();
        camera->searchAndLock();
        event->accept();
        break;
    case Qt::Key_Camera:
        if (camera->captureMode() == QCamera::CaptureStillImage) {
            if(ui->captureWidget->currentIndex())
                on_takeImageButton_Rec_clicked();
            else
                on_takeImageButton_Input_clicked();
        }
        event->accept();
        break;
    default:
        QMainWindow::keyPressEvent(event);
    }
}

void Camera::keyReleaseEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat())
        return;

    switch (event->key()) {
    case Qt::Key_CameraFocus:
        camera->unlock();
        break;
    default:
        QMainWindow::keyReleaseEvent(event);
    }
}

void Camera::processCapturedImage(int requestId, const QImage& img)
{
    Q_UNUSED(requestId);
    QImage scaledImage = img.scaled(ui->viewfinder->size(),
                                    Qt::KeepAspectRatio,
                                    Qt::SmoothTransformation);

    ui->lastImagePreviewLabel->setPixmap(QPixmap::fromImage(scaledImage));

    // Display captured image for 2 seconds.
    displayCapturedImage();
    QTimer::singleShot(2000, this, SLOT(displayViewfinder()));
}

void Camera::configureCaptureSettings()
{
    configureImageSettings();
}

void Camera::configureImageSettings()
{
    ImageSettings settingsDialog(imageCapture);

    settingsDialog.setImageSettings(imageSettings);

    if (settingsDialog.exec()) {
        imageSettings = settingsDialog.imageSettings();
        imageCapture->setEncodingSettings(imageSettings);
    }
}

void Camera::toggleLock()
{
    switch (camera->lockStatus()) {
    case QCamera::Searching:
    case QCamera::Locked:
        camera->unlock();
        break;
    case QCamera::Unlocked:
        camera->searchAndLock();
    }
}

void Camera::updateLockStatus(QCamera::LockStatus status, QCamera::LockChangeReason reason)
{
    QColor indicationColor = Qt::black;

    switch (status) {
    case QCamera::Searching:
        indicationColor = Qt::yellow;
        ui->statusbar->showMessage(tr("Focusing..."));
        ui->lockButton->setText(tr("Focusing..."));
        break;
    case QCamera::Locked:
        indicationColor = Qt::darkGreen;
        ui->lockButton->setText(tr("Unlock"));
        ui->statusbar->showMessage(tr("Focused"), 2000);
        break;
    case QCamera::Unlocked:
        indicationColor = reason == QCamera::LockFailed ? Qt::red : Qt::black;
        ui->lockButton->setText(tr("Focus"));
        if (reason == QCamera::LockFailed)
            ui->statusbar->showMessage(tr("Focus Failed"), 2000);
    }

    QPalette palette = ui->lockButton->palette();
    palette.setColor(QPalette::ButtonText, indicationColor);
    ui->lockButton->setPalette(palette);
}

void Camera::displayCaptureError(int id, const QCameraImageCapture::Error error, const QString &errorString)
{
    Q_UNUSED(id);
    Q_UNUSED(error);
    QMessageBox::warning(this, tr("Image Capture Error"), errorString);
    isCapturingImage = false;
}

void Camera::startCamera()
{
    camera->start();
}

void Camera::stopCamera()
{
    camera->stop();
}

void Camera::updateCaptureMode()
{
    QCamera::CaptureModes captureMode = QCamera::CaptureStillImage;

    if (camera->isCaptureModeSupported(captureMode))
        camera->setCaptureMode(captureMode);
}

void Camera::updateCameraState(QCamera::State state)
{
    switch (state) {
    case QCamera::ActiveState:
        ui->actionStartCamera->setEnabled(false);
        ui->actionStopCamera->setEnabled(true);
        ui->captureWidget->setEnabled(true);
        ui->actionSettings->setEnabled(true);
        break;
    case QCamera::UnloadedState:
    case QCamera::LoadedState:
        ui->actionStartCamera->setEnabled(true);
        ui->actionStopCamera->setEnabled(false);
        ui->captureWidget->setEnabled(false);
        ui->actionSettings->setEnabled(false);
    }
}

void Camera::setExposureCompensation(int index)
{
    camera->exposure()->setExposureCompensation(index*0.5);
}

void Camera::displayCameraError()
{
    QMessageBox::warning(this, tr("Camera error"), camera->errorString());
}

void Camera::updateCameraDevice(QAction *action)
{
    setCamera(qvariant_cast<QCameraInfo>(action->data()));
}

void Camera::displayViewfinder()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void Camera::displayCapturedImage()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void Camera::readyForCapture(bool ready)
{
    ui->takeImageButton_Rec->setEnabled(ready);
    ui->takeImageButton_Input->setEnabled(ready);
}

void Camera::imageSaved(int id, const QString &fileName)
{
    Q_UNUSED(id);
    Q_UNUSED(fileName);

    isCapturingImage = false;
    if (applicationExiting)
        close();
}

void Camera::closeEvent(QCloseEvent *event)
{
    if (isCapturingImage) {
        setEnabled(false);
        applicationExiting = true;
        event->ignore();
    } else {
        event->accept();
    }
}

void Camera::on_actionTraining_triggered()
{
    TrainingSetting training;
    training.exec();
}

void Camera::on_actionOpen_data_triggered()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Open Image"), ".", tr("Image Files(*.jpg *.png)"));
    if(path.length() == 0) {
        QMessageBox::information(NULL, tr("Path"), tr("You didn't select any files."));
    } else {
        QMessageBox::information(NULL, tr("Path"), tr("You selected \nThis function is under development.") + path);
    }
}

void Camera::on_takeImageButton_Input_clicked()
{
    // 第一次录入时记录用户ID和训练次数
    if(0 == inputTimes)
    {
        // 用户ID
        userId = ui->lineEdit->text();
        // 训练次数
        trainTimes = ui->spinBox->value();

        // 对用户ID进行判空
        if(userId.isEmpty())
        {
            QMessageBox::warning(NULL, "Warning", "Please input User ID !", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
            return;
        }
        // 判断训练次数的正确性
        if(trainTimes < 1 || trainTimes > MAXTRAINTIMES)
        {
            QMessageBox::warning(NULL, "Warning", "Please input a correct train times !", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
            return;
        }
    }

    isCapturingImage = true;

    // BPNN-Face-Recognition 路径
    QDir dir = programRootDir();
    // imagesTrain 路径
    dir.cd(IMAGESTRAIN);
    QString imagesTrain_path = dir.absolutePath();

    // data 路径
    QDir dir_2 = programRootDir();
    dir_2.cd(DATA);
    QString data_path = dir_2.absolutePath();
    // 录入图片的名字
    QString imgName_input("/" + userId + "_" + QString::number(inputTimes + 1) + "_1" + ".pgm");

    // 拍照，传入绝对路径
    imageCapture->capture(imagesTrain_path + imgName_input);

    // 训练集与测试集文件
    QFile file_train(data_path + TRAINNAME), file_testAll(data_path + TESTALLNAME);
    QString dir_imagesTrain(INPUTDIR);
    // 训练集与测试集中图片的相对路径
    QString dir_input(dir_imagesTrain + '/' + userId + "_" + QString::number(inputTimes + 1) + "_1" + ".pgm" + '\n');

    // 打开训练集与测试集
    if(file_train.open(QIODevice::WriteOnly|QIODevice::Append) && file_testAll.open(QIODevice::WriteOnly|QIODevice::Append))
    {
        // 写入训练集与测试集
        if(-1 == file_train.write(dir_input.toLatin1(), dir_input.length()) || (-1 == file_testAll.write(dir_input.toLatin1(), dir_input.length())))
        {
            QMessageBox::warning(NULL, "Warning", "Fail to write the file!", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
            file_train.close();
            file_testAll.close();
            return;
        }
    }
    else
    {
        QMessageBox::warning(NULL, "Warning", "Fail to open the file!", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        return;
    }
    file_train.close();
    file_testAll.close();

    // 录入次数增加1
    inputTimes++;
    // 重置训练次数提示标签
    ui->label_photoTimes->clear();
    ui->label_photoTimes->setText(QString::fromStdString("Take photo " + std::to_string(INPUTTIMES - inputTimes) + " times"));

    if(inputTimes >= INPUTTIMES)
    {
        // 录入次数置0
        inputTimes = 0;
        // 重置训练次数提示标签
        ui->label_photoTimes->clear();
        ui->label_photoTimes->setText(QString::fromStdString("Take photo " + std::to_string(INPUTTIMES) + " times"));

        QMessageBox::StandardButton questionButton = QMessageBox::question(NULL, "Start training?", "Has taken " + QString::number(INPUTTIMES) + " times! \nDo you start training?", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        if(questionButton == QMessageBox::Yes)
        {
            // 开始训练
            facetrain(trainTimes);
        }
    }
}

void Camera::on_takeImageButton_Rec_clicked()
{
    QString userId = ui->lineEdit_2->text();

    isCapturingImage = true;

    // BPNN-Face-Recognition 路径
    QDir dir = programRootDir();
    // imagesRec 路径
    dir.cd(IMAGESREC);
    QString imagesRec_path = dir.absolutePath();
    // 识别图片的名字
    QString imgName_rec(IMAGESNAMEREC);
    // 拍照，传入绝对路径
    imageCapture->capture(imagesRec_path + imgName_rec);

    char netname[256], recListName[256];
    netname[0] = recListName[0] = '\0';

    dir = programRootDir();
    dir.cd(DATA);
    QString data_path = dir.absolutePath();
    std::string str_data_path = data_path.toStdString();
    // 网络名
    strcpy(netname, (str_data_path + NETNAME).c_str());

    BPNN *net;
    if ((net = bpnn_read(netname)) == NULL)
    {
        QMessageBox *trainMessageBox = new QMessageBox();
        trainMessageBox->setText("开始训练？");
        trainMessageBox->setInformativeText("找不到网络，是否训练网络？");
        trainMessageBox->setStandardButtons(QMessageBox::Cancel | QMessageBox::Ok);
        trainMessageBox->setDefaultButton(QMessageBox::Ok);
        trainMessageBox->show();
        int ret = trainMessageBox->exec();
        switch (ret) {
          case QMessageBox::Cancel:
              return;
          case QMessageBox::Ok:
              // 开始训练
              facetrain(trainTimes);
          default:
              break;
        }
        trainMessageBox->close();
    }
    else
    {
        QMessageBox *trainMessageBox = new QMessageBox();
        trainMessageBox->setText("开始识别？");
        trainMessageBox->setInformativeText("已找到网络，是否开始识别？");
        trainMessageBox->setStandardButtons(QMessageBox::Cancel | QMessageBox::Ok);
        trainMessageBox->setDefaultButton(QMessageBox::Ok);
        trainMessageBox->show();
        int ret = trainMessageBox->exec();
        switch (ret) {
          case QMessageBox::Cancel:
              return;
          case QMessageBox::Ok:
            {
            //---------------- 开始识别 ---------------------

            IMAGELIST *imagelist_rec;
            imagelist_rec = imgl_alloc();

            // 测试集名
            strcpy(recListName, (str_data_path + RECLISTNAME).c_str());

            // 获取map_userId
            std::map<std::string, int> *map_userId = new std::map<std::string, int>;
            map_userId = imgl_get_map_userId(recListName, map_userId);

            if (recListName[0] != '\0')
                imgl_load_images_from_textfile(imagelist_rec, recListName);

            // 输出识别集中每张图片的匹配情况
            printf("Matching at the end of the iteration: \n\n");
            printf("Recognition set : \n\n");
            // 识别
            std::string s_userId_rec = result_on_imagelist(net, imagelist_rec, 0, map_userId);
            QString qs_userId_rec = QString::fromStdString(s_userId_rec);

            if(!userId.isEmpty())
            {
                // 判断识别对错
                if(qs_userId_rec == userId)
                {
                    QMessageBox::about(NULL, "Yes", "I'm <font color='red'>right</font>!\nHe is " + userId + " !\nI guess he is " + qs_userId_rec);
                }
                else if(!qs_userId_rec.isEmpty())
                {
                    QMessageBox::about(NULL, "No", "I'm <font color='red'>wrong</font>!\nHe is " + userId + " !\nBut I guess he is " + qs_userId_rec);
                }
                else
                {
                    QMessageBox::about(NULL, "No", "I'm <font color='red'>wrong</font>!\nHe is " + userId + " !\nBut I don't know who he is.");
                }
            }
            else
            {
                if(qs_userId_rec.isEmpty())
                {
                    QMessageBox::about(NULL, "No", "I don't know who he is.");
                }
                else
                {
                    QMessageBox::about(NULL, "OK", "I guess he is " + qs_userId_rec);
                }
            }
            }

          default:
              break;
        }
        trainMessageBox->close();

    }
}
