#include <QFile>
#include <QDirIterator>
#include <QDir>
#include <QDebug>
#include <QIODevice>
#include <QFileDialog>
#include <QtSql>
#include <QtAlgorithms>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "structs.h"
#include "dbhelper.h"

QString m_dbDriver("QPSQL");
QString m_dbHostName("10.30.1.1");
QString m_dbDatabaseName("BilkonErp");
QString m_dbUserName("BilkonErpAdmin");
QString m_dbPassword("Aibotah8");
int m_dbPort(5432);

QDir m_logDir("log");
QList<logTxtInfo> m_listTxtLog;
QList<logCamTest> m_listCamTestLog;
QList<logEncoderTest> m_listEncoderTestLog;
QStringList m_allSerials;
QStringList m_txtSerials;
QStringList m_camTestSerials;
QStringList m_encoderTestSerials;
QStringList m_cam_audio_errors;
QStringList m_cam_io_errors;
QStringList m_cam_zoom_errors;
QStringList m_encoder_errors;

logTxtInfo processLog(QStringList log);
logTxtInfo processLog(QString log);
QStringList getSerialListFromFile(QString fileName);
void readFiles();
void readSql();
void sortSerials();
void fillData(QString serial);
void outputLog(QString serial, QString dir = "output");
void outputLogs(QString dir = "output");
void outputLogs(QStringList serials, QString dir);
void outputTxtLog(logTxtInfo info, QString dir);
void outputCamLog(logCamTest info, QString dir);
void outputEncLog(logEncoderTest info, QString dir);
bool addSerial(QString serial);
bool addSerial(QString serial, QStringList* list);
void output();
void fillErrorTypes();



MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	init();

	fillErrorTypes();
	readFiles();
	readSql();
	sortSerials();

	emit dataReceived();

	//output(); //txt output*/
}

MainWindow::~MainWindow()
{
	serialCompleter->deleteLater();
	delete ui;
}

