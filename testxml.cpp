#include "testxml.h"

TestXml::TestXml()
{

}

void TestXml::testQMap() {

    QString key1 = "Key";
    QString value1 = "Value 1";
    QString* keyPtr1 = &key1;

    QString key2= "Key";
    QString value2 = "Value 2";
    QString* keyPtr2 = &key2;

    QMap<QString*, QString> valores;
    valores.insertMulti( keyPtr1, value1 );
    valores.insertMulti( keyPtr2, value2 );

    qDebug() << &value1;
    qDebug() << &value2;

    QString* m = valores.begin().key(); //valores.value( key1, "Não encontrado" );

    QString v = valores.value( keyPtr2, "Não encontrado" );

    qDebug() << v;
    qDebug() << m;

    qDebug() << "----------------------";

    QMap<QString, QString> valores2;
    valores2.insertMulti(key1, value1);
    valores2.insertMulti(key2, value2);

    qDebug() << valores2.value( key1, "Não encontrado 1" );
    qDebug() << valores2.value( key2, "Não encontrado 2" );

    QList<QString> values = valores2.values( key2 );

    foreach (QString v, values) {
        qDebug() << v;
    }

}

void TestXml::testXMLTabela() {

    //XMLElement* element = NULL;

    QString xmlTabela = readFile( ":/resource/test_example.xml" );

    //readXML2( xmlTabela );

    //EasyXmlStreamReader e(":/resource/test_example.xml");
    //e.printElementTree();

    readXML2( xmlTabela );

    exit(0);

}

QString TestXml::readFile(QString path) {

    QResource common( path ) ;

    QFile commonFile( common.absoluteFilePath() );

    if ( !commonFile.open( QIODevice::ReadOnly | QIODevice::Text ) ) {
        qCritical() << "Não foi possível abrir arquivo" << commonFile.fileName() << "Motivo:" << commonFile.errorString();
    }

    QTextStream in( &commonFile );

    QString res = in.readAll();

    commonFile.close();

    return res;

    }

XMLElement* TestXml::readXML(QString xml, XMLElement *parent) {

    QXmlStreamReader reader( xml );

    if (parent == NULL) {
        parent = new XMLElement;
    }

    while ( !reader.atEnd() && !reader.hasError() ) {
        reader.readNext();

        qDebug() << reader.name();

        if (reader.isStartElement()) {
            XMLElement* element = new XMLElement;

            QXmlStreamAttributes attributes = reader.attributes();
            QString childXML = reader.readElementText( QXmlStreamReader::IncludeChildElements );
            QString text = reader.readElementText( QXmlStreamReader::SkipChildElements );

            qDebug() << childXML;

            for( int i = 0; i < attributes.length(); i++ ) {
                element->addAttribute( attributes.at( i ).name().toString(), attributes.at( i ).value().toString() );
            }

            element->setText( text );

            if ( !childXML.isEmpty() ) {
                readXML( childXML, element );
            }

            parent->addChild( element );
        }

    }

    return parent;

}

QString readElement(QXmlStreamReader* reader) {

    while (reader->readNextStartElement()) {
        qDebug() << readElement( reader );
    }

    //qDebug() << reader->readElementText(QXmlStreamReader::IncludeChildElements);
    //qDebug() << reader->readElementText(QXmlStreamReader::SkipChildElements);

    return reader->name().toString() + " - " + reader->readElementText();

}

void checkXML(QString xml) {

    QXmlStreamReader reader(xml);

   bool gotoNext = true;
   while(!reader.atEnd())
   {
      if(gotoNext) {
         reader.readNextStartElement();
      }
      QString output = reader.name().toString();
      reader.readNext();
      if(reader.isEndElement()) {
         output += " is empty";
         gotoNext = true;
      } else {
         output += " has children";
         gotoNext = false;
      }
      qDebug() << output;
   }

}

XMLElement* printXML(QString xml) {

    QXmlStreamReader reader(xml);

    XMLElement* element = new XMLElement;
    XMLElement* original = element;

    bool skip = false;

    while (!reader.atEnd() && reader.readNext()) {
        if ( reader.tokenType() == QXmlStreamReader::StartElement ) {

            if (skip) {
                XMLElement* newElement = new XMLElement;
                newElement->setParent( element );
                element->addChild( newElement );
                element = newElement;
                skip = false;
            }

            element->setName( reader.name().toString() );

            QXmlStreamAttributes attributes = reader.attributes();

            for( int i = 0; i < attributes.length(); i++ ) {
                element->addAttribute( attributes.at( i ).name().toString(), attributes.at( i ).value().toString() );
            }

        } else if (reader.tokenType() == QXmlStreamReader::Characters) {

            if (!skip) {
                element->setText( reader.text().toString() );

                XMLElement* newElement = new XMLElement;
                newElement->setParent( element );
                element->addChild( newElement );
                element = newElement;
            }

        } else if (reader.tokenType() == QXmlStreamReader::EndElement) {
            element = element->getParent();
            if (!skip) {
                element = element->getParent();
            }
            skip = true;
        }

    }

    return original;

}

void TestXml::readXML2(QString xml) {

    QXmlStreamReader reader( xml );

    //checkXML(xml);

    //readElement( &reader );

    XMLElement* tree = printXML( xml );

    qDebug() << tree->getName();

    /*
    if (reader.readNextStartElement()) {
        if (reader.name() == "root"){
            while(reader.readNextStartElement()){

                if(reader.name() == "childA"){
                    while(reader.readNextStartElement()){
                        //if(reader.name() == "subchild1"){
                            QString s = reader.readElementText();
                            qDebug() << s;
                        //}
                        //else
                            //reader.skipCurrentElement();
                    }
                } else if ( reader.name() == "childB" ) {
                    QString s = reader.readElementText();
                    qDebug() << s;
                }
                else {
                    reader.skipCurrentElement();
                }

            }
        }
        else
            reader.raiseError(QObject::tr("Incorrect file"));
    }
    */

    exit(0);

}
