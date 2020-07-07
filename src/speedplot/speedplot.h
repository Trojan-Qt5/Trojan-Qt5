#ifndef SPEEDPLOT_H
#define SPEEDPLOT_H

#include "speedwidget.hpp"

#include <QDialog>
#include <QFocusEvent>

namespace Ui {
class SpeedPlot;
}

class SpeedPlot : public QDialog
{
    Q_OBJECT

public:
    explicit SpeedPlot(QWidget *parent = nullptr);
    ~SpeedPlot();

public slots:
    void onStatsAvailable(QList<quint64> data);
    void onReconnect();

private:
    Ui::SpeedPlot *ui;
    SpeedWidget *speedChartWidget;
};

#endif // SPEEDPLOT_H
