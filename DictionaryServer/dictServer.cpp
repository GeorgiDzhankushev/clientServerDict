#include <dictServer.h>

// default constructor
dictServer::dictServer(QObject *parent) : QObject(parent) {
    // initialize server socket for listening
    server = new QTcpServer();
    connect(server, SIGNAL(newConnection()), this, SLOT(newConnection()));
    server->listen(QHostAddress::Any, PORT);

    // initialize object for querying local database
    dataBase = QSqlDatabase::addDatabase("QSQLITE");
    dataBase.setDatabaseName(PATH_TO_DB);
    dataBase.open();
    sqlQuery = QSqlQuery(dataBase);

    words = new QStringList();

}

// handles new client connections
void dictServer::newConnection() {
    socket = server->nextPendingConnection();

    // get list of languages
    QStringList langs;
    for (int i = 1; i < dataBase.record("words").count(); i++)
        langs.append(dataBase.record("words").field(i).name());

    // get some words
    sqlQuery.exec("SELECT english FROM words LIMIT 10");
    while (sqlQuery.next())
        words->append(sqlQuery.value(0).toString());

    // send them to client
    socket->write(langs.join(',').toUtf8() + "." + words->join(',').toUtf8());
    socket->waitForBytesWritten();

    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
}

// sends the requested information to the client
void dictServer::readyRead() {
    QByteArray arr = socket->readAll();
    QStringList data = QString(arr).split('.');
    if (data[0] == "1") { // translate function was called
        sqlQuery.exec("SELECT " + data[3] + " FROM words WHERE " + data[2] + " = LOWER('" + data[1] + "')");
        QString translation = (sqlQuery.next() && sqlQuery.value(0).toString().length() > 1) ? sqlQuery.value(0).toString() : "word not found.";
        socket->write(translation.toUtf8());
        socket->waitForBytesWritten();
    } else if (data[0] == "2") { // addWord function was called
        QString w = "";
        for (QString str : data[2].split(','))
            w += "LOWER('" + str + "'), ";
        sqlQuery.exec("INSERT INTO words (" + data[1] + ") VALUES (" + w.left(w.length()-2) + ")");
    } else if (data[0] == "3") { // addLang function was called
        QStringList newLang = data[2].split(',');
        QStringList english = data[3].split(',');
        sqlQuery.exec("ALTER TABLE words ADD COLUMN " + data[1] + " VARCHAR(64)");
        for (int i = 0; i < newLang.length(); i++)
            sqlQuery.exec("UPDATE words SET " + data[1] + " = '" + newLang[i] + "' WHERE english = '" + english[i] + "'");
    }
}
