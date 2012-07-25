#ifndef CONFDLG_H
#define CONFDLG_H

#include <QtGui>

static const QStringList baudrates = QStringList() << "50" << "75" << "110" << "134" << "150" << "200" << "300" << "600" << "1200" << "1800" << "2400"
                                                   << "4800" << "9600" << "19.2k" << "38.4k" << "57.6k" << "115.2k" << "230.4k" << "460.8k" << "500k"
                                                   << "576k" << "921.6k" << "1000k" << "1152k" << "1500k" << "2000k" << "2500k" << "3000k"
                                                   << "3500k" << "4000k";

enum portmode { rs232, rs485 };

struct portConfiguration {
    QString xPort, yPort;
    int xBaudRate, yBaudRate;
    portmode mode;
};

class confdlg : public QDialog
{
    Q_OBJECT
public:
    explicit confdlg(QWidget *parent = 0);
    portConfiguration getPortConfiguration();

private:
    QComboBox *comboBox_xPort;
    QComboBox *comboBox_xBaudRate;
    QComboBox *comboBox_yPort;
    QComboBox *comboBox_yBaudRate;
    QRadioButton *radioButton_rs232;
    QRadioButton *radioButton_rs485;
    QPushButton *button_connect;

    portConfiguration p_pc;

    unsigned int baudRateToParameter(int index);
    void saveSettings();
    void loadSettings();

private slots:
    void collectSerialPorts();
    void updatePortConfigData();

signals:
    
public slots:
    
};

#endif // CONFDLG_H
