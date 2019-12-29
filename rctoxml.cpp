#include "rctoxml.h"
#include "tools.h"

RcToXml::RcToXml()
{

}

RcToXml::RcToXml(const QString &fileName)
{
    SetFile(fileName);
}

RcToXml::~RcToXml()
{
    Close();
    CloseDefines();
}

void RcToXml::SetFile(const QString &fileName)
{
    rcfileName=fileName;
}

void RcToXml::SetTargetPath(const QString &path)
{
    targetPath=path;
}

void RcToXml::Close()
{
    if(pDoc)
    {
        delete pDoc;
        pDoc=nullptr;
    }
    if(pStream)
    {
        delete pStream;
        pStream=nullptr;
    }
    if(pRcFile)
    {
        pRcFile->close();
        delete pRcFile;
        pRcFile=nullptr;
    }
    while(stack.size()>0)
    {
        auto d=stack.at(0);
        stack.removeAt(0);
        delete d;
    }
    stack.clear();
}

bool RcToXml::ReadRcObjects()
{
    if(!fileExists(rcfileName)) return false;
    Close();
    pRcFile=new QFile(rcfileName);
    if(!pRcFile->open(QIODevice::ReadOnly|QIODevice::Text)) return false;
    std::string encode=detectEncode(rcfileName);
    pStream=new QTextStream(pRcFile);
    if(!encode.empty()) pStream->setCodec(encode.c_str());

    QString line,nextLine;
    while(!pStream->atEnd())
    {
        ReadLine(line);
        if(line.isEmpty()) continue;
        nextLine.clear();
        if(line.endsWith(" MENU",Qt::CaseInsensitive))
        {
            ReadLine(nextLine);
            if(nextLine.compare("BEGIN",Qt::CaseInsensitive)==0)
            {
                stack.clear();
                ReadMenus(line.mid(0,line.indexOf(" MENU",0,Qt::CaseInsensitive)).trimmed());
                continue;
            }
        }
        if(line.indexOf(" DIALOGEX ",0,Qt::CaseInsensitive)>0)
        {
            ReadLine(nextLine);
            if(nextLine.startsWith("STYLE ",Qt::CaseInsensitive) ||
                    nextLine.startsWith("EXSTYLE ",Qt::CaseInsensitive) || nextLine.startsWith("FONT ",Qt::CaseInsensitive) ||
                    nextLine.startsWith("CAPTION ",Qt::CaseInsensitive))
            {
                stack.clear();
                ReadDialog(line,nextLine);
                continue;
            }
        }
    }
    Close();
    return true;
}

bool RcToXml::ReadDefines()
{
    if(!fileExists(rcfileName)) return false;
    Close();

    pRcFile=new QFile(rcfileName);
    if(!pRcFile->open(QIODevice::ReadOnly|QIODevice::Text)) return false;
    std::string encode=detectEncode(rcfileName);
    pStream=new QTextStream(pRcFile);
    if(!encode.empty()) pStream->setCodec(encode.c_str());
    QString line,v;
    int index;
    QFileInfo fInfo(rcfileName);
    if(fInfo.baseName().compare("BabyGrid",Qt::CaseInsensitive)==0 || fInfo.baseName().compare("Bitmap",Qt::CaseInsensitive)==0)
    {
        Close();
        return false;
    }
    QString tmp;
    QString key=QString("%1.%2").arg(fInfo.baseName()).arg(fInfo.suffix());
    QString value=QString("#include \"%1\"").arg(rcfileName);
    while(!pStream->atEnd())
    {
        ReadLine(line);
        if(!line.startsWith("#")) continue;
        index=line.indexOf("define ",1,Qt::CaseInsensitive);
        if(index<1) continue;
        line=line.mid(index+7).trimmed();
        index=line.indexOf(" ");
        if(index<1) continue;
        v=line.mid(index+1).trimmed();
        if(v.isEmpty()) continue;
        //        if(v.startsWith("'")) continue;
        //        if(v.startsWith("\"")) continue;
        if(v.startsWith("/")) continue;
        if(v=="__stdcall") continue;
        line=line.mid(0,index).trimmed();
        if(line.indexOf('(')>=0) continue;
        if(line.startsWith("generic_",Qt::CaseInsensitive)) continue;
        if(line.startsWith("JSON_",Qt::CaseInsensitive)) continue;
        if(line.startsWith("VK_",Qt::CaseInsensitive)) continue;
        if(line.startsWith("TIXMLA_",Qt::CaseInsensitive)) continue;
        if(line.startsWith("TIXML_",Qt::CaseInsensitive)) continue;
        if(line.startsWith("ID_PLUGINS_REMOVING",Qt::CaseInsensitive)) continue;
        if(line.compare("MainFileManager",Qt::CaseInsensitive)==0) continue;
        tmp=QString("%1:%2").arg(key).arg(line);
        if(AllDefines.find(tmp)==AllDefines.end()) AllDefines.insert(tmp,QString("output_define(%1);").arg(line));
        if(headers.find(key)==headers.end()) headers.insert(key,value);
    }

    Close();
    return true;
}

