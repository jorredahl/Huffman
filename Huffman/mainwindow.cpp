#include "mainwindow.h"

#include <QtWidgets>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {

    counts.resize(256,0);
    codes.resize(256,"");

    QWidget *center = new QWidget();
    setCentralWidget(center);

    QVBoxLayout *mainLayout = new QVBoxLayout(center);

    QHBoxLayout *buttonBar = new QHBoxLayout();

    table = new QTableWidget();

    table->setSortingEnabled(false);

    mainLayout->addLayout(buttonBar);
    mainLayout->addWidget(table);

    QPushButton *loadButton = new QPushButton("Load");
    buttonBar->addWidget(loadButton, 0, Qt::AlignCenter);
    connect(loadButton , &QPushButton::clicked, this, &MainWindow::load);

    QPushButton *encodeButton = new QPushButton("Encode");
    buttonBar->addWidget(encodeButton, 0, Qt::AlignCenter);
    connect(encodeButton , &QPushButton::clicked, this, &MainWindow::encode);

    QPushButton *decodeButton = new QPushButton("Decode");
    buttonBar->addWidget(decodeButton, 0, Qt::AlignCenter);
    connect(decodeButton , &QPushButton::clicked, this, &MainWindow::decode);

    table->setRowCount(256);
    table->setColumnCount(4);

    table->setHorizontalHeaderLabels(QStringList() << "Code" << "Ch" << "#" << "Encoding");

    for (int iRow = 0; iRow < 256; ++iRow) {
        QTableWidgetItem *item1 = new QTableWidgetItem(QString::number(iRow, 16).rightJustified(2, '0'));
        table->setItem(iRow, 0, item1);
        QTableWidgetItem *item2 = new QTableWidgetItem(QChar(iRow));
        table->setItem(iRow, 1, item2);
    }
}

MainWindow::~MainWindow() {}

void MainWindow::load() {
    QString fileName = QFileDialog::getOpenFileName();
    QFile in(fileName);
    if(!in.open(QIODevice::ReadOnly)) {
        QMessageBox::information(this, "Error", QString("Can't open file \"%1\"").arg(fileName));
        return;
    }
    bArray = in.readAll();
    table->sortItems(0);
    table->setSortingEnabled(false);
    for (int i = 0; i < 256; ++i) {
        counts[i] = 0;
        codes[i] = "";
        QTableWidgetItem *item = new QTableWidgetItem("");
        table->setItem(i, 3, item);
    }
    children.erase(children.begin(), children.end());
    for (int i = 0; i < bArray.length(); ++i) {
        ++counts[(unsigned char) bArray[i]];
    }
    for (int iRow = 0; iRow < 256; ++iRow) {
        QTableWidgetItem *item = new QTableWidgetItem();
        item->setData(Qt::DisplayRole, counts.at(iRow));
        table->setItem(iRow, 2, item);
    }
}

void MainWindow::encode() {
    QMultiMap<int, QByteArray> toDo;
    for (int i = 0; i < 256; ++i) {
        if (counts.at(i) > 0) {
            toDo.insert(counts.at(i), QByteArray(1, char(i)));
        }
    }
    while (toDo.size() > 1) {
        int w0 = toDo.begin().key();
        QByteArray a0 = toDo.begin().value();
        toDo.erase(toDo.begin());
        int w1 = toDo.begin().key();
        QByteArray a1 = toDo.begin().value();
        toDo.erase(toDo.begin());
        children[a0+a1] = qMakePair(a0, a1);
        toDo.insert(w0 + w1, a0 + a1);
    }
    if (children.isEmpty()) {
        if (toDo.size() == 0) {
            return;
        } else {
            codes[(unsigned char) toDo.begin().value()[0]] = "0";
            QTableWidgetItem *item = new QTableWidgetItem(codes.at((unsigned char) toDo.begin().value()[0]));
            table->setItem((unsigned char) toDo.begin().value()[0], 3, item);
        }
    } else {
        QByteArray root = toDo.begin().value();
        for (int i = 0; i < 256; ++i) {
            createEncoding(root, "", QByteArray(1, char(i)));
            QTableWidgetItem *item = new QTableWidgetItem(codes.at(i));
            table->setItem(i, 3, item);
        }
    }
    table->setSortingEnabled(true);

    QString fileName = QFileDialog::getSaveFileName();
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::information(this, "Error", QString("Can't write to file \"%1\"").arg(fileName));
        return;
    }
    QDataStream out(&file);
    QString binaryString = "";
    for (int i = 0; i < bArray.length(); ++i) {
        binaryString += codes[(unsigned char) bArray[i]];
    }
    QMultiMap<QString, QByteArray> encodingKey;
    for (int i = 0; i < codes.length(); ++i) {
        if (counts[i] > 0)
            encodingKey.insert(codes[i], QByteArray(1, QChar(i).toLatin1()));
    }
    QByteArray output;
    for (int i = 0; i < binaryString.length() / 8; ++i) {
        output.append(char(binaryString.mid(8 * i, 8).toInt(nullptr, 2)));
    }
    out << encodingKey;
    out << binaryString.length();
    out.writeRawData(output.data(), output.length());
}

void MainWindow::decode() {
    QString inFileName = QFileDialog::getOpenFileName();
    QFile in(inFileName);
    if(!in.open(QIODevice::ReadOnly)) {
        QMessageBox::information(this, "Error", QString("Can't open file \"%1\"").arg(inFileName));
        return;
    }
    QDataStream encoded(&in);
    QMultiMap<QString, QByteArray> encodingKey;
    encoded >> encodingKey;
    qsizetype numBits;
    encoded >> numBits;
    QByteArray bytes((numBits + 7) / 8, 0);
    encoded.readRawData(bytes.data(), bytes.length());
    QString bits;
    for (int i = 0; i < bytes.length(); ++i) {
        bits += QString::number((unsigned char) bytes[i], 2).rightJustified(8, '0');
    }
    QString temp = "";
    QByteArray decoded;
    for (int i = 0; i < numBits; ++i) {
        temp += bits.at(i);
        if (encodingKey.contains(temp)) {
            decoded += encodingKey.value(temp);
            temp = "";
        }
    }

    QString outFileName = QFileDialog::getSaveFileName();
    QFile outFile(outFileName);
    if (!outFile.open(QIODevice::WriteOnly)) {
        QMessageBox::information(this, "Error", QString("Can't write to file \"%1\"").arg(outFileName));
        return;
    }
    QDataStream out(&outFile);
    out.writeRawData(decoded.data(), decoded.length());
}

void MainWindow::createEncoding(QByteArray current, QString code, QByteArray target) {
    if (current == target) {
        codes[(unsigned char) target[0]] = code;
        return;
    } else if (children[current].first.contains(target)) {
        createEncoding(children[current].first, code + "0", target);
    } else if (children[current].second.contains(target)) {
        createEncoding(children[current].second, code + "1", target);
    } else {
        return;
    }
}
