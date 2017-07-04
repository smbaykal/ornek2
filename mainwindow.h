#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>

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
    QStringList allSerials;
    QStringList serialsFromTxt;
    QStringList serialsFromCamTest;
    QStringList serialsFromEncoder;
    int serialCount;

    void readFiles();
    void readSql();


private slots:
    void refreshInfoForm();
    void buttonClick();
};

#endif // MAINWINDOW_H
