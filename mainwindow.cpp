#include <QFile>
#include <QDirIterator>
#include <QDir>
#include <QDebug>
#include <QIODevice>
#include <QtSql>
#include <QtAlgorithms>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "structs.h"

logTxtInfo processLog(QStringList log);
logTxtInfo processLog(QString log);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    readFiles();
    readSql();

    ui->comboBoxSerials->addItems(allSerials);

    serialCompleter = new QCompleter(allSerials, this);
    serialCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    ui->comboBoxSerials->setCompleter(serialCompleter);

    ui->comboBoxSerials->setCurrentIndex(0);
    onSerialChanged(ui->comboBoxSerials->currentText());

    connect(ui->comboBoxSerials, SIGNAL(currentIndexChanged(QString)),
            this, SLOT(onSerialChanged(QString)));
    connect(ui->comboBoxCamTestDate, SIGNAL(currentIndexChanged(QString)),
            this, SLOT(onCamTestDateChanged(QString)));
    connect(ui->comboBoxEncoderTestDate, SIGNAL(currentIndexChanged(QString)),
            this, SLOT(onEncoderTestDateChanged(QString)));

    qSort(allSerials.begin(), allSerials.end());

    output(); //txt output
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::readFiles()
{
    QDir dir("log");
    QDirIterator iterator(dir.absolutePath(), QStringList() << "*.txt", QDir::Files, QDirIterator::Subdirectories);
    while(iterator.hasNext()){
        QFile file(iterator.next());
        //qDebug() << file.fileName();
        if(file.open(QIODevice::ReadOnly | QIODevice::Text)){
            QStringList log;
            logTxtInfo processed;
            bool isFirst = true;
            while(!file.atEnd()){
                QString line = file.readLine().simplified();
                if(line.contains("cpu load", Qt::CaseInsensitive) &&
                        !isFirst){
                    processed = processLog(log);
                    listTxtLog << processed;
                    txtSerials << processed.serial_no;
                    addSerial(processed.serial_no);
                    log << line;
                    continue;
                }
                log << line;
                isFirst = false;
            }
            listTxtLog << processed;
            txtSerials << processed.serial_no;
            addSerial(processed.serial_no);
            file.close();
        }
        else{
            qDebug() << "Can't open file. : " << file.fileName();
        }
    }

    /*foreach(logTxtInfo info, logList){
        qDebug() << info.cpu_load
                 << info.freemem
                 << info.uptime
                 << info.firmwareVersion
                 << info.hardware_id
                 << info.serial_no
                 << info.fps_value
                 << info.resolutions
                 << info.mac_addr
                 << info.error_code
                 << info.info_test
                 << info.audio_card_record
                 << info.low_state
                 << info.high_state
                 << info.zoom_in
                 << info.zoom_out
                 << info.zoom_in_out << "\n";
    }*/
}
void MainWindow::readSql()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName("10.30.1.1");
    db.setDatabaseName("BilkonErp");
    db.setUserName("BilkonErpAdmin");
    db.setPassword("Aibotah8");
    db.setPort(5432);

    try {
        db.open();
        QSqlQuery query = db.exec("SELECT * FROM bilkon_cam_test_log");
        while(query.next()){
            logCamTest tmp;
            //
            // Beware of first 2 lines
            //
            tmp.cam_hid = query.record().value("cam_serial").toString();
            tmp.cam_serial = query.record().value("cam_hid").toString();
            tmp.cam_audio = query.record().value("cam_audio").toString();
            tmp.cam_io = query.record().value("cam_io").toString();
            tmp.cam_zoom = query.record().value("cam_zoom").toString();
            tmp.cam_fps = query.record().value("cam_fps").toString();
            tmp.cam_mac = query.record().value("cam_mac").toString();
            tmp.cam_version = query.record().value("cam_version").toString();
            tmp.cam_cpu = query.record().value("cam_cpu").toString();
            tmp.test_state = query.record().value("test_state").toString();
            tmp.test_date = query.record().value("test_date").toString();
            tmp.test_person = query.record().value("test_person").toString();
            listCamTestLog << tmp;
            camTestSerials << tmp.cam_serial;
            addSerial(tmp.cam_hid);
        }

        query = db.exec("SELECT * FROM encoder_test_logging");
        while(query.next()){
            logEncoderTest tmp;
            tmp.encoder_serial = query.record().value("encoder_serial").toString();
            tmp.opcode = query.record().value("opcode").toString();
            tmp.err_code = query.record().value("err_code").toString();
            tmp.date = query.record().value("date").toString();
            tmp.operator_ = query.record().value("operator").toString();
            tmp.terminal = query.record().value("terminal").toString();
            listEncoderTestLog << tmp;
            encoderTestSerials << tmp.encoder_serial;
            addSerial(tmp.encoder_serial);
        }
    } catch (std::exception &e) {
        qDebug() << "Error: " << e.what();
    }

    db.close();
}

