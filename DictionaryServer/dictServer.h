#ifndef DICTSERVER_H
#define DICTSERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QtSql>
#include <QSqlQuery>

class dictServer : public QObject {
    Q_OBJECT
public:
    explicit dictServer(QObject *parent = nullptr); // default constructor
public slots:
    void newConnection(); // handles new client connections
    void readyRead(); // sends the requested information to the client
private:
    QTcpServer *server; // server socket that listens for incoming connections
    QTcpSocket *socket; // TCP socket created when accepting new connection
    const quint16 PORT = 4000; // port on which the server socket listens
    const QString PATH_TO_DB = "..\\DictionaryServer\\words.db"; // path to the words database
    QSqlDatabase dataBase; // object which represents connection with the database
    QSqlQuery sqlQuery; // object for querying the database
    QStringList *words; // list of several english words

};

#endif // DICTSERVER_H