bool RcToXml::CreateDefines()
{
    CloseDefines();
    QString fileName=pathCombine({targetPath,"get_define.h"});
    pOut=new QFile(fileName);
    if(!pOut->open(QIODevice::WriteOnly|QIODevice::Truncate|QIODevice::Text)) return false;
    pOutStream=new QTextStream(pOut);
    pOutStream->setCodec("UTF-8");

    headers.clear();
    defines.clear();
    AllDefines.clear();
    return true;
}

void RcToXml::CloseDefines()
{
    if(pOutStream)
    {
        *pOutStream<<"#ifndef GET_DEFINE_H"<<endl;
        *pOutStream<<"#define GET_DEFINE_H"<<endl<<endl;
        *pOutStream<<"#include <windows.h>"<<endl;
        *pOutStream<<"#include <ostream>"<<endl;
        *pOutStream<<"#include <fstream>"<<endl<<endl;
        for(auto it=headers.begin();it!=headers.end();it++)
        {
            *pOutStream<<it.value()<<endl;
        }
        *pOutStream<<endl;
        *pOutStream<<"using namespace std;"<<endl<<endl;
        *pOutStream<<"#define output_define(ID) outfile<<#ID<<\"=\"<<(ID)<<endl"<<endl<<endl;

        //process_out1
        *pOutStream<<"void process_out1()"<<endl;
        *pOutStream<<"{"<<endl;
        *pOutStream<<"std::ofstream outfile;"<<endl;
        *pOutStream<<"outfile.open(\"Keys.ini\");"<<endl;
        auto keys=defines.keys();
        keys.sort(Qt::CaseInsensitive);
        QString k,tmp="";
        int index;
        for(int i=0,size=keys.size();i<size;i++)
        {
            index=keys[i].indexOf(':');
            k=keys[i].mid(0,index);
            if(k.compare(tmp,Qt::CaseInsensitive)!=0)
            {
                tmp=k;
                *pOutStream<<"outfile<<endl<<\"["<<k<<"]\"<<endl;"<<endl;
            }
            *pOutStream<<defines[keys[i]]<<endl;
        }
        *pOutStream<<endl<<"outfile.close();"<<endl;
        *pOutStream<<"}"<<endl<<endl;

        //process_out2
        *pOutStream<<"void process_out2()"<<endl;
        *pOutStream<<"{"<<endl;
        *pOutStream<<"std::ofstream outfile;"<<endl;
        *pOutStream<<"outfile.open(\"AllKeys.ini\");"<<endl;
        keys=AllDefines.keys();
        keys.sort(Qt::CaseInsensitive);
        tmp="";
        for(int i=0,size=keys.size();i<size;i++)
        {
            index=keys[i].indexOf(':');
            k=keys[i].mid(0,index);
            if(k.compare(tmp,Qt::CaseInsensitive)!=0)
            {
                tmp=k;
                *pOutStream<<"outfile<<endl<<\"["<<k<<"]\"<<endl;"<<endl;
            }
            *pOutStream<<AllDefines[keys[i]]<<endl;
        }
        *pOutStream<<endl<<"outfile.close();"<<endl;
        *pOutStream<<"}"<<endl<<endl;

        //process_out
        *pOutStream<<"void process_out()"<<endl;
        *pOutStream<<"{"<<endl;
        *pOutStream<<"process_out1();"<<endl;
        *pOutStream<<"process_out2();"<<endl;
        *pOutStream<<"}"<<endl<<endl;

        *pOutStream<<endl<<"#endif // GET_DEFINE_H"<<endl;
        emit HandleFileChanged("get_define.h",pathCombine({targetPath,"get_define.h"}),true);

        headers.clear();
        defines.clear();
        AllDefines.clear();
        delete pOutStream;
        pOutStream=nullptr;
    }
    if(pOut)
    {
        pOut->close();
        delete pOut;
        pOut=nullptr;
    }
}

