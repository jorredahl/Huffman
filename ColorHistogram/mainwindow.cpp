#include "mainwindow.h"

#include <QtWidgets>

#include "colorhistogram.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {

    QAction *openFileAct = new QAction("&Open image file");
    connect(openFileAct, &QAction::triggered, this, &MainWindow::openFileSlot);
    openFileAct->setShortcut(Qt::CTRL | Qt::Key_O);

    QMenu *fileMenu = new QMenu("&File");
    fileMenu->addAction(openFileAct);
    menuBar()->addMenu(fileMenu);

    QSettings settings("FJS Systems", "Graphics1");
    lastDir = settings.value("lastDir", "").toString();
}

MainWindow::~MainWindow() {
    QSettings settings("FJS Systems", "Graphics1");
    settings.setValue("lastDir", lastDir);
}

void MainWindow::openFileSlot() {
    QString fName = QFileDialog::getOpenFileName(this, "Select image file",  lastDir, "Image files (*.png *.jpeg *.jpg *.bmp)");
    if (fName.isEmpty()) return;

    QImage image(fName);
    if (image.isNull()) return;

    lastDir = QFileInfo(fName).absolutePath();

    ColorHistogram *colorHistogram = new ColorHistogram(image);
    setCentralWidget(colorHistogram);

}
