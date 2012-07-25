#include "confdlg.h"

confdlg::confdlg(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("PoLis - Schnittstellenkonfiguration");

    QGroupBox *groupBox_x = new QGroupBox(tr("X-Achse"),this);
    QLabel *label_xPort = new QLabel(tr("Schnittstelle"),groupBox_x);
    comboBox_xPort = new QComboBox(groupBox_x);
    QLabel *label_xBaudRate = new QLabel(tr("Baudrate"),groupBox_x);
    comboBox_xBaudRate = new QComboBox(groupBox_x);
    comboBox_xBaudRate->addItems(baudrates);

    QHBoxLayout *layout_x = new QHBoxLayout(groupBox_x);
    layout_x->addWidget(label_xPort);
    layout_x->addWidget(comboBox_xPort);
    layout_x->addWidget(label_xBaudRate);
    layout_x->addWidget(comboBox_xBaudRate);

    QGroupBox *groupBox_y = new QGroupBox(tr("Y-Achse"),this);
    QLabel *label_yPort = new QLabel(tr("Schnittstelle"),groupBox_y);
    comboBox_yPort = new QComboBox(groupBox_y);
    QLabel *label_yBaudRate = new QLabel(tr("Baudrate"),groupBox_y);
    comboBox_yBaudRate = new QComboBox(groupBox_y);
    comboBox_yBaudRate->addItems(baudrates);

    QHBoxLayout *layout_y = new QHBoxLayout(groupBox_y);
    layout_y->addWidget(label_yPort);
    layout_y->addWidget(comboBox_yPort);
    layout_y->addWidget(label_yBaudRate);
    layout_y->addWidget(comboBox_yBaudRate);

    QGroupBox *groupBox_mode = new QGroupBox(tr("Schnittstellen-Modus"),this);
    radioButton_rs232 = new QRadioButton(tr("RS-232"),groupBox_mode);
    radioButton_rs485 = new QRadioButton(tr("RS-485"),groupBox_mode);
    radioButton_rs485->setChecked(true);

    QHBoxLayout *layout_mode = new QHBoxLayout(groupBox_mode);
    layout_mode->addWidget(radioButton_rs232);
    layout_mode->addWidget(radioButton_rs485);

    QPushButton *button_scanPorts = new QPushButton(tr("Schnittstellen aktualisieren"),this);
    button_connect = new QPushButton(tr("Verbinden"),this);
    button_connect->setEnabled(false);

    QHBoxLayout *layout_buttons = new QHBoxLayout;
    layout_buttons->addWidget(button_scanPorts);
    layout_buttons->addWidget(button_connect);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(groupBox_x);
    layout->addWidget(groupBox_y);
    layout->addWidget(groupBox_mode);
    layout->addLayout(layout_buttons);

    collectSerialPorts();
    loadSettings();
    updatePortConfigData();

    connect(button_scanPorts,SIGNAL(clicked()),SLOT(collectSerialPorts()));
    connect(comboBox_xPort,SIGNAL(currentIndexChanged(int)),SLOT(updatePortConfigData()));
    connect(comboBox_xBaudRate,SIGNAL(currentIndexChanged(int)),SLOT(updatePortConfigData()));
    connect(comboBox_yPort,SIGNAL(currentIndexChanged(int)),SLOT(updatePortConfigData()));
    connect(comboBox_yBaudRate,SIGNAL(currentIndexChanged(int)),SLOT(updatePortConfigData()));
    connect(radioButton_rs232,SIGNAL(toggled(bool)),SLOT(updatePortConfigData()));
    connect(button_connect,SIGNAL(clicked()),SLOT(accept()));
}

void confdlg::updatePortConfigData()
{
    p_pc.xPort = comboBox_xPort->currentText();
    p_pc.yPort = comboBox_yPort->currentText();
    p_pc.xBaudRate = baudRateToParameter(comboBox_xBaudRate->currentIndex());
    p_pc.yBaudRate = baudRateToParameter(comboBox_yBaudRate->currentIndex());
    p_pc.mode = radioButton_rs232->isChecked() ? rs232 : rs485;
    button_connect->setEnabled( !p_pc.xPort.isEmpty() && !p_pc.yPort.isEmpty() && p_pc.xPort != p_pc.yPort );
}

portConfiguration confdlg::getPortConfiguration()
{
    saveSettings(); //User seems to continue, so save his settings here
    return p_pc;
}

void confdlg::collectSerialPorts()
{
    comboBox_xPort->clear();
    comboBox_yPort->clear();
    QDir dirDev("/dev");
    QStringList dirFilters;
    dirFilters << "tty?*";
    dirDev.setNameFilters(dirFilters);
    dirDev.setFilter(QDir::Readable | QDir::Files | QDir::Drives | QDir::System);
    comboBox_xPort->addItems(dirDev.entryList());
    comboBox_yPort->addItems(dirDev.entryList());
}

unsigned int confdlg::baudRateToParameter(int index)
{
    if( index < 0 )
        return 0;
    index++; //index = 1 -> B50 = 1
    if( index > 17 )
        index += 9984; //index = 18 -> B57600 = 10001
    if( index > 10017)
        return 0; //B4000000 = 10017, more than that is invalid
    return index;
}

void confdlg::saveSettings()
{
    QSettings settings;
    settings.setValue("ports/xport",p_pc.xPort);
    settings.setValue("ports/yport",p_pc.yPort);
    settings.setValue("ports/xbaudrateindex",comboBox_xBaudRate->currentIndex());
    settings.setValue("ports/ybaudrateindex",comboBox_yBaudRate->currentIndex());
    settings.setValue("ports/mode",(int)p_pc.mode);
}

void confdlg::loadSettings()
{
    QSettings settings;
    comboBox_xPort->setCurrentIndex(comboBox_xPort->findText(settings.value("ports/xport").toString()));
    comboBox_yPort->setCurrentIndex(comboBox_yPort->findText(settings.value("ports/yport").toString()));
    comboBox_xBaudRate->setCurrentIndex(settings.value("ports/xbaudrateindex",12).toInt());
    comboBox_yBaudRate->setCurrentIndex(settings.value("ports/ybaudrateindex",12).toInt());
    portmode pm = (portmode)settings.value("ports/mode").toInt();
    radioButton_rs232->setChecked(pm == rs232);
    radioButton_rs485->setChecked(pm == rs485);
}
