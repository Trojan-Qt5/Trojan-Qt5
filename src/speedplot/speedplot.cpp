#include "speedplot.h"
#include "ui_speedplot.h"
#include "midman.h"

SpeedPlot::SpeedPlot(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SpeedPlot)
{
    ui->setupUi(this);

    speedChartWidget = new SpeedWidget(this);
    ui->speedChart->addWidget(speedChartWidget);

    connect(&MidMan::getConnection(), &Connection::dataTrafficAvailable, this, &SpeedPlot::onStatsAvailable);
    connect(MidMan::getStatusConnection(), &Connection::connectionSwitched, this, &SpeedPlot::onReconnect);

}

SpeedPlot::~SpeedPlot()
{
    delete ui;
}

void SpeedPlot::onReconnect()
{
    connect(&MidMan::getConnection(), &Connection::dataTrafficAvailable, this, &SpeedPlot::onStatsAvailable);
}

void SpeedPlot::onStatsAvailable(QList<quint64> data)
{
    speedChartWidget->AddPointData(data);
}