bool RcToXml::ReadDialog(const QString &tline,const QString &tnextline)
{
    int index=tline.indexOf(" DIALOGEX ",0,Qt::CaseInsensitive);
    QString objName=tline.mid(0,index).trimmed();
    QString tmp=tline.mid(index+9).trimmed();
    auto lsize=tmp.split(',');
    QString caption,k;
    if(tnextline.startsWith("CAPTION ",Qt::CaseInsensitive))
    {
        caption=tnextline.mid(7).trimmed();
        if(caption.startsWith('"') && caption.endsWith('"')) caption=caption.mid(1,caption.length()-2).trimmed();
    }else{
        caption="";
    }
    if(pDoc) delete pDoc;
    pDoc=new xml_document();
    k=QString("%1:").arg(objName);
    if(defines.find(k)==defines.end()) defines.insert(k,QString("output_define(%1);").arg(objName));
    tmp=QString("<widget class=\"QDialog\" name=\"%1\"><property name=\"geometry\"><rect><x>%2</x><y>%3</y><width>%4</width><height>%5</height></rect></property><property name=\"windowTitle\"><string>%6</string></property></widget>")
            .arg(objName).arg(lsize[0].trimmed()).arg(lsize[1].trimmed()).arg(lsize[2].trimmed()).arg(lsize[3].trimmed())
            .arg(caption);
    auto rt=pDoc->load_string(tmp.toUtf8());
    if(!rt)
    {
        emit HandleFileChanged(objName,QString("对话框汇出xml错误:%1").arg(rt.description()),false);
        return false;
    }
    xml_node root=pDoc->child("widget");
    QString line,nextline;
    while(!pStream->atEnd())
    {
        ReadLine(line);
        if(line.isEmpty()) break;
        if(line.compare("BEGIN",Qt::CaseInsensitive)==0) continue;
        if(line.compare("END",Qt::CaseInsensitive)==0) break;
        if(line.startsWith("//",Qt::CaseInsensitive)) continue;
        if(line.startsWith("GROUPBOX ",Qt::CaseInsensitive))
        {
            tmp=line.mid(8).trimmed();
            lsize=tmp.split(',');
            if(lsize.size()<6)
            {
                ReadLine(nextline);
                tmp=QString("%1%2").arg(tmp).arg(nextline);
                lsize=tmp.split(',');
            }
            auto node=root.append_child("widget");
            node.append_attribute("class").set_value("QGroupBox");
            node.append_attribute("name").set_value(lsize[1].trimmed().toStdString().c_str());
            auto child=node.append_child("property");
            child.append_attribute("name").set_value("geometry");
            child=child.append_child("rect");
            child.append_child("x").text().set(lsize[2].trimmed().toStdString().c_str());
            child.append_child("y").text().set(lsize[3].trimmed().toStdString().c_str());
            child.append_child("width").text().set(lsize[4].trimmed().toStdString().c_str());
            child.append_child("height").text().set(lsize[5].trimmed().toStdString().c_str());
            child=node.append_child("property");
            child.append_attribute("name").set_value("title");
            caption=lsize[0].trimmed();
            if(caption.startsWith('"') && caption.endsWith('"')) caption=caption.mid(1,caption.length()-2).trimmed();
            child.append_child("string").text().set(lsize[0].trimmed().toStdString().c_str());

            k=QString("%1:%2").arg(objName).arg(lsize[1].trimmed());
            if(defines.find(k)==defines.end()) defines.insert(k,QString("output_define(%1);").arg(lsize[1].trimmed()));
            continue;
        }
        if(line.startsWith("CONTROL ",Qt::CaseInsensitive))
        {
            tmp=line.mid(7).trimmed();
            lsize=tmp.split(',');
            if(lsize.size()<8)
            {
                ReadLine(nextline);
                tmp=QString("%1%2").arg(tmp).arg(nextline);
                lsize=tmp.split(',');
            }
            auto node=root.append_child("widget");
            if(lsize[3].indexOf("BS_AUTORADIOBUTTON",0,Qt::CaseInsensitive)>=0)
            {
                node.append_attribute("class").set_value("QRadioButton");
            }
            else if(lsize[3].indexOf("BS_AUTOCHECKBOX",0,Qt::CaseInsensitive)>=0)
            {
                node.append_attribute("class").set_value("QCheckBox");
            }else{
                root.append_child(xml_node_type::node_comment).set_value(line.toStdString().c_str());
                continue;
            }
            node.append_attribute("name").set_value(lsize[1].trimmed().toStdString().c_str());
            auto child=node.append_child("property");
            child.append_attribute("name").set_value("geometry");
            child=child.append_child("rect");
            child.append_child("x").text().set(lsize[4].trimmed().toStdString().c_str());
            child.append_child("y").text().set(lsize[5].trimmed().toStdString().c_str());
            child.append_child("width").text().set(lsize[6].trimmed().toStdString().c_str());
            child.append_child("height").text().set(lsize[7].trimmed().toStdString().c_str());
            child=node.append_child("property");
            child.append_attribute("name").set_value("title");
            caption=lsize[0].trimmed();
            if(caption.startsWith('"') && caption.endsWith('"')) caption=caption.mid(1,caption.length()-2).trimmed();
            child.append_child("string").text().set(lsize[0].trimmed().toStdString().c_str());

            k=QString("%1:%2").arg(objName).arg(lsize[1].trimmed());
            if(defines.find(k)==defines.end()) defines.insert(k,QString("output_define(%1);").arg(lsize[1].trimmed()));
            continue;
        }
        if(line.startsWith("EDITTEXT ",Qt::CaseInsensitive))
        {
            tmp=line.mid(8).trimmed();
            lsize=tmp.split(',');
            if(lsize.size()<5)
            {
                ReadLine(nextline);
                tmp=QString("%1%2").arg(tmp).arg(nextline);
                lsize=tmp.split(',');
            }
            auto node=root.append_child("widget");
            node.append_attribute("class").set_value("QLineEdit");
            node.append_attribute("name").set_value(lsize[0].trimmed().toStdString().c_str());
            auto child=node.append_child("property");
            child.append_attribute("name").set_value("geometry");
            child=child.append_child("rect");
            child.append_child("x").text().set(lsize[1].trimmed().toStdString().c_str());
            child.append_child("y").text().set(lsize[2].trimmed().toStdString().c_str());
            child.append_child("width").text().set(lsize[3].trimmed().toStdString().c_str());
            child.append_child("height").text().set(lsize[4].trimmed().toStdString().c_str());

            k=QString("%1:%2").arg(objName).arg(lsize[0].trimmed());
            if(defines.find(k)==defines.end()) defines.insert(k,QString("output_define(%1);").arg(lsize[0].trimmed()));
            continue;
        }
        if(line.startsWith("LISTBOX ",Qt::CaseInsensitive))
        {
            tmp=line.mid(8).trimmed();
            lsize=tmp.split(',');
            if(lsize.size()<6)
            {
                ReadLine(nextline);
                tmp=QString("%1%2").arg(tmp).arg(nextline);
                lsize=tmp.split(',');
            }
            auto node=root.append_child("widget");
            node.append_attribute("class").set_value("QListView");
            node.append_attribute("name").set_value(lsize[0].trimmed().toStdString().c_str());
            auto child=node.append_child("property");
            child.append_attribute("name").set_value("geometry");
            child=child.append_child("rect");
            child.append_child("x").text().set(lsize[1].trimmed().toStdString().c_str());
            child.append_child("y").text().set(lsize[2].trimmed().toStdString().c_str());
            child.append_child("width").text().set(lsize[3].trimmed().toStdString().c_str());
            child.append_child("height").text().set(lsize[4].trimmed().toStdString().c_str());

            k=QString("%1:%2").arg(objName).arg(lsize[0].trimmed());
            if(defines.find(k)==defines.end()) defines.insert(k,QString("output_define(%1);").arg(lsize[0].trimmed()));
            continue;
        }
        if(line.startsWith("COMBOBOX ",Qt::CaseInsensitive))
        {
            tmp=line.mid(8).trimmed();
            lsize=tmp.split(',');
            if(lsize.size()<6)
            {
                ReadLine(nextline);
                tmp=QString("%1%2").arg(tmp).arg(nextline);
                lsize=tmp.split(',');
            }
            auto node=root.append_child("widget");
            node.append_attribute("class").set_value("QComboBox");
            node.append_attribute("name").set_value(lsize[0].trimmed().toStdString().c_str());
            auto child=node.append_child("property");
            child.append_attribute("name").set_value("geometry");
            child=child.append_child("rect");
            child.append_child("x").text().set(lsize[1].trimmed().toStdString().c_str());
            child.append_child("y").text().set(lsize[2].trimmed().toStdString().c_str());
            child.append_child("width").text().set(lsize[3].trimmed().toStdString().c_str());
            child.append_child("height").text().set(lsize[4].trimmed().toStdString().c_str());

            k=QString("%1:%2").arg(objName).arg(lsize[0].trimmed());
            if(defines.find(k)==defines.end()) defines.insert(k,QString("output_define(%1);").arg(lsize[0].trimmed()));
            continue;
        }
        if(line.startsWith("LTEXT ",Qt::CaseInsensitive) || line.startsWith("RTEXT ",Qt::CaseInsensitive) || line.startsWith("CTEXT ",Qt::CaseInsensitive))
        {
            tmp=line.mid(5).trimmed();
            lsize=tmp.split(',');
            if(lsize.size()<6)
            {
                ReadLine(nextline);
                tmp=QString("%1%2").arg(tmp).arg(nextline);
                lsize=tmp.split(',');
            }
            auto node=root.append_child("widget");
            node.append_attribute("class").set_value("QLabel");
            node.append_attribute("name").set_value(lsize[1].trimmed().toStdString().c_str());
            auto child=node.append_child("property");
            child.append_attribute("name").set_value("geometry");
            child=child.append_child("rect");
            child.append_child("x").text().set(lsize[2].trimmed().toStdString().c_str());
            child.append_child("y").text().set(lsize[3].trimmed().toStdString().c_str());
            child.append_child("width").text().set(lsize[4].trimmed().toStdString().c_str());
            child.append_child("height").text().set(lsize[5].trimmed().toStdString().c_str());
            child=node.append_child("property");
            child.append_attribute("name").set_value("title");
            caption=lsize[0].trimmed();
            if(caption.startsWith('"') && caption.endsWith('"')) caption=caption.mid(1,caption.length()-2).trimmed();
            child.append_child("string").text().set(lsize[0].trimmed().toStdString().c_str());

            k=QString("%1:%2").arg(objName).arg(lsize[1].trimmed());
            if(defines.find(k)==defines.end()) defines.insert(k,QString("output_define(%1);").arg(lsize[1].trimmed()));
            continue;
        }
        if(line.startsWith("PUSHBUTTON ",Qt::CaseInsensitive))
        {
            tmp=line.mid(10).trimmed();
            lsize=tmp.split(',');
            if(lsize.size()<6)
            {
                ReadLine(nextline);
                tmp=QString("%1%2").arg(tmp).arg(nextline);
                lsize=tmp.split(',');
            }
            auto node=root.append_child("widget");
            node.append_attribute("class").set_value("QPushButton");
            node.append_attribute("name").set_value(lsize[1].trimmed().toStdString().c_str());
            auto child=node.append_child("property");
            child.append_attribute("name").set_value("geometry");
            child=child.append_child("rect");
            child.append_child("x").text().set(lsize[2].trimmed().toStdString().c_str());
            child.append_child("y").text().set(lsize[3].trimmed().toStdString().c_str());
            child.append_child("width").text().set(lsize[4].trimmed().toStdString().c_str());
            child.append_child("height").text().set(lsize[5].trimmed().toStdString().c_str());
            child=node.append_child("property");
            child.append_attribute("name").set_value("title");
            caption=lsize[0].trimmed();
            if(caption.startsWith('"') && caption.endsWith('"')) caption=caption.mid(1,caption.length()-2).trimmed();
            child.append_child("string").text().set(lsize[0].trimmed().toStdString().c_str());

            k=QString("%1:%2").arg(objName).arg(lsize[1].trimmed());
            if(defines.find(k)==defines.end()) defines.insert(k,QString("output_define(%1);").arg(lsize[1].trimmed()));
            continue;
        }
        if(line.startsWith("DEFPUSHBUTTON ",Qt::CaseInsensitive))
        {
            tmp=line.mid(13).trimmed();
            lsize=tmp.split(',');
            if(lsize.size()<6)
            {
                ReadLine(nextline);
                tmp=QString("%1%2").arg(tmp).arg(nextline);
                lsize=tmp.split(',');
            }
            auto node=root.append_child("widget");
            node.append_attribute("class").set_value("QPushButton");
            node.append_attribute("name").set_value(lsize[1].trimmed().toStdString().c_str());
            auto child=node.append_child("property");
            child.append_attribute("name").set_value("geometry");
            child=child.append_child("rect");
            child.append_child("x").text().set(lsize[2].trimmed().toStdString().c_str());
            child.append_child("y").text().set(lsize[3].trimmed().toStdString().c_str());
            child.append_child("width").text().set(lsize[4].trimmed().toStdString().c_str());
            child.append_child("height").text().set(lsize[5].trimmed().toStdString().c_str());
            child=node.append_child("property");
            child.append_attribute("name").set_value("title");
            caption=lsize[0].trimmed();
            if(caption.startsWith('"') && caption.endsWith('"')) caption=caption.mid(1,caption.length()-2).trimmed();
            child.append_child("string").text().set(caption.toStdString().c_str());

            k=QString("%1:%2").arg(objName).arg(lsize[1].trimmed());
            if(defines.find(k)==defines.end()) defines.insert(k,QString("output_define(%1);").arg(lsize[1].trimmed()));
            continue;
        }
        if(line.startsWith("STYLE ",Qt::CaseInsensitive) || line.startsWith("EXSTYLE ",Qt::CaseInsensitive) || line.startsWith("FONT ",Qt::CaseInsensitive)) continue;
        if(line.startsWith("CAPTION ",Qt::CaseInsensitive))
        {
            caption=line.mid(7).trimmed();
            if(caption.startsWith('"') && caption.endsWith('"')) caption=caption.mid(1,caption.length()-2).trimmed();
            for(xml_node n:root.children("property"))
            {
                if(strcmp(n.attribute("name").value(),"windowTitle")==0)
                {
                    n.child("string").text().set(caption.toStdString().c_str());
                    break;
                }
            }
            continue;
        }
        root.append_child(xml_node_type::node_comment).set_value(line.toStdString().c_str());
    }
    tmp=pathCombine({targetPath,"dialog"});
    if(!pathExists(tmp)) createDir(tmp);
    tmp=pathCombine({tmp,QString("%1.xml").arg(objName)});
    if(fileExists(tmp)) deleteFile(tmp);
    pDoc->save_file(tmp.toUtf8(),PUGIXML_TEXT("\t"),pugi::format_default|pugi::format_no_declaration);
    emit HandleFileChanged(objName,tmp,true);
    return true;
}

