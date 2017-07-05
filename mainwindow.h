#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>
#include <QCompleter>
#include "structs.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QList<logTxtInfo> listTxtLog;
    QList<logCamTest> listCamTestLog;
    QList<logEncoderTest> listEncoderTestLog;
    QStringList listEncoderDate;
    QStringList listCamDate;
    QStringList allSerials;
    QCompleter* serialCompleter;

    void readFiles();
    void readSql();
    void fillData(QString serial);
    void fillTxtTab(QString serial);
    void fillCamTestTab(QString serial, QString date);
    void fillEncoderTestTab(QString serial, QString date);
    void clearTxtTab();
    void clearCamTestTab();
    void clearEncoderTestTab();
    void addSerial(QString serial);


private slots:
    void onSerialChanged(QString);
    void onCamTestDateChanged(QString);
    void onEncoderTestDateChanged(QString);
};

#endif // MAINWINDOW_H
