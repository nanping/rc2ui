#ifndef TOOLS_H
#define TOOLS_H

#include <QString>
#include <initializer_list>
#include <QFileInfo>
#include <QDir>
#include <QMessageBox>
#include "uchardet/uchardet.h"

//提示窗口封装
void msgboxWarn(const QString &text,const QString &title="警告",QWidget *parent=nullptr);
void msgboxInfo(const QString &text,const QString &title="提示",QWidget *parent=nullptr);
void msgboxErr(const QString &text,const QString &title="错误",QWidget *parent=nullptr);
int  msgboxQst(const QString &text,const QString &title="询问",QWidget *parent=nullptr,QMessageBox::StandardButtons buttons = QMessageBox::StandardButtons(QMessageBox::Yes | QMessageBox::No),QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);

//合并路径
const QString pathCombine(std::initializer_list<QString> lst) noexcept;
//判断文件是否存在
bool fileExists(const QString &fileName);
//判断路劲是否存在
bool pathExists(const QString &fileName);
//比较字符串是否相等
bool compare(const std::string &str1,const std::string &str2,const bool ignoreCase=true);
//探测字符编码
const std::string detectEncode(const QString &fileName);
const std::string detectEncode(const char *buf, const size_t len);
//创建文件夹
bool createDir(const QString &path);
//删除文件
bool deleteFile(const QString &fileName);
#endif // TOOLS_H