void RcToXml::ReadLine(QString &line)
{
    line.clear();
    while(!pStream->atEnd())
    {
        line=pStream->readLine();
        RemoveSpeChar(line);
        if(!line.isEmpty()) break;
    }
}

void RcToXml::RemoveSpeChar(QString &line)
{
    line=line.replace('\t',' ');
    line=line.trimmed();
}

bool RcToXml::ReadMenus(const QString &name)
{
    QString line,nextLine,tmp,k;
    stack.push(new NodeData(name,NodeType::eSubMenu));
    while(!pStream->atEnd())
    {
        ReadLine(line);
        if(line.isEmpty()) break;
        if(line.startsWith("POPUP ",Qt::CaseInsensitive))
        {
            ReadLine(nextLine);
            if(nextLine.compare("BEGIN",Qt::CaseInsensitive)==0)
            {
                tmp=line.mid(line.indexOf("POPUP ",0,Qt::CaseInsensitive)+5).trimmed();
                if(tmp.startsWith('"') && tmp.endsWith('"')) tmp=tmp.mid(1,tmp.length()-2).trimmed();
                stack.push(new NodeData("",tmp,NodeType::eSubMenu));
            }
            else{
                emit HandleFileChanged(name,"菜单结构错误1.",false);
                return false;
            }
            continue;
        }
        if(line.startsWith("MENUITEM ",Qt::CaseInsensitive) && line.endsWith(" SEPARATOR",Qt::CaseInsensitive))
        {
            stack.push(new NodeData(NodeType::eMenuSeparator));
            continue;
        }
        if(line.startsWith("MENUITEM ",Qt::CaseInsensitive))
        {
            NodeData *p=new NodeData(NodeType::eMenuItem);
            tmp=line.mid(line.indexOf("MENUITEM ",0,Qt::CaseInsensitive)+8).trimmed();
            auto start=tmp.indexOf('"');
            auto end=tmp.indexOf('"',start+1);
            if(end>start+1) {
                p->Text=tmp.mid(start+1,end-start-1).trimmed();
                tmp=tmp.mid(end+1).trimmed();
            }
            start=tmp.indexOf(',');
            if(start>=0)
            {
                end=tmp.indexOf(',',start+1);
                if(end>start)
                {
                    tmp=tmp.mid(start+1,end-start-1).trimmed();
                }else{
                    tmp=tmp.mid(start+1).trimmed();
                }
            }
            if(!tmp.isEmpty()) {
                p->Name=tmp;
                k=QString("%1:%2").arg(name).arg(tmp);
                if(defines.find(k)==defines.end()) defines.insert(k,QString("output_define(%1);").arg(tmp));
            }
            stack.push(p);
            continue;
        }
        if(line.compare("END",Qt::CaseInsensitive)==0)
        {
            QVector<NodeData *> *pList=new QVector<NodeData *>();
            while(stack.size()>=1)
            {
                auto p=stack.pop();
                if(p->Type==NodeType::eMenuItem || p->Type==NodeType::eMenuSeparator)
                {
                    pList->insert(0,p);
                }
                else if(p->Type==NodeType::eSubMenu)
                {
                    p->pList=pList;
                    p->Type=NodeType::eMenuItem;
                    stack.push(p);
                    break;
                }
            }
            if(stack.size()<=1) break;
        }
    }
    if(stack.size()!=1)
    {
        emit HandleFileChanged(name,"菜单结构错误2.",false);
        return false;
    }
    k=QString("%1:").arg(name);
    if(defines.find(k)==defines.end()) defines.insert(k,QString("output_define(%1);").arg(name));
    if(pDoc) delete pDoc;
    pDoc=new xml_document();
    tmp=QString("<widget class=\"QMenuBar\" name=\"%1\"><property name=\"geometry\"><rect><x>0</x><y>0</y><width>1193</width><height>26</height></rect></property></widget>").arg(name);
    auto rt=pDoc->load_string(tmp.toUtf8());
    if(!rt)
    {
        emit HandleFileChanged(name,QString("菜单汇出xml错误:%1").arg(rt.description()),false);
        return false;
    }
    xml_node root=pDoc->child("widget");
    auto p=stack.pop();
    if(p->pList && p->pList->count()>0)
    {
        for(int i=0,size=p->pList->count();i<size;i++)
        {
            p->pList->at(i)->Data=QString("%1").arg(i);
            if(!AppendMenuXml(p->pList->at(i),root)) {
                emit HandleFileChanged(name,"菜单汇出xml错误.",false);
                return false;
            }
        }
    }
    tmp=pathCombine({targetPath,"menu"});
    if(!pathExists(tmp)) createDir(tmp);
    tmp=pathCombine({tmp,QString("%1.xml").arg(name)});
    if(fileExists(tmp)) deleteFile(tmp);
    pDoc->save_file(tmp.toUtf8(),PUGIXML_TEXT("\t"),pugi::format_default|pugi::format_no_declaration);
    emit HandleFileChanged(name,tmp,true);
    return true;
}

