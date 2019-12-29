#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include "rctoxml.h"
#include <QSettings>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    RcToXml rcXml;
    QString sourcePath,targetPath;
    QStringList filters;
    QLabel *lblmsg=nullptr;
    Ui::MainWindow *ui;
    QSettings *pIniFile=nullptr;
    QString strToHtml(QString &str);
    //
    void SearchFile(const QString &path);
    //
    void OnSelectPath();
    //
    void OnExecuteChange();
    //
    void ReadRcFileChanged(const QString &objName,const QString &msg,const bool success);
};

#endif // MAINWINDOW_H