void MainWindow::fillTxtTab(QString serial)
{
    for(int i = listTxtLog.length() - 1; i >= 0; i--){
        if(listTxtLog.at(i).serial_no.contains(serial)){
            logTxtInfo tmp = listTxtLog.at(i);
            ui->textBrowserTxtCpuLoad->setText(tmp.cpu_load);
            ui->textBrowserTxtFreemem->setText(tmp.freemem);
            ui->textBrowserTxtUptime->setText(tmp.uptime);
            ui->textBrowserTxtFirmwareVersion->setText(tmp.firmwareVersion);
            ui->textBrowserTxtHardwareId->setText(tmp.hardware_id);
            ui->textBrowserTxtSerialNo->setText(tmp.serial_no);
            ui->textBrowserTxtFpsValue->setText(tmp.fps_value);
            ui->textBrowserTxtResolutions->setText(tmp.resolutions);
            ui->textBrowserTxtMacAddr->setText(tmp.mac_addr);
            ui->textBrowserTxtErrorCode->setText(tmp.error_code);
            ui->textBrowserTxtInfoTest->setText(tmp.info_test);
            ui->textBrowserTxtAudioCardRecord->setText(tmp.audio_card_record);
            ui->textBrowserTxtLowState->setText(tmp.low_state);
            ui->textBrowserTxtHighState->setText(tmp.high_state);
            ui->textBrowserTxtZoomIn->setText(tmp.zoom_in);
            ui->textBrowserTxtZoomOut->setText(tmp.zoom_out);
            ui->textBrowserTxtZoomInOut->setText(tmp.zoom_in_out);
            break;
        }
    }
}

void MainWindow::fillCamTestTab(QString serial, QString date)
{
    for(int i = 0; i < listCamTestLog.length(); i++){
        if(listCamTestLog.at(i).cam_serial.contains(serial) &&
                listCamTestLog.at(i).test_date.contains(date)){
            logCamTest tmp = listCamTestLog.at(i);
            ui->textBrowserCamTestCamHid->setText(tmp.cam_hid);
            ui->textBrowserCamTestCamSerial->setText(tmp.cam_serial);
            ui->textBrowserCamTestCamAudio->setText(tmp.cam_audio);
            ui->textBrowserCamTestCamIo->setText(tmp.cam_io);
            ui->textBrowserCamTestCamZoom->setText(tmp.cam_zoom);
            ui->textBrowserCamTestCamFps->setText(tmp.cam_fps);
            ui->textBrowserCamTestCamMac->setText(tmp.cam_mac);
            ui->textBrowserCamTestCamVersion->setText(tmp.cam_version);
            ui->textBrowserCamTestCamCpu->setText(tmp.cam_cpu);
            ui->textBrowserCamTestTestState->setText(tmp.test_state);
            ui->textBrowserCamTestTestDate->setText(tmp.test_date);
            ui->textBrowserCamTestTestPerson->setText(tmp.test_person);
            break;
        }
    }
}

