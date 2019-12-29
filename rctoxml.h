#ifndef RCTOXML_H
#define RCTOXML_H

#include <QMap>
#include <QStack>
#include <QFile>
#include <QTextStream>
#include "pugixml/pugixml.hpp"

using namespace pugi;

class RcToXml:public QObject
{
    Q_OBJECT
public:
    RcToXml();
    RcToXml(const QString &fileName);
    ~RcToXml();

    void SetFile(const QString &fileName);
    void SetTargetPath(const QString &path);
    bool ReadRcObjects();
    bool ReadDefines();
    bool CreateDefines();
    void CloseDefines();
signals:
    void HandleFileChanged(const QString &objName,const QString &msg,const bool success);
protected:
    enum NodeType
    {
        eNone,
        eSubMenu,
        eMenuItem,
        eMenuSeparator,
        eDialog
    };
    struct NodeData
    {
        QString Name="";
        QString Text="";
        QString Data="";
        NodeType Type=NodeType::eNone;
        QVector<NodeData *> *pList=nullptr;
        NodeData()
        {}

        NodeData(const NodeType type)
        {
            Type=type;
        }

        NodeData(const QString &name,const NodeType type)
        {
            Name=name;
            Type=type;
        }

        NodeData(const QString &name,const QString &text,const NodeType type)
        {
            Name=name;
            Text=text;
            Type=type;
        }

        ~NodeData()
        {
        }
    };
    QString rcfileName="";
    QString targetPath="";
    QFile *pOut=nullptr;
    QTextStream *pOutStream=nullptr;
    QMap<QString,QString> headers;
    QMap<QString,QString> defines;
    QMap<QString,QString> AllDefines;
    QStack<NodeData *> stack;
    QFile *pRcFile=nullptr;
    QTextStream *pStream=nullptr;
    xml_document *pDoc=nullptr;

    void Close();
    void ReadLine(QString &line);
    void RemoveSpeChar(QString &line);
    bool ReadMenus(const QString &name);
    bool ReadDialog(const QString &tline,const QString &tnextline);
    bool AppendMenuXml(NodeData *data,xml_node &parent);
};

#endif // RCTOXML_H
