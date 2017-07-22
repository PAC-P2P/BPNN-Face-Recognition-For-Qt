#ifndef CAMERA_H
#define CAMERA_H

#include <QCamera>
#include <QCameraImageCapture>
#include <QMediaRecorder>

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class Camera; }
QT_END_NAMESPACE

class Camera : public QMainWindow
{
    Q_OBJECT

public:
    Camera(QWidget *parent = 0);
    ~Camera();

private slots:
    void setCamera(const QCameraInfo &cameraInfo);

    void startCamera();
    void stopCamera();

    void toggleLock();
//    void takeImage_rec();
//    void takeImage_input();
    void displayCaptureError(int, QCameraImageCapture::Error, const QString &errorString);

    void configureCaptureSettings();
    void configureImageSettings();

    void displayCameraError();

    void updateCameraDevice(QAction *action);

    void updateCameraState(QCamera::State);
    void updateCaptureMode();
    void setExposureCompensation(int index);

    void processCapturedImage(int requestId, const QImage &img);
    void updateLockStatus(QCamera::LockStatus, QCamera::LockChangeReason);

    void displayViewfinder();
    void displayCapturedImage();

    void readyForCapture(bool ready);
    void imageSaved(int id, const QString &fileName);

    void on_actionTraining_triggered();

    void on_actionOpen_data_triggered();

    void on_takeImageButton_Input_clicked();

    void on_takeImageButton_Rec_clicked();

protected:
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void closeEvent(QCloseEvent *event);

private:
    Ui::Camera *ui;

    QCamera *camera;
    QCameraImageCapture *imageCapture;

    QImageEncoderSettings imageSettings;
    bool isCapturingImage;
    bool applicationExiting;
    int inputTimes;
    int trainTimes;
    QString userId;
};

#endif
