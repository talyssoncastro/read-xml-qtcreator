#include "easyxmlstreamreader.h"

#include <QFile>
#include <QHash>
#include <QDebug>

EasyXmlStreamReader::EasyXmlStreamReader(const QString filename)
{
    _file = new QFile(filename);
    _file->open(QIODevice::ReadOnly);
    _xml.setDevice(_file);
}

EasyXmlStreamReader::EasyXmlStreamReader(QIODevice *ioDevice) {
    _xml.setDevice(ioDevice);
}

EasyXmlStreamReader::~EasyXmlStreamReader() {
    if (_xml.device()) {
        // readNextStartElement() leaves the stream in
        // an invalid state at the end. A single readNext()
        // will advance us to EndDocument.
        if (_xml.tokenType() == QXmlStreamReader::Invalid)
            _xml.readNext();
        if (_xml.hasError()) {
            _xml.raiseError();
            qDebug() << errorString();
        }
    }
    if (_file) {
        _file->close();
        delete _file;
    }
}

// Debugging methods
void EasyXmlStreamReader::printElementTree() {
    int indent = 4;
    int level = 0;
    printCurrent("start");
    while (!_xml.atEnd() && _xml.readNext()) {
        if (_xml.tokenType() == _xml.EndElement)
            level--;
        printCurrent(QString(level * indent, ' '));
        if (_xml.tokenType() == _xml.StartElement)
            level++;
    }
    printCurrent("end");
}

void EasyXmlStreamReader::printStartElementTree() {
    int indent = 4;
    int level = 0;
    while (!_xml.atEnd()) {
        while (_xml.readNextStartElement())
            printCurrent(QString(level++ * indent, ' '));
        level--;
    }
    // This will print Invalid if uncommented
    // printCurrent(QString(level * indent, ' '));
}

// Helper method to print out where we are in the stream
void EasyXmlStreamReader::printCurrent(QString extra) {
    QString data;
    switch (_xml.tokenType()) {
    case QXmlStreamReader::StartElement:
    case QXmlStreamReader::EndElement:
        data = _xml.name().toString();
        break;
    case QXmlStreamReader::Characters:
    case QXmlStreamReader::Comment:
        data = "\"" + _xml.text().toString() + "\"";
        break;
    default:
        break;
    }
    data.replace("\n", "\\n");
    qDebug("%s %s: %s",
           qPrintable(extra),
           qPrintable(_xml.tokenString().remove("Element")),
           qPrintable(data));
}

// Acts like QDomDocument::elementsByTagName(name), applying method to each one
void EasyXmlStreamReader::processElementsByTagName(QString name, fn_type method, void *data) {
    if (name.isEmpty() || method == 0)
        return;

    while (!_xml.atEnd()) {
        while (_xml.readNextStartElement()) {
            if (_xml.name() == name) {
                method(*this, data);
                // Ensure that we are at the end of the element
                // matching name or else we might match a nested
                // element with the same name:
                // <name> ... <name>...<name/> ... </name>
                //
                // skipCurrentElement() moves forward to the nearest
                // end element which moves the current element up a
                // level. Using <0><1></1><2></2></0>: if we start at
                // <1>, we'd skip to </1>, then </0>.
                //
                // Edge case that we can't do anything about:
                // <0>  <1>  <0></0> <0></0> </1> </0>
                //           ^- start here
                // If we start from the second <0>, skip moves to the
                // nearby </0>. This will drop us out of the skip loop.
                // Then readNextStartElement() will put as at the next
                // <0> which will trigger a call to method(). The only
                // way to avoid this is to build our own local readNext
                // and skip methods keeping track of what level we're on.
                // Then, you'd need to ensure that method() only used
                // our methods to move around. QXmlStreamReader doesn't
                // expose any way of knowing where we are in the tree.
                while(!(_xml.isEndElement() && _xml.name() == name))
                    _xml.skipCurrentElement();
            }
        }
    }
}

// Like processElementsByTagName() above but with a defined hierarchy.
// method will only be applied to elements with a hierarchy that matches names.
// Example: names = QStringList() << "root element" << "next level" << ... << "name";
// This is a little faster than processElementsByTagName()
void EasyXmlStreamReader::processElementsByTagNameHierarchy(QStringList names, fn_type method, void *data)
{
    if (names.isEmpty() || method == 0)
        return;

    QString currentElementName = _xml.name().toString();
    QString name = names.first();
    names.removeFirst();
    while (!_xml.atEnd()) {
        while (_xml.readNextStartElement()) {
            if (_xml.name() != name) {
                _xml.skipCurrentElement();
                continue;
            }

            if (names.isEmpty())
                method(*this, data);
            else
                processElementsByTagNameHierarchy(names, method, data);

            // The just called level may not have left us
            // at the end of the element named name. This
            // explicitly moves us there. Otherwise we might
            // recurse unexpectedly. Example: if "name" has
            // a child element named "name" and we've returned
            // here before seeing the child, we'll end up
            // processing the child as if it were the parent.
            while(!(_xml.isEndElement() && _xml.name() == name))
                _xml.skipCurrentElement();
        }
        if (_xml.isEndElement() && _xml.name() == currentElementName)
            break;
    }
}

// Find each child element of the current element with a name found in names.
// Return a hash table of string results.
// The code can be made to return early once all names have been found.
// One method is to stop when the number of results equals the number of names.
// The other is to remove keys from names as they're found and return when
// names is empty. Performance may differ based on the number of child elements
// and the number of keys in names. Regardless of which approach is used, sorting
// names by the order that the keys can expect to be seen will yield the best results.
QHash<QString, QString> EasyXmlStreamReader::getTextElements(QStringList names) {
    QHash<QString, QString> results;
    // readNextStartElement() descends the XML tree which
    // is undesirable, but we skip over descendants below
    // which keeps us on the same level.
    while (!names.isEmpty() && _xml.readNextStartElement()) {
        if (!names.contains(_xml.name().toString())) {
            _xml.skipCurrentElement();
            continue;
        }
        // readElementText() internally skips the current element
        results.insert(_xml.name().toString(),
                       _xml.readElementText(_xml.SkipChildElements));
        names.removeOne(_xml.name().toString());
    }
    return results;
}

// Use .text() instead of .readElementText()
QHash<QString, QString> EasyXmlStreamReader::getTextElements2(QStringList names) {
    QHash<QString, QString> results;
    // readNextStartElement() descends the XML tree which
    // is undesirable, but we skip over descendants below
    // which keeps us on the same level.
    while (!names.isEmpty() && _xml.readNextStartElement()) {
        if (names.contains(_xml.name().toString())) {
            results.insert(_xml.name().toString(),
                           _xml.text().toString());
            names.removeOne(_xml.name().toString());
        }
        _xml.skipCurrentElement();
    }
    return results;
}

QString EasyXmlStreamReader::errorString() {
    return QObject::tr("%1\nLine %2, column %3")
            .arg(_xml.errorString())
            .arg(_xml.lineNumber())
            .arg(_xml.columnNumber());
}
