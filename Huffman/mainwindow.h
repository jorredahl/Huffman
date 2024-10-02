#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QPushButton;
class QTableWidget;
class Node;

class MainWindow : public QMainWindow
{
    Q_OBJECT

    QTableWidget *table;

    QByteArray bArray;
    QVector<int> counts;
    QMap<QByteArray, QPair<QByteArray, QByteArray> > children;
    QVector<QString> codes;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void load();
    void encode();
    void decode();
    void createEncoding(QByteArray current, QString code, QByteArray target);
};
#endif // MAINWINDOW_H