bool RcToXml::AppendMenuXml(RcToXml::NodeData *p, xml_node &parent)
{
    int i,size;
    switch(p->Type)
    {
    case eMenuItem:
    {
        if(p->pList && (size=p->pList->count())>0)
        {
            if(p->Name.isEmpty()) p->Name=QString("menu%1").arg(p->Data);
            auto node=parent.append_child("widget");
            node.append_attribute("class").set_value("QMenu");
            node.append_attribute("name").set_value(p->Name.toStdString().c_str());
            if(!p->Text.isEmpty())
            {
                auto child=node.append_child("property");
                child.append_attribute("name").set_value("title");
                child=child.append_child("string");
                child.text().set(p->Text.toStdString().c_str());
            }
            auto child=parent.append_child("addaction");
            child.append_attribute("name").set_value(p->Name.toStdString().c_str());
            for(i=0;i<size;i++)
            {
                p->pList->at(i)->Data= p->Data+QString("%1").arg(i,2,10,QChar('0'));
                if(!AppendMenuXml(p->pList->at(i),node)) return false;
            }
        }else{
            if(p->Name.isEmpty()) p->Name=QString("menu%101").arg(p->Data);
            auto node=parent.append_child("addaction");
            node.append_attribute("name").set_value(p->Name.toStdString().c_str());

            node=pDoc->root().append_child("action");
            node.append_attribute("name").set_value(p->Name.toStdString().c_str());
            node=node.append_child("property");
            node.append_attribute("name").set_value("text");
            node=node.append_child("string");
            node.text().set(p->Text.toStdString().c_str());
        }
        break;
    }
    case eMenuSeparator:
    {
        auto node=parent.append_child("addaction");
        node.append_attribute("name").set_value("separator");
        break;
    }
    default:
        return false;
    }
    return true;
}
