#include <QCoreApplication>

#include <QDebug>

#include "testxml.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


    QList<QString> objectListToIgnore;
    objectListToIgnore << "endereco";

    QString xml = "<endereco><cidade cod_ibge=\"4107207\" uf=\"PR\">Dois Vizinhos</cidade><rua>Presidente Getulio Vargas</rua></endereco>";

    TestXml t;
    t.testXMLTabela();



    return a.exec();
}