void MainWindow::fillEncoderTestTab(QString serial, QString date)
{
    for(int i=0; i < listEncoderTestLog.length(); i++){
        if(listEncoderTestLog.at(i).encoder_serial.contains(serial) &&
                listEncoderTestLog.at(i).date.contains(date)){
            logEncoderTest tmp = listEncoderTestLog.at(i);
            ui->textBrowserEncoderTestEncoderSerial->setText(tmp.encoder_serial);
            ui->textBrowserEncoderTestOpcode->setText(tmp.opcode);
            ui->textBrowserEncoderTestErrCode->setText(tmp.err_code);
            ui->textBrowserEncoderTestDate->setText(tmp.date);
            ui->textBrowserEncoderTestOperator->setText(tmp.operator_);
            ui->textBrowserEncoderTestTerminal->setText(tmp.terminal);
            break;
        }
    }
}

void MainWindow::clearTxtTab()
{
    ui->textBrowserTxtCpuLoad->setText("");
    ui->textBrowserTxtFreemem->setText("");
    ui->textBrowserTxtUptime->setText("");
    ui->textBrowserTxtFirmwareVersion->setText("");
    ui->textBrowserTxtHardwareId->setText("");
    ui->textBrowserTxtSerialNo->setText("");
    ui->textBrowserTxtFpsValue->setText("");
    ui->textBrowserTxtResolutions->setText("");
    ui->textBrowserTxtMacAddr->setText("");
    ui->textBrowserTxtErrorCode->setText("");
    ui->textBrowserTxtInfoTest->setText("");
    ui->textBrowserTxtAudioCardRecord->setText("");
    ui->textBrowserTxtLowState->setText("");
    ui->textBrowserTxtHighState->setText("");
    ui->textBrowserTxtZoomIn->setText("");
    ui->textBrowserTxtZoomOut->setText("");
    ui->textBrowserTxtZoomInOut->setText("");
}

void MainWindow::clearCamTestTab()
{
    ui->comboBoxCamTestDate->clear();
    ui->textBrowserCamTestCamHid->setText("");
    ui->textBrowserCamTestCamSerial->setText("");
    ui->textBrowserCamTestCamAudio->setText("");
    ui->textBrowserCamTestCamIo->setText("");
    ui->textBrowserCamTestCamZoom->setText("");
    ui->textBrowserCamTestCamFps->setText("");
    ui->textBrowserCamTestCamMac->setText("");
    ui->textBrowserCamTestCamVersion->setText("");
    ui->textBrowserCamTestCamCpu->setText("");
    ui->textBrowserCamTestTestState->setText("");
    ui->textBrowserCamTestTestDate->setText("");
    ui->textBrowserCamTestTestPerson->setText("");
}

void MainWindow::clearEncoderTestTab()
{
    ui->comboBoxEncoderTestDate->clear();
    ui->textBrowserEncoderTestEncoderSerial->setText("");
    ui->textBrowserEncoderTestOpcode->setText("");
    ui->textBrowserEncoderTestErrCode->setText("");
    ui->textBrowserEncoderTestDate->setText("");
    ui->textBrowserEncoderTestOperator->setText("");
    ui->textBrowserEncoderTestTerminal->setText("");
}

void MainWindow::addSerial(QString serial)
{
    if(allSerials.contains(serial)) return;
    if(serial.isEmpty() || serial.length() != 8) return;
    allSerials << serial;
}

void MainWindow::output()
{
    QFile file("output.txt");
    if(file.open(QFile::WriteOnly)){
        QTextStream wFileStream(&file);
        foreach(QString serial, allSerials){
            QString out = serial;
            out.append(",");
            if(txtSerials.contains(serial)) out.append("1,");
            else out.append("0,");
            if(camTestSerials.contains(serial)) out.append("1,");
            else out.append("0,");
            if(encoderTestSerials.contains(serial)) out.append("1\n");
            else out.append("0\n");
            //qDebug() << out;
            wFileStream << out;
        }
    }
    file.close();
}

void MainWindow::onSerialChanged(QString text)
{
    if(text.length() != 8) return;

    clearTxtTab();
    clearCamTestTab();
    clearEncoderTestTab();

    fillTxtTab(text);

    listCamDate.clear();
    foreach(logCamTest tmp, listCamTestLog){
        if(tmp.cam_serial.contains(text)){
            listCamDate << tmp.test_date;
        }
    }
    ui->comboBoxCamTestDate->addItems(listCamDate);
    onCamTestDateChanged(ui->comboBoxCamTestDate->currentText());

    listEncoderDate.clear();
    foreach(logEncoderTest tmp, listEncoderTestLog){
        if(tmp.encoder_serial.contains(text)){
            listEncoderDate << tmp.date;
        }
    }
    ui->comboBoxEncoderTestDate->addItems(listEncoderDate);
    onEncoderTestDateChanged(ui->comboBoxEncoderTestDate->currentText());
}

