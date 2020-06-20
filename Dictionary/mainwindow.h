#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QMenu>
#include <QDialog>
#include <QTcpSocket>

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr); // default constructor
    void getLangs(); // retrieves list of all languages in DB and some words
private slots:
    void translate(); // searches for translation of typed word in DB
    void addWord(); // creates GUI for adding new word to DB
    void addLanguage(); // creates GUI for adding new lang to DB
    void closeEvent (QCloseEvent *event); // terminate connection with server on program exit
private:
    QStringList *langs; // list of available languages
    QStringList *words; // list of several english words from the database
    QLabel *fromLbl; // label displaying "from:"
    QLabel *toLbl; // label displaying "to:"
    QComboBox *fromCombo; // dropdown for selecting "from" lang
    QComboBox *toCombo; // dropdown for selecting "to" lang
    QLineEdit *textBox; // element where user types the word they would like translated
    QLineEdit *textBox2; // element where the word translation appears
    QPushButton *translateBtn; // button for calling the translate function
    QPushButton *addBtn; // button that reveals a menu for adding words or langs to db
    QMenu *addBtnMenu; // menu for adding words or langs to db
    QAction *addWordAction; // menu option for adding a new word to db
    QAction *addLangAction; // menu option for adding a new lang to db
    const QString HOST = "127.0.0.1"; // server address
    const quint16 PORT = 4000; // server port
    QTcpSocket *socket; // network socket used to communicate with server

};

#endif // MAINWINDOW_H
