#include "mainwindow.h"

// default constructor
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    // establish connection with server
    socket = new QTcpSocket(this);
    socket->connectToHost(HOST, PORT);
    socket->waitForConnected(3000);

    getLangs(); // get all langs in DB

    // draw main window GUI
    this->setFixedSize(245, 100);
    fromLbl = new QLabel("From: ", this);
    fromLbl->setGeometry(5, 15, 40, 20);
    fromCombo = new QComboBox(this);
    fromCombo->addItems(*langs);
    fromCombo->setGeometry(40, 15, 75, 20);
    toLbl = new QLabel("To: ", this);
    toLbl->setGeometry(125, 15, 20, 20);
    toCombo = new QComboBox(this);
    toCombo->addItems(*langs);
    toCombo->setGeometry(150, 15, 75, 20);
    textBox = new QLineEdit(this);
    textBox->setGeometry(5, 40, 115, 20);
    textBox2 = new QLineEdit(this);
    textBox2->setGeometry(125, 40, 115, 20);
    textBox2->setReadOnly(true);
    translateBtn = new QPushButton("Translate", this);
    translateBtn->setGeometry(95, 70, 60, 25);
    addBtn = new QPushButton("Add", this);
    addBtn->setGeometry(200, 70, 40, 25);
    addBtnMenu = new QMenu(this);
    addWordAction = addBtnMenu->addAction("Add word");
    addLangAction = addBtnMenu->addAction("Add language");
    addBtn->setMenu(addBtnMenu);

    // connect functions to the buttons
    connect(translateBtn, SIGNAL(clicked()), this, SLOT(translate()));
    connect(addWordAction, SIGNAL(triggered()), this, SLOT(addWord()));
    connect(addLangAction, SIGNAL(triggered()), this, SLOT(addLanguage()));
} // default constructor

// retrieves list of all languages in DB and some words
void MainWindow::getLangs() {
    socket->waitForReadyRead();
    QString str = QString(socket->readAll());
    langs = new QStringList(str.split('.')[0].split(','));
    words = new QStringList(str.split(".")[1].split(','));
} // getLangs()

// searches for translation of typed word in DB
void MainWindow::translate() {
    if (textBox->text().isEmpty()) return;

    // request translation from server
    QString word = textBox->text();
    QString fromLang = fromCombo->currentText();
    QString toLang = toCombo->currentText();
    QByteArray arr = "1." + word.toUtf8() + '.' + fromLang.toUtf8() + '.' + toLang.toUtf8();
    socket->write(arr);
    socket->waitForBytesWritten();

    // receive and display translation
    socket->waitForReadyRead();
    QString translation = QString(socket->readAll());
    textBox2->setText(translation);
} // translate()

// creates GUI for adding new word to DB
void MainWindow::addWord() {
    QDialog *wordDialog = new QDialog(this);
    QLabel *labelArr = new QLabel[langs->length()];
    QLineEdit *lineArr = new QLineEdit[langs->length()];

    // draw GUI for writing the word in all langs
    for (int i = 0; i < langs->length(); i++) {
        labelArr[i].setParent(wordDialog);
        labelArr[i].setText(langs->at(i));
        labelArr[i].setGeometry(5, 5 + 20 * i, 50, 18);
        lineArr[i].setParent(wordDialog);
        lineArr[i].setGeometry(60, 5 + 20 * i, 100, 18);
    }
    lineArr[0].setPlaceholderText("required");

    QPushButton *btn = new QPushButton("Add word", wordDialog);
    btn->setGeometry(48, 10 + 20 * langs->length(), 70, 20);
    wordDialog->setModal(true);
    wordDialog->show();

    // lambda for button press
    connect(btn, &QPushButton::clicked, this, [=](){
        if (lineArr[0].text().length() < 2) return;

        QStringList l, w;

        // get the word in the languages specified by the user
        for (int i = 0; i < this->langs->length(); i++)
            if (lineArr[i].text().length() > 1) {
                l.push_back(labelArr[i].text());
                w.push_back(lineArr[i].text());
            }

        // send it to the server
        QByteArray arr = "2." + l.join(',').toUtf8() + "." + w.join(',').toUtf8();
        socket->write(arr);
        socket->waitForBytesWritten();

        wordDialog->done(0);
    }); // lambda for button press
} // addWord()

// creates GUI for adding new lang to DB
void MainWindow::addLanguage() {
    // select some english words for optional translation into new lang
    QStringList *engWords = new QStringList();
    for (QString w : *words)
        engWords->append(w);

    // draw GUI
    QDialog *wordDialog = new QDialog(this);
    QLabel *labelArr = new QLabel[engWords->length()];
    QLineEdit *lineArr = new QLineEdit[engWords->length()];
    QLabel *nameOfLang = new QLabel("Name of language:", wordDialog);
    nameOfLang->setGeometry(40, 5, 90, 20);
    QLineEdit *nameOfLangInput = new QLineEdit(wordDialog);
    nameOfLangInput->setPlaceholderText("required");
    nameOfLangInput->setGeometry(40, 25, 90, 18);
    for (int i = 0; i < engWords->length(); i++) {
        labelArr[i].setParent(wordDialog);
        labelArr[i].setText(engWords->at(i));
        labelArr[i].setGeometry(5, 50 + 20 * i, 55, 18);
        lineArr[i].setParent(wordDialog);
        lineArr[i].setGeometry(65, 50 + 20 * i, 100, 18);
    }

    QPushButton *btn = new QPushButton("Add lang", wordDialog);
    btn->setGeometry(48, 55 + 20 * engWords->length(), 70, 20);
    wordDialog->setModal(true);
    wordDialog->show();

    // lambda for button press
    connect(btn, &QPushButton::clicked, this, [=](){
        if (nameOfLangInput->text().length() < 2) return;

        // ensure language is not already present
        QString newLang = nameOfLangInput->text();
        if (langs->contains(newLang.toLower())) return

        // add lang to client GUI
        this->langs->append(newLang);
        this->fromCombo->addItem(newLang);
        this->toCombo->addItem(newLang);

        // send entered data to server
        QStringList w, l;
        for (int i = 0; i < engWords->length(); i++)
            if (lineArr[i].text().length() > 2) {
                w.push_back(lineArr[i].text());
                l.push_back(labelArr[i].text());
            }

        socket->write("3." + newLang.toUtf8() + "." + w.join(',').toUtf8() + "." + l.join(',').toUtf8());
        socket->waitForBytesWritten();

        wordDialog->done(0);
    }); // lambda for button press
} // addLanguage()

// terminate connection with server on program exit
void MainWindow::closeEvent(QCloseEvent *event) {
    socket->close();
}
