#include "tools.h"


void msgboxWarn(const QString &text, const QString &title,QWidget *parent)
{
    QMessageBox::warning(parent,title,text);
}

void msgboxInfo(const QString &text, const QString &title, QWidget *parent)
{
    QMessageBox::information(parent,title,text);
}

void msgboxErr(const QString &text, const QString &title, QWidget *parent)
{
    QMessageBox::critical(parent,title,text);
}

int msgboxQst(const QString &text, const QString &title, QWidget *parent,QMessageBox::StandardButtons buttons,QMessageBox::StandardButton defaultButton)
{
    return QMessageBox::question(parent,title,text,buttons,defaultButton);
}

const QString pathCombine(std::initializer_list<QString> lst) noexcept
{
    QString result="";
    for(auto &a:lst)
    {
        if(a.isEmpty()) continue;
        if(!a.startsWith(QDir::separator()))
        {
            if(!result.isEmpty() && !result.endsWith(QDir::separator()))
            {
                result.append(QDir::separator());
            }
            result.append(a);
        }else{
            result.append(a.mid(1));
        }
    }
    return result;
}

bool fileExists(const QString &fileName)
{
    QFileInfo file(fileName);
    if(!file.exists()) return false;
    return file.isFile();
}

bool pathExists(const QString &fileName)
{
    QFileInfo file(fileName);
    if(!file.exists()) return false;
    return file.isDir();
}

const std::string detectEncode(const char *buf, const size_t len)
{
    uchardet_t ud=uchardet_new();
    //如果样本字符不够，有可能分析失败
    if(uchardet_handle_data(ud, buf, len)!=0)
    {
        uchardet_delete(ud);
        return std::string("");
    }
    uchardet_data_end(ud);
    const char* cs = uchardet_get_charset(ud);
    uchardet_delete(ud);
    return std::string(cs);
}

const std::string detectEncode(const QString &fileName)
{
    if(!fileExists(fileName)) return std::string("");
    FILE *file;
    file=_wfopen(fileName.toStdWString().c_str(),L"rt");
    if(!file) return std::string("");
    char data[4096];
    auto fsize=fread(data,sizeof(char),4096,file);
    fclose(file);
    return detectEncode(data,fsize);
}

bool deleteFile(const QString &fileName)
{
    QFileInfo fInfo(fileName);
    if(!fInfo.exists()) return true;
    if(fInfo.isDir()) return false;
    return QFile::remove(fileName);
}

bool createDir(const QString &path)
{
    if(path.isEmpty()) return  false;
    QDir dir(path);
    if(dir.exists()) return true;
    if(dir.mkdir(path)) return true;
    int i=0,size=path.size();
    auto c=path.at(i).toLatin1();
    if(c=='/' || c=='\\') i++;
    c=path.at(i).toLatin1();
    if(c=='/' || c=='\\') i++;
    for(;i<size;i++)
    {
        c=path.at(i).toLatin1();
        if(c=='/' || c=='\\'){
            if(!dir.mkdir(path.mid(0,i))) return false;
        }
    }
    dir.setCurrent(path);
    if(dir.exists()) return true;
    return dir.mkdir(path);
}
