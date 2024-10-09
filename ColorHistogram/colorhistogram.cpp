#include "colorhistogram.h"
#include "imageviewer.h"

#include <QtWidgets>

ColorHistogram::ColorHistogram(QImage newImage) {
    QHBoxLayout *layout = new QHBoxLayout();
    setLayout(layout);
    image = newImage;
    counts = QVector<int>(2 << 24, 0);
    processImage();
    slices = QVector<QPixmap>(256);
    ImageViewer *imageViewer = new ImageViewer(image);
    QVBoxLayout *histo = new QVBoxLayout();
    layout->addWidget(imageViewer);
    layout->addLayout(histo);

    histo->addWidget(new QLabel("Presence of Colors in the Image"), 0, Qt::AlignCenter);

    QHBoxLayout *xAxisLabels = new QHBoxLayout();
    QVBoxLayout *yAxisLabels = new QVBoxLayout();
    xAxisLabels->addWidget(new QLabel("0"), 0, Qt::AlignLeft);
    xTitle = new QLabel("Green");
    xAxisLabels->addWidget(xTitle, 0, Qt::AlignCenter);
    xAxisLabels->addWidget(new QLabel("255"), 0, Qt::AlignRight);
    histo->addLayout(xAxisLabels);
    yTitle = new QLabel("Blue");
    yAxisLabels->addWidget(yTitle, 0, Qt::AlignBottom);
    yAxisLabels->addWidget(new QLabel("255"), 0, Qt::AlignBottom);

    QHBoxLayout *chartLayout = new QHBoxLayout;
    chart = new QLabel();
    chart->setPixmap(slices[0]);
    chartLayout->addLayout(yAxisLabels);
    chartLayout->addWidget(chart, 0, Qt::AlignCenter);
    histo->addLayout(chartLayout);

    scale = new QComboBox();
    scale->addItem("1");
    scale->addItem("2");
    scale->addItem("8");
    scale->addItem("16");
    scale->addItem("32");
    scale->addItem("64");
    scale->addItem("128");
    scale->addItem("256");
    colors = new QComboBox();
    colors->addItem("Red");
    colors->addItem("Green");
    colors->addItem("Blue");

    connect(scale, &QComboBox::currentTextChanged, this, &ColorHistogram::changeColor);
    histo->addWidget(new QLabel("Scale:"), 0, Qt::AlignCenter);
    histo->addWidget(scale, 0, Qt::AlignCenter);

    slider = new QSlider(Qt::Horizontal);
    slider->setRange(0, 255);
    connect(slider, &QSlider::sliderMoved, this, &ColorHistogram::sliderMoved);
    sliderLabel = new QLabel("Choose " + colors->currentText() + " Value: " + QString::number(slider->value()));
    histo->addWidget(sliderLabel, 0, Qt::AlignCenter);
    histo->addWidget(slider, 0);

    connect(colors, &QComboBox::currentTextChanged, this, &ColorHistogram::changeColor);
    histo->addWidget(new QLabel("Choose Color:"), 0, Qt::AlignCenter);
    histo->addWidget(colors, 0, Qt::AlignCenter);
    changeColor();
    sliderMoved(slider->value());
}

void ColorHistogram::processImage() {
    for(int i = 0; i < image.height(); ++i) {
        for(int j = 0; j < image.width(); ++ j) {
            QRgb p = image.pixel(j, i);
            ++counts[qRed(p) + 256 * qGreen(p) + 65536 * qBlue(p)];
        }
    }
}

void ColorHistogram::changeColor() {
    int scaleFactor = scale->currentText().toInt();
    if (colors->currentText() == "Red") {
        xTitle->setText("Green");
        yTitle->setText("Blue");
        for(int i = 0; i < 256; ++i) {
            QImage tempImage(256,256,QImage::Format_RGB32);
            for(int j = 0; j < 256; ++j) {
                for(int k = 0; k < 256; ++k) {
                    int shade = 255 - (counts[i + 256 * j + 65536 * k] * scaleFactor);
                    if (shade < 0) shade = 0;
                    tempImage.setPixel(j, k, shade*0x010101);
                }
            }
            slices[i] = QPixmap::fromImage(tempImage);
        }
    } else if (colors->currentText() == "Green") {
        xTitle->setText("Red");
        yTitle->setText("Blue");
        for(int i = 0; i < 256; ++i) {
            QImage tempImage(256,256,QImage::Format_RGB32);
            for(int j = 0; j < 256; ++j) {
                for(int k = 0; k < 256; ++k) {
                    int shade = 255 - (counts[j + 256 * i + 65536 * k] * scaleFactor);
                    if (shade < 0) shade = 0;
                    tempImage.setPixel(j, k, shade*0x010101);
                }
            }
            slices[i] = QPixmap::fromImage(tempImage);
        }
    } else if (colors->currentText() == "Blue") {
        xTitle->setText("Red");
        yTitle->setText("Green");
        for(int i = 0; i < 256; ++i) {
            QImage tempImage(256,256,QImage::Format_RGB32);
            for(int j = 0; j < 256; ++j) {
                for(int k = 0; k < 256; ++k) {
                    int shade = 255 - (counts[j + 256 * k + 65536 * i] * scaleFactor);
                    if (shade < 0) shade = 0;
                    tempImage.setPixel(j, k, shade*0x010101);
                }
            }
            slices[i] = QPixmap::fromImage(tempImage);
        }
    }
    sliderMoved(slider->value());
    sliderLabel->setText("Choose " + colors->currentText() + " Value: " + QString::number(slider->value()));
}

void ColorHistogram::sliderMoved(int x) {
    chart->setPixmap(slices[x]);
    sliderLabel->setText("Choose " + colors->currentText() + " Value: " + QString::number(x));
}
