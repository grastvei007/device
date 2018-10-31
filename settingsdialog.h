#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QtSerialPort/QSerialPort>
#include "serialportsettings.h"

QT_USE_NAMESPACE

namespace Ui {
class SettingsDialog;
}

class QIntValidator;


class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog();

    const SerialPortSettings& getSettings() const;

private slots:
    void showPortInfo(int idx);
    void apply();
    void checkCustomBaudRatePolicy(int idx);
    void checkCustomDevicePathPolicy(int idx);

private:
    void fillPortsParameters();
    void fillPortsInfo();
    void updateSettings();

private:
    Ui::SettingsDialog *ui;
    QIntValidator *intValidator;
    SerialPortSettings mSerialPortSettings;
};

#endif // SETTINGSDIALOG_H
