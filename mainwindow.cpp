#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QDirIterator>
#include <QDir>
#include <QDebug>
#include <QIODevice>
#include <QtSql>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    serialCount = 0;

    connect(ui->pushButton, SIGNAL(clicked(bool)),
            this, SLOT(buttonClick()));

    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(refreshInfoForm()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::readFiles()
{
    QStringList serialNos;

    QDir dir("log");
    QDirIterator iterator(dir.absolutePath(), QStringList() << "*.txt", QDir::Files, QDirIterator::Subdirectories);
    while(iterator.hasNext()){
        //qDebug() << iterator.next();
        QFile file(iterator.next());
        if(file.open(QIODevice::ReadOnly | QIODevice::Text)){
            while(!file.atEnd()){
                QString line = file.readLine();

                int result = line.indexOf("serial", 0, Qt::CaseInsensitive);
                if(result == 0){
                    //qDebug() << result;
                    // simplified removes escape character at the end
                    QString serial = file.readLine().simplified();
                    if(serial.isEmpty() || serial.length() != 8) continue;
                    //qDebug() << serial;
                    serialsFromTxt << serial;
                    if(!allSerials.contains(serial)){
                        allSerials << serial;
                        serialCount++;
                    }
                    break;  // there are 1 to many serial numbers in file,
                    // we don't need duplicates
                }
            }
            //qDebug() << file.fileName();
            file.close();
        }
        else{
            qDebug() << "Can't open file. : " << file.fileName();
        }
    }
}

void MainWindow::readSql()
{
    //QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
    //qDebug() << "Has a valid QPSQL driver: " << (db.isValid() ? "YES" : "NO");

    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName("10.30.1.1");
    db.setDatabaseName("BilkonErp");
    db.setUserName("BilkonErpAdmin");
    db.setPassword("Aibotah8");
    db.setPort(5432);

    try {
        qDebug() << db.open();

        // This is for checking columns
        //QString queryString = "select column_name,data_type "
        //    "from information_schema.columns "
        //    "where table_name = 'bilkon_cam_test_log'";
        // column name: cam_hid, type: text

        //QString queryString = "select column_name,data_type "
        //                      "from information_schema.columns "
        //                      "where table_name = 'encoder_test_logging'";
        // column name: encoder_serial, type: text

        /*QSqlQuery query = db.exec(queryString);

        while(query.next()){
            qDebug() << query.value(0) << " : " << query.value(1);
        }*/

        QSqlQuery query = db.exec("SELECT DISTINCT cam_hid FROM bilkon_cam_test_log");
        while(query.next()){
            //qDebug() << query.record().value(0).toString();
            QString serial = query.record().value(0).toString();
            serialsFromCamTest << serial;
            if(!allSerials.contains(serial)){
                allSerials << serial;
                serialCount++;
            }
        }

        query = db.exec("SELECT DISTINCT encoder_serial FROM encoder_test_logging");
        while(query.next()){
            //qDebug() << query.record().value(0).toString();
            QString serial = query.record().value(0).toString();
            serialsFromEncoder << serial;
            if(!allSerials.contains(serial)){
                allSerials << serial;
                serialCount++;
            }
        }

    } catch (std::exception &e) {
        qDebug() << "Error: " << e.what();
    }

    db.close();



}

void MainWindow::refreshInfoForm()
{
    QString serial = ui->comboBox->currentText();

    ui->textBrowserSerialNo->setText(serial);

    qDebug() << serial;

    ui->textBrowserUsedIn->setText("");

    if(serialsFromTxt.contains(serial)){
        ui->textBrowserUsedIn->setText("Text\r\n");
    }
    if(serialsFromCamTest.contains(serial)){
        ui->textBrowserUsedIn->setText(ui->textBrowserUsedIn->toPlainText() + "Cam\r\n");
    }
    if(serialsFromEncoder.contains(serial)){
        ui->textBrowserUsedIn->setText(ui->textBrowserUsedIn->toPlainText() + "Encoder\r\n");
    }
}

void MainWindow::buttonClick()
{
    readFiles();
    readSql();
    /*foreach(QString serial, allSerials){
        qDebug() << serial;
    }*/

    ui->comboBox->addItems(allSerials);

    qDebug() << serialCount;
}
