#ifndef STATUSBAR_H
#define STATUSBAR_H

#include <QStatusBar>

class QLabel;
class QPushButton;

class StatusBar : public QStatusBar
{
    Q_DISABLE_COPY(StatusBar)

public:
    StatusBar(QWidget *parent = nullptr);
    ~StatusBar() override;

public slots:
    void refresh(QString localAddr, QList<int> ports, QList<QString> stats);

private:
    void updatePortStatus(QString localAddr, int socks5Port, int httpPort, int pacPort);
    void updateSpeedLabels(QString down, QString up, QString downTotal, QString upTotal);

    QPushButton *m_dlSpeedLbl;
    QPushButton *m_upSpeedLbl;
    QLabel *m_SOCKS5Lbl;
    QLabel *m_HTTPLbl;
    QLabel *m_PACLbl;
};

#endif // STATUSBAR_H
