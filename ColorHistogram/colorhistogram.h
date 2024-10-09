#ifndef COLORHISTOGRAM_H
#define COLORHISTOGRAM_H

#include <QtWidgets>

class ColorHistogram : public QWidget
{
    Q_OBJECT;

    QImage image;
    QLabel *xTitle;
    QLabel *yTitle;
    QLabel *chart;
    QComboBox *scale;
    QSlider *slider;
    QLabel *sliderLabel;
    QComboBox *colors;

    QVector<int> counts;
    QVector<QPixmap> slices;
public:
    ColorHistogram(QImage newImage);

    void processImage();
    void changeColor();
    void sliderMoved(int x);
};

#endif // COLORHISTOGRAM_H
