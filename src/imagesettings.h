#ifndef IMAGESETTINGS_H
#define IMAGESETTINGS_H

#include <QDialog>
#include <QAudioEncoderSettings>
#include <QVideoEncoderSettings>

QT_BEGIN_NAMESPACE
class QComboBox;
class QCameraImageCapture;
namespace Ui { class ImageSettingsUi; }
QT_END_NAMESPACE

class ImageSettings : public QDialog
{
    Q_OBJECT

public:
    ImageSettings(QCameraImageCapture *imageCapture, QWidget *parent = 0);
    ~ImageSettings();

    QAudioEncoderSettings audioSettings() const;
    void setAudioSettings(const QAudioEncoderSettings &settings);

    QImageEncoderSettings imageSettings() const;
    void setImageSettings(const QImageEncoderSettings &settings);

    QString format() const;
    void setFormat(const QString &format);

protected:
    void changeEvent(QEvent *e);

private:
    QVariant boxValue(const QComboBox *box) const;
    void selectComboBoxItem(QComboBox *box, const QVariant &value);

    Ui::ImageSettingsUi *ui;
    QCameraImageCapture *imagecapture;
};

#endif // IMAGESETTINGS_H
