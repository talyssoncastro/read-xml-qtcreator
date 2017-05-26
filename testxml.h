#ifndef TESTXML_H
#define TESTXML_H

#include <QDebug>
#include <QString>
#include <QXmlStreamReader>
#include <QFile>
#include <QResource>

#include "xmlelement.h"
#include "easyxmlstreamreader.h"

class TestXml
{
public:
    TestXml();

    void testQMap();
    void testXMLTabela();

    QString readFile(QString path);

    XMLElement* readXML(QString xml, XMLElement* parent = NULL);
    void readXML2(QString xml);

};

#endif // TESTXML_H
