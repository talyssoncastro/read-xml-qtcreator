#ifndef EASYXMLSTREAMREADER_H
#define EASYXMLSTREAMREADER_H

#include <QXmlStreamReader>
#include <QString>
#include <QStringList>
#include <QFile>

/*
 * Examples:
 *
 * void processElementMethod(EasyXmlStreamReader &reader);
 *
 * EasyXmlStreamReader reader("file.xml");
 * reader.processAllWithMethod(QStringList() << "root" << "level_1" << "level_2",
 *                             processElementMethod);
 * reader.processAllNamedElementsWithMethod("level_2", processElementMethod);
 */

class EasyXmlStreamReader
{
public:
    EasyXmlStreamReader(const QString filename);
    EasyXmlStreamReader(QIODevice *ioDevice);
    ~EasyXmlStreamReader();

    void printElementTree();
    void printStartElementTree();

    typedef void (*fn_type)(EasyXmlStreamReader &reader, void *data);

    void processElementsByTagName(QString name, fn_type method, void *data);
    void processElementsByTagNameHierarchy(QStringList names, fn_type method, void *data);

    // Uses readElementText(SkipChildElements)
    QHash<QString, QString> getTextElements(QStringList names);
    // Uses text()
    QHash<QString, QString> getTextElements2(QStringList names);

    void printCurrent(QString extra = QString());

    QString errorString();

    QString _filename;
    QFile *_file;
    QXmlStreamReader _xml;
};

#endif // EASYXMLSTREAMREADER_H