void MainWindow::onCamTestDateChanged(QString date)
{
    if(date.isEmpty()) return;
    QString serial = ui->comboBoxSerials->currentText();
    if(serial.length() != 8 || serial.isEmpty()) return;

    fillCamTestTab(serial, date);
}

void MainWindow::onEncoderTestDateChanged(QString date)
{
    if(date.isEmpty()) return;
    QString serial = ui->comboBoxSerials->currentText();
    if(serial.length() != 8 || serial.isEmpty()) return;

    fillEncoderTestTab(serial, date);
}

logTxtInfo processLog(QStringList log)
{
    logTxtInfo txtInfo;

    for(int i = 0; i < log.length(); i++){
        QString line = log.at(i);
        if(line.contains("cpu load", Qt::CaseInsensitive) &&
                log.length() > i + 1){
            txtInfo.cpu_load = log.at(i + 1);
        }
        else if(line.contains("freemem", Qt::CaseInsensitive) &&
                log.length() > i + 1){
            txtInfo.freemem = log.at(i + 1);
        }
        else if(line.contains("uptime", Qt::CaseInsensitive) &&
                log.length() > i + 1){
            txtInfo.uptime = log.at(i + 1);
        }
        else if(line.contains("firmwareVersion", Qt::CaseInsensitive) &&
                log.length() > i + 1){
            txtInfo.firmwareVersion = log.at(i + 1);
        }
        else if(line.contains("Hardware ID", Qt::CaseInsensitive) &&
                log.length() > i + 1){
            txtInfo.hardware_id = log.at(i + 1);
        }
        else if(line.contains("Serial No", Qt::CaseInsensitive) &&
                log.length() > i + 1){
            txtInfo.serial_no = log.at(i + 1);
        }
        else if(line.contains("fps value", Qt::CaseInsensitive) &&
                log.length() > i + 1){
            txtInfo.fps_value = log.at(i + 1);
        }
        else if(line.contains("resolutions", Qt::CaseInsensitive) &&
                log.length() > i + 1){
            txtInfo.resolutions = log.at(i + 1);
        }
        else if(line.contains("mac addr", Qt::CaseInsensitive) &&
                log.length() > i + 1){
            txtInfo.mac_addr = log.at(i + 1);
        }
        else if(
                line.contains("Q", Qt::CaseInsensitive) &&
                line.contains("I", Qt::CaseInsensitive) &&
                line.contains("A", Qt::CaseInsensitive) &&
                line.contains("Z", Qt::CaseInsensitive) &&
                line.length() == 2){
            txtInfo.error_code = line;
        }
        else if(line.contains("Info Test", Qt::CaseInsensitive)){
            txtInfo.info_test = line;
        }
        else if(line.contains("audio card Record", Qt::CaseInsensitive)){
            txtInfo.audio_card_record = line;
        }
        else if(line.contains("Low state", Qt::CaseInsensitive) &&
                log.length() > i + 1){
            txtInfo.low_state = log.at(i + 1);
        }
        else if(line.contains("high state", Qt::CaseInsensitive) &&
                log.length() > i + 1){
            txtInfo.high_state = log.at(i + 1);
        }
        else if(line == ("zoom in") &&
                log.length() > i + 3){
            txtInfo.zoom_in = log.at(i + 3);
        }
        else if(line.contains("zoom_out", Qt::CaseInsensitive) &&
                log.length() > i + 3){
            txtInfo.zoom_out = log.at(i + 3);
        }
        else if(line.contains("Zoom In & Out", Qt::CaseInsensitive)){
            txtInfo.zoom_in_out = line;
        }
    }

    return txtInfo;
}
logTxtInfo processLog(QString log)
{
    return processLog(log.split("\n"));
}