void MainWindow::init()
{
	labelInfo = new QLabel(this);
	ui->statusBar->addWidget(labelInfo);
	connect(this, SIGNAL(dataReceived()),
			this, SLOT(onDataReceived()));
	connect(ui->pushButtonOutput, SIGNAL(clicked(bool)),
			this, SLOT(onOutputClicked()));
	connect(ui->pushButtonOutputAll, SIGNAL(clicked(bool)),
			this, SLOT(onOutputAllClicked()));
	connect(ui->pushButtonSetLogDirectory, SIGNAL(clicked(bool)),
			this, SLOT(onLogDirectoryChangeClicked()));
	connect(ui->pushButtonOpenSerialListFile, SIGNAL(clicked(bool)),
			this, SLOT(onOpenSerialListClicked()));
	connect(ui->comboBoxSerials, SIGNAL(currentIndexChanged(QString)),
			this, SLOT(onSerialChanged(QString)));
}
void MainWindow::fillTxtTab(QString serial)
{
	for(int i = m_listTxtLog.length() - 1; i >= 0; i--){
		if(m_listTxtLog.at(i).serial_no.contains(serial)){
			logTxtInfo tmp = m_listTxtLog.at(i);
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
void MainWindow::fillCamTestTab(QString serial)
{
	for(int i = 0; i < m_listCamTestLog.length(); i++){
		if(m_listCamTestLog.at(i).cam_serial.contains(serial) /*&&
				!audio_errors.contains(listCamTestLog.at(i).cam_audio) &&
				!io_errors.contains(listCamTestLog.at(i).cam_io) &&
				!zoom_errors.contains(listCamTestLog.at(i).cam_zoom)*/){
			logCamTest tmp = m_listCamTestLog.at(i);
			ui->textBrowserCamTestCamHid->append(tmp.cam_hid);
			ui->textBrowserCamTestCamSerial->append(tmp.cam_serial);
			ui->textBrowserCamTestCamAudio->append(tmp.cam_audio);
			ui->textBrowserCamTestCamIo->append(tmp.cam_io);
			ui->textBrowserCamTestCamZoom->append(tmp.cam_zoom);
			ui->textBrowserCamTestCamFps->append(tmp.cam_fps);
			ui->textBrowserCamTestCamMac->append(tmp.cam_mac);
			ui->textBrowserCamTestCamVersion->append(tmp.cam_version);
			ui->textBrowserCamTestCamCpu->append(tmp.cam_cpu);
			ui->textBrowserCamTestTestState->append(tmp.test_state);
			ui->textBrowserCamTestTestDate->append(tmp.test_date);
			ui->textBrowserCamTestTestPerson->append(tmp.test_person);
			//break;
		}
	}
}
void MainWindow::fillEncoderTestTab(QString serial)
{

	for(int i=0; i < m_listEncoderTestLog.length(); i++){
		if(m_listEncoderTestLog.at(i).encoder_serial.contains(serial) /*&&
				encoder_errors.contains(listEncoderTestLog.at(i).err_code)*/){
			logEncoderTest tmp = m_listEncoderTestLog.at(i);
			ui->textBrowserEncoderTestEncoderSerial->append(tmp.encoder_serial);
			ui->textBrowserEncoderTestOpcode->append(tmp.opcode);
			ui->textBrowserEncoderTestErrCode->append(tmp.err_code);
			ui->textBrowserEncoderTestDate->append(tmp.date);
			ui->textBrowserEncoderTestOperator->append(tmp.operator_);
			ui->textBrowserEncoderTestTerminal->append(tmp.terminal);
			//break;
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
	ui->textBrowserEncoderTestEncoderSerial->setText("");
	ui->textBrowserEncoderTestOpcode->setText("");
	ui->textBrowserEncoderTestErrCode->setText("");
	ui->textBrowserEncoderTestDate->setText("");
	ui->textBrowserEncoderTestOperator->setText("");
	ui->textBrowserEncoderTestTerminal->setText("");
}
void MainWindow::onDataReceived()
{
	ui->comboBoxSerials->clear();
	ui->comboBoxSerials->addItems(m_allSerials);

	serialCompleter = new QCompleter(m_allSerials, this);
	serialCompleter->setCaseSensitivity(Qt::CaseInsensitive);
	ui->comboBoxSerials->setCompleter(serialCompleter);

	ui->comboBoxSerials->setCurrentIndex(0);
	onSerialChanged(ui->comboBoxSerials->currentText());

	QString info = QString("All serials: %1; Txt serials: %2; Cam serials: %3; Enc serials: %4")
			.arg(QString::number(m_allSerials.length()),
				 QString::number(m_txtSerials.length()),
				 QString::number(m_camTestSerials.length()),
				 QString::number(m_encoderTestSerials.length()));
	labelInfo->setText(info);
}
void MainWindow::onOutputClicked()
{
	QString dir = QFileDialog::getExistingDirectory(this,
													tr("Select dir to save"),
													QString());
	outputLog(MainWindow::ui->comboBoxSerials->currentText(), dir);
}
void MainWindow::onOutputAllClicked()
{
	QString dir = QFileDialog::getExistingDirectory(this,
													tr("Select directory to save"),
													"",
													QFileDialog::ShowDirsOnly |
													QFileDialog::DontResolveSymlinks);
	outputLogs(dir);
}
void MainWindow::onLogDirectoryChangeClicked()
{
	m_logDir = QFileDialog::getExistingDirectory(this,
													tr("Open directory"),
													"",
													QFileDialog::ShowDirsOnly |
													QFileDialog::DontResolveSymlinks);
	foreach(QString serial, m_txtSerials){
		m_allSerials.removeOne(serial);
	}
	m_txtSerials.clear();
	m_listTxtLog.clear();
	readFiles();
	sortSerials();
	emit MainWindow::dataReceived();
}
void MainWindow::onOpenSerialListClicked()
{
	QString fileName = QFileDialog::getOpenFileName(this,
												tr("Select file"),
												QString(),
												"Text files (*.txt)");
	QString dir = QFileDialog::getExistingDirectory(this,
													tr("Select dir to save"),
													QString());

	QStringList serials = getSerialListFromFile(fileName);
	outputLogs(serials, dir);

}
void MainWindow::onSerialChanged(QString serial)
{
	if(serial.length() != 8) return;

	clearTxtTab();
	fillTxtTab(serial);

	clearCamTestTab();
	fillCamTestTab(serial);

	clearEncoderTestTab();
	fillEncoderTestTab(serial);
}

void output()
{
	QFile fileO("output.txt");
	if(fileO.open(QFile::WriteOnly)){
		QTextStream wFileStream(&fileO);
		foreach(QString serial, m_allSerials){
			QString out = serial;
			out.append(",");
			if(m_txtSerials.contains(serial)) out.append("1,");
			else out.append("0,");
			if(m_camTestSerials.contains(serial)) out.append("1,");
			else out.append("0,");
			if(m_encoderTestSerials.contains(serial)) out.append("1\n");
			else out.append("0\n");
			//qDebug() << out;
			wFileStream << out;
		}
	}
	fileO.close();

	QFile fileT("outputTxtLog.txt");
	if(fileT.open(QFile::WriteOnly)){
		QTextStream wFileStream(&fileT);
		foreach(QString serial, m_txtSerials){
			QString out = serial;
			out.append("\n");
			wFileStream << out;
		}
	}
	fileT.close();

	QFile fileC("outputCamTestLog.txt");
	if(fileC.open(QFile::WriteOnly)){
		QStringList writtenSerials;
		QTextStream wFileStream(&fileC);
		foreach(logCamTest info, m_listCamTestLog){
			if(!m_cam_audio_errors.contains(info.cam_audio) &&
					!m_cam_io_errors.contains(info.cam_io) &&
					!m_cam_zoom_errors.contains(info.cam_zoom) &&
					!writtenSerials.contains(info.cam_serial) &&
					m_camTestSerials.contains(info.cam_serial)){
				QString out = info.cam_serial.simplified();
				writtenSerials << out;
				//out.append("\n");
				//wFileStream << out;
			}
		}
		qSort(writtenSerials.begin(), writtenSerials.end());
		foreach(QString serial, writtenSerials){
			wFileStream << serial << "\n";
		}
	}
	fileC.close();

	QFile fileE("outputEncoderTestLog.txt");
	if(fileE.open(QFile::WriteOnly)){
		QStringList writtenSerials;
		QTextStream wFileStream(&fileE);
		foreach(logEncoderTest info, m_listEncoderTestLog){
			if(!m_encoder_errors.contains(info.err_code) &&
					!writtenSerials.contains(info.encoder_serial) &&
					m_encoderTestSerials.contains(info.encoder_serial)) {
				QString out = info.encoder_serial;
				writtenSerials << out;
				//out.append("\n");
				//wFileStream << out;
			}
		}
		qSort(writtenSerials.begin(), writtenSerials.end());
		foreach(QString serial, writtenSerials){
			wFileStream << serial << "\n";
		}
	}
	fileE.close();

	qDebug() << "All serials: " << m_allSerials.length()
			 << "\nTxt log: " << m_txtSerials.length()
			 << "\nCam test log: " << m_camTestSerials.length()
			 << "\nEncoder test log: " << m_encoderTestSerials.length();
}
void readFiles()
{
	QStringList nameFilter("*.txt");
	QStringList txtFiles = m_logDir.entryList(nameFilter);
	foreach(QString fileName, txtFiles){
		QFile file(m_logDir.absolutePath() + "/" + fileName);
		if(file.open(QIODevice::ReadOnly | QIODevice::Text)){
			QStringList log;
			logTxtInfo processed;
			bool isFirst = true;
			while(!file.atEnd()){
				QString line = file.readLine().simplified();
				if(line.contains("cpu load", Qt::CaseInsensitive) &&
						!isFirst){
					processed = processLog(log);
					m_listTxtLog << processed;
					addSerial(processed.serial_no, &m_txtSerials);
					addSerial(processed.serial_no);
					log << line;
					continue;
				}
				log << line;
				isFirst = false;
			}
			processed = processLog(log);
			m_listTxtLog << processed;
			addSerial(processed.serial_no, &m_txtSerials);
			addSerial(processed.serial_no);
			file.close();
		}
		else{
			qDebug()	<<	"Can't open file. : "	<<	file.fileName()
						<<	"\n"	<<	file.errorString();
		}
	}
}
void readSql()
{
	DbHelper db(m_dbDriver,
				m_dbHostName,
				m_dbDatabaseName,
				m_dbUserName,
				m_dbPassword,
				m_dbPort);
	QSqlQuery query = db.getData("bilkon_cam_test_log");
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
		m_listCamTestLog << tmp;
		addSerial(tmp.cam_serial, &m_camTestSerials);
		addSerial(tmp.cam_serial);
	}

	query=db.getData("encoder_test_logging");

	while(query.next()){
		logEncoderTest tmp;
		tmp.encoder_serial = query.record().value("encoder_serial").toString();
		tmp.opcode = query.record().value("opcode").toString();
		tmp.err_code = query.record().value("err_code").toString();
		tmp.date = query.record().value("date").toString();
		tmp.operator_ = query.record().value("operator").toString();
		tmp.terminal = query.record().value("terminal").toString();
		m_listEncoderTestLog << tmp;
		addSerial(tmp.encoder_serial, &m_encoderTestSerials);
		addSerial(tmp.encoder_serial);
	}
}
void sortSerials()
{
	qSort(m_allSerials.begin(), m_allSerials.end());
	qSort(m_txtSerials.begin(), m_txtSerials.end());
	qSort(m_camTestSerials.begin(), m_camTestSerials.end());
	qSort(m_encoderTestSerials.begin(), m_encoderTestSerials.end());
}
void outputLog(QString serial, QString dir)
{
	foreach(logTxtInfo info, m_listTxtLog){
		if (!info.serial_no.contains(serial)) continue;
			outputTxtLog(info, dir);
	}

	foreach(logCamTest info, m_listCamTestLog){
		if(!info.cam_serial.contains(serial)) continue;
			outputCamLog(info, dir);
	}

	foreach(logEncoderTest info, m_listEncoderTestLog){
		if(!info.encoder_serial.contains(serial)) continue;
		outputEncLog(info, dir);
	}
}
void outputLogs(QString dir)
{
	QFile file;
	QTextStream wFileStream(&file);

	foreach(logTxtInfo info, m_listTxtLog){
		if(!m_txtSerials.contains(info.serial_no)){
			//qDebug() << info.serial_no;
			continue;
		}
		outputTxtLog(info, dir);
	}

	qDebug() << "Txt log saved.";

	foreach(logCamTest info, m_listCamTestLog){
		if(!m_camTestSerials.contains(info.cam_serial)){
			//qDebug() << info.cam_serial;
			continue;
		}
		outputCamLog(info, dir);
	}

	qDebug() << "Cam log saved.";

	foreach(logEncoderTest info, m_listEncoderTestLog){
		if(!m_encoderTestSerials.contains(info.encoder_serial)){
			//qDebug() << info.encoder_serial;
			continue;
		}
		outputEncLog(info, dir);
	}

	qDebug() << "Encoder log saved.";

	//file.close();
}
void outputLogs(QStringList serials, QString dir)
{
	bool isInAllSerials = false;
	bool isInTxtSerials = false;
	bool isInCamSerials = false;
	bool isInEncSerials = false;
	foreach(QString serial, serials){
		if(m_allSerials.contains(serial)) isInAllSerials = true;
		if(m_txtSerials.contains(serial)) isInTxtSerials = true;
		if(m_camTestSerials.contains(serial)) isInCamSerials = true;
		if(m_encoderTestSerials.contains(serial)) isInEncSerials = true;
	}

	if(!isInAllSerials) return;

	if(isInTxtSerials){
		foreach(logTxtInfo info, m_listTxtLog){
			if(!serials.contains(info.serial_no)){
				//qDebug() << info.serial_no;
				continue;
			}
			outputTxtLog(info, dir);
		}
		qDebug() << "Txt log saved.";
	}

	if(isInCamSerials){
		foreach(logCamTest info, m_listCamTestLog){
			if(!serials.contains(info.cam_serial)){
				//qDebug() << info.cam_serial;
				continue;
			}
			outputCamLog(info, dir);
		}
		qDebug() << "Cam log saved.";
	}

	if(isInEncSerials){
		foreach(logEncoderTest info, m_listEncoderTestLog){
			if(!serials.contains(info.encoder_serial)){
				//qDebug() << info.encoder_serial;
				continue;
			}
			outputEncLog(info, dir);
		}
		qDebug() << "Encoder log saved.";
	}

	//file.close();
}

void outputTxtLog(logTxtInfo info, QString dir)
{
	QFile file;
	QTextStream wFileStream(&file);
	file.setFileName(dir + "/" + info.serial_no + ".txt");
	if(!file.open(QIODevice::WriteOnly | QIODevice::Append)){
		qDebug() << "Can't open file. : "  << file.fileName();
		return;
	}
	wFileStream << ":::::::::::::::::TXT\n";
	QString o;
	o += "CPU Load\n";
	o += info.cpu_load + "\n";
	o += "Freemem\n";
	o += info.freemem + "\n";
	o += "Uptime\n";
	o += info.uptime + "\n";
	o += "Firmware Version\n";
	o += info.firmwareVersion + "\n";
	o += "Hardware ID\n";
	o += info.hardware_id + "\n";
	o += "Serial No\n";
	o += info.serial_no + "\n";
	o += "FPS Value\n";
	o += info.fps_value + "\n";
	o += "Resolutions\n";
	o += info.resolutions + "\n";
	o += "MAC Address\n";
	o += info.mac_addr + "\n";
	o += "Error Code\n";
	o += info.error_code + "\n";
	o += info.info_test + "\n";
	o += info.audio_card_record + "\n";
	o += "Low State\n";
	o += info.low_state + "\n";
	o += "High State\n";
	o += info.high_state + "\n";
	o += "Zoom In\n";
	o += info.zoom_in + "\n";
	o += "Zoom Out\n";
	o += info.zoom_out + "\n";
	o += info.zoom_in_out + "\n";
	o += "::::::::::::::::::::\n\n";
	wFileStream << o;
	file.close();
}

void outputCamLog(logCamTest info, QString dir)
{
	QFile file;
	QTextStream wFileStream(&file);
	file.setFileName(dir + "/" + info.cam_serial + ".txt");
	if(!file.open(QIODevice::WriteOnly | QIODevice::Append)){
		qDebug() << "Can't open file. : "  << file.fileName();
		return;
	}
	wFileStream << ":::::::::::::::::CAM\n";
	QString o;
	o += "Cam Hid\n";
	o += info.cam_hid + "\n";
	o += "Cam Serial\n";
	o += info.cam_serial + "\n";
	o += "Cam Audio\n";
	o += info.cam_audio + "\n";
	o += "Cam IO\n";
	o += info.cam_io + "\n";
	o += "Cam Zoom\n";
	o += info.cam_zoom + "\n";
	o += "Cam FPS\n";
	o += info.cam_fps + "\n";
	o += "Cam MAC\n";
	o += info.cam_mac + "\n";
	o += "Cam Version\n";
	o += info.cam_version + "\n";
	o += "Cam CPU\n";
	o += info.cam_cpu + "\n";
	o += "Test State\n";
	o += info.test_state + "\n";
	o += "Test Date\n";
	o += info.test_date + "\n";
	o += "Test Person\n";
	o += info.test_person + "\n";
	o += "::::::::::::::::::::\n\n";
	wFileStream << o;
	file.close();
}

void outputEncLog(logEncoderTest info, QString dir)
{
	QFile file;
	QTextStream wFileStream(&file);
	file.setFileName(dir + "/" + info.encoder_serial + ".txt");
	if(!file.open(QIODevice::WriteOnly | QIODevice::Append)){
		qDebug() << "Can't open file. : "  << file.fileName();
		return;
	}
	wFileStream << ":::::::::::::::::ENC\n";
	QString o;
	o += "Encoder Serial\n";
	o += info.encoder_serial + "\n";
	o += "Opcode\n";
	o += info.opcode + "\n";
	o += "Error Code\n";
	o += info.err_code + "\n";
	o += "Date\n";
	o += info.date + "\n";
	o += "Operator\n";
	o += info.operator_ + "\n";
	o += "Terminal\n";
	o += info.terminal + "\n";
	o += "::::::::::::::::::::\n\n";
	wFileStream << o;
	file.close();
}

void fillErrorTypes()
{
	m_cam_audio_errors << "";
	m_cam_audio_errors << "A0";
	m_cam_audio_errors << "A1";
	m_cam_audio_errors << "A2";
	m_cam_io_errors << "";
	m_cam_io_errors << "IO1";
	m_cam_zoom_errors << "Z0";
	m_encoder_errors << "";
	m_encoder_errors << "-1";
}

bool addSerial(QString serial)
{
	return addSerial(serial, &m_allSerials);
}
bool addSerial(QString serial, QStringList* list)
{
	if(list->contains(serial)) return false;
	if(serial.isEmpty() || serial.length() != 8) return false;
	*list << serial;
	return true;
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
		else if(line.contains("serial", Qt::CaseInsensitive) &&
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
				line.contains("A0", Qt::CaseInsensitive) &&
				line.contains("A1", Qt::CaseInsensitive) &&
				line.contains("A2", Qt::CaseInsensitive) &&
				line.contains("Z0", Qt::CaseInsensitive) &&
				line.contains("IO1", Qt::CaseInsensitive)){
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
QStringList getSerialListFromFile(QString fileName)
{
	QFile f(fileName);
	if(!f.open(QIODevice::ReadOnly | QIODevice::Text))
		return QStringList ();

	QString data = f.readAll().data();

	QStringList flds;
	if(data.contains(","))
	flds = data.split(",");
	if(data.contains(":"))
	flds = data.split(":");
	if(data.contains("\n"))
	flds = data.split("\n");
	return flds;
}
