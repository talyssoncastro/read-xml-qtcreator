#ifndef XMLELEMENT_H
#define XMLELEMENT_H

#include <QString>

class XMLElement
{
public:
    QString getName() const { return name; }
    void setName(const QString &value) { name = value; }

    QString getText() const { return text; }
    void setText(const QString &value) { text = value; }

    QMap<QString, QString> getAttributes() const { return attributes; }
    void setAttributes(const QMap<QString, QString> &value) { attributes = value; }
    void addAttribute(QString key, QString value) { attributes.insertMulti( key, value ); }

    QList<XMLElement *> getChildren() const { return children; }
    void setChildren(const QList<XMLElement *> &value) { children = value; }
    void addChild( XMLElement* value ) { children << value; }

    XMLElement *getParent() const { return parent; }
    void setParent(XMLElement *value) { parent = value; }

private:
    QString name;
    QString text;
    QMap<QString, QString> attributes;
    QList<XMLElement*> children;
    XMLElement* parent;
};

#endif // XMLELEMENT_H
