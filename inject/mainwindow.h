#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWindow>
#include <Windows.h>
#include <TlHelp32.h>
#include <QMessageBox>
#include <QtDebug>
#include <QFile>
#include <QFileDialog>
#include <QDir>
#include <QTextCodec>
#include <QByteArray>
#include <QLibrary>
#include <QProcess>



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QString dllName;
    DWORD PrcoID;

private slots:
    void on_newButton_clicked();

    void on_browserFileToolButton_clicked();

    void itemTextSolt();
    void on_injectButton_clicked();


    void on_uninjectButton_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
