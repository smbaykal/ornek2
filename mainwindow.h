#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>
#include <QCompleter>
#include <QLabel>
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

signals:
	void dataReceived();

private:
	Ui::MainWindow *ui;
    QCompleter* serialCompleter;
	QLabel* labelInfo;

	void init();
    void fillTxtTab(QString serial);
	void fillCamTestTab(QString serial);
	void fillEncoderTestTab(QString serial);
    void clearTxtTab();
    void clearCamTestTab();
    void clearEncoderTestTab();


private slots:
	void onDataReceived();
	void onOutputClicked();
	void onOutputAllClicked();
	void onLogDirectoryChangeClicked();
	void onOpenSerialListClicked();
	void onSerialChanged(QString);
};

#endif // MAINWINDOW_H
