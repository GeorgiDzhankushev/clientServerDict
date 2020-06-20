#include <dictServer.h>

#include <QCoreApplication>

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    dictServer serv;

    return a.exec();
}
