#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPushButton>
#include <QAction>
#include <QFileDialog>
#include <QBrush>
#include "tools.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    pIniFile=new QSettings("Config.ini",QSettings::IniFormat);
    ui->setupUi(this);
    lblmsg=new QLabel("info:");
    ui->statusBar->addWidget(lblmsg,1);//添加在左边并全部填充
    ui->txtLog->clear();
    ui->txtSourcePath->setText(pIniFile->value("Path/source","").toString());
    ui->txtTargetPath->setText(pIniFile->value("Path/Target","").toString());
    connect(ui->btnSourcePath,&QPushButton::clicked,this,&MainWindow::OnSelectPath);
    connect(ui->btnTargetPath,&QPushButton::clicked,this,&MainWindow::OnSelectPath);
    connect(ui->actionRun,&QAction::triggered,this,&MainWindow::OnExecuteChange);
    connect(&rcXml,&RcToXml::HandleFileChanged,this,&MainWindow::ReadRcFileChanged);
    this->showMaximized();
}

MainWindow::~MainWindow()
{
    if(pIniFile)
    {
        delete pIniFile;
        pIniFile=nullptr;
    }
    if(lblmsg)
    {
        delete lblmsg;
        lblmsg=nullptr;
    }
    delete ui;
}

QString MainWindow::strToHtml(QString &str)
{
    //注意这几行代码的顺序不能乱，否则会造成多次替换
    str.replace("&","&amp;");
    str.replace(">","&gt;");
    str.replace("<","&lt;");
    str.replace("\"","&quot;");
    str.replace("\'","&#39;");
    str.replace(" ","&nbsp;");
    str.replace("\n","<br>");
    str.replace("\r","<br>");
    return str;
}

void MainWindow::SearchFile(const QString &path)
{
    QDir dir(path);
    if(!dir.exists()) return;
    dir.setFilter(QDir::Dirs|QDir::NoDotAndDotDot|QDir::NoSymLinks);
    QFileInfoList list=dir.entryInfoList();
    for(int i=0,size=list.size();i<size;i++)
    {
        SearchFile(list.at(i).filePath());
    }
    list=dir.entryInfoList(filters,QDir::Files);
    for(int i=0,size=list.size();i<size;i++)
    {
        auto ext=list.at(i).suffix();
        rcXml.SetFile(pathCombine({list.at(i).path(),list.at(i).fileName()}));
        if(ext.compare("rc",Qt::CaseInsensitive)==0)
        {
            rcXml.ReadRcObjects();
        }
        else
        {
            rcXml.ReadDefines();
        }
    }
}

void MainWindow::OnSelectPath()
{
    QPushButton *btn=qobject_cast<QPushButton*>(sender());
    if(btn->objectName().compare("btnSourcePath",Qt::CaseInsensitive)==0)
    {
        ui->txtSourcePath->setText(QFileDialog::getExistingDirectory(this,"选择源码位置","./",QFileDialog::ShowDirsOnly));
    }
    else if(btn->objectName().compare("btnTargetPath",Qt::CaseInsensitive)==0)
    {
        ui->txtTargetPath->setText(QFileDialog::getExistingDirectory(this,"选择目标位置","./",QFileDialog::ShowDirsOnly));
    }
}

void MainWindow::OnExecuteChange()
{
    sourcePath=ui->txtSourcePath->text().trimmed();
    if(sourcePath.isEmpty())
    {
        msgboxWarn("请选择源码位置！");
        ui->txtSourcePath->setFocus();
        return;
    }
    else if(!pathExists(sourcePath))
    {
        msgboxWarn("你选择的源码位置不存在！");
        ui->txtSourcePath->setFocus();
        ui->txtSourcePath->selectAll();
        return;
    }
    targetPath=ui->txtTargetPath->text().trimmed();
    if(targetPath.isEmpty())
    {
        msgboxWarn("请选择目标位置！");
        ui->txtTargetPath->setFocus();
        return;
    }
    else
    {
        if(!pathExists(targetPath))
        {
            bool iscreate=QDir().mkdir(targetPath);
            if(!iscreate)
            {
                msgboxWarn("该目标位置不存在，但也不能创建，请重新选择可写入的目标位置！");
                ui->txtTargetPath->setFocus();
                ui->txtTargetPath->selectAll();
                return;
            }
        }
    }
    pIniFile->setValue("Path/source",sourcePath);
    pIniFile->setValue("Path/Target",targetPath);

    filters.clear();
    filters<<"*.h"<<"*.hpp"<<"*.rc";
    rcXml.SetTargetPath(targetPath);
    ui->txtLog->clear();
    if(!rcXml.CreateDefines())
    {
        msgboxErr("创建get_define.h失败！");
        return;
    }
    SearchFile(sourcePath);
    rcXml.CloseDefines();
    msgboxInfo("提取信息完毕！");
}

void MainWindow::ReadRcFileChanged(const QString &objName, const QString &msg, const bool success)
{
    ui->txtLog->insertHtml(QString("<span>%1:</span>").arg(strToHtml(const_cast<QString&>(objName))));
    if(success)
    {
        ui->txtLog->insertHtml(QString("&nbsp;<span style='color:blue;'>%1</span><br>").arg(strToHtml(const_cast<QString&>(msg))));
    }
    else{
        ui->txtLog->insertHtml(QString("&nbsp;<span style='color:red;font-style:italic;'>%1</span><br>").arg(strToHtml(const_cast<QString&>(msg))));
    }
}
