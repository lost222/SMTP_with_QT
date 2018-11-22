#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <Winsock2.h>
#include <QMainWindow>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include "listenthread.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    bool loadFile(const QString &fileName);
    bool decode_File(const QString &fileName);
    ListenThread listenthread;

private:
    Ui::MainWindow *ui;
    int out2line;

private slots:
    void changeString(const QString &);
    void on_mail_saved(const QString &);

};

#endif // MAINWINDOW_H
