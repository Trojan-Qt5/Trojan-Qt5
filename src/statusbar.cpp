#include "statusbar.h"
#include "utils.h"

#include <QApplication>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>

StatusBar::StatusBar(QWidget *parent)
    : QStatusBar(parent)
{

    QWidget *container = new QWidget(this);
    auto *layout = new QHBoxLayout(container);
    layout->setContentsMargins(0,0,0,0);

    container->setLayout(layout);

    m_dlSpeedLbl = new QPushButton(this);
    m_dlSpeedLbl->setIcon(QIcon(":/icons/icons/download.svg"));
    m_dlSpeedLbl->setFlat(true);
    m_dlSpeedLbl->setFocusPolicy(Qt::NoFocus);
    m_dlSpeedLbl->setCursor(Qt::PointingHandCursor);
    m_dlSpeedLbl->setStyleSheet("text-align:left;");
    m_dlSpeedLbl->setMinimumWidth(200);

    m_upSpeedLbl = new QPushButton(this);
    m_upSpeedLbl->setIcon(QIcon(":/icons/icons/upload.svg"));
    m_upSpeedLbl->setFlat(true);
    m_upSpeedLbl->setFocusPolicy(Qt::NoFocus);
    m_upSpeedLbl->setCursor(Qt::PointingHandCursor);
    m_upSpeedLbl->setStyleSheet("text-align:left;");
    m_upSpeedLbl->setMinimumWidth(200);

    m_SOCKS5Lbl = new QLabel(tr("SOCKS5   %1: %2").arg("127.0.0.1").arg("0"), this);
    m_SOCKS5Lbl->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    m_HTTPLbl = new QLabel(tr("HTTP   %1: %2").arg("127.0.0.1").arg("0"), this);
    m_HTTPLbl->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    m_PACLbl = new QLabel(tr("PAC   %1: %2").arg("127.0.0.1").arg("0"), this);
    m_PACLbl->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    // Because on some platforms the default icon size is bigger
    // and it will result in taller/fatter statusbar, even if the
    // icons are actually 16x16
    m_dlSpeedLbl->setIconSize(Utils::smallIconSize());
    m_upSpeedLbl->setIconSize(Utils::smallIconSize());

    QFrame *statusSep1 = new QFrame(this);
    statusSep1->setObjectName("statusFrame");
    statusSep1->setFrameStyle(QFrame::VLine);
#ifndef Q_OS_MACOS
    statusSep1->setFrameShadow(QFrame::Raised);
#endif
    QFrame *statusSep2 = new QFrame(this);
    statusSep2->setObjectName("statusFrame");
    statusSep2->setFrameStyle(QFrame::VLine);
#ifndef Q_OS_MACOS
    statusSep2->setFrameShadow(QFrame::Raised);
#endif
    QFrame *statusSep3 = new QFrame(this);
    statusSep3->setObjectName("statusFrame");
    statusSep3->setFrameStyle(QFrame::VLine);
#ifndef Q_OS_MACOS
    statusSep3->setFrameShadow(QFrame::Raised);
#endif
    QFrame *statusSep4 = new QFrame(this);
    statusSep4->setObjectName("statusFrame");
    statusSep4->setFrameStyle(QFrame::VLine);
#ifndef Q_OS_MACOS
    statusSep4->setFrameShadow(QFrame::Raised);
#endif

    layout->addWidget(m_SOCKS5Lbl);
    layout->addWidget(statusSep1);
    layout->addWidget(m_HTTPLbl);
    layout->addWidget(statusSep2);
    layout->addWidget(m_PACLbl);
    layout->addWidget(statusSep4);
    layout->addWidget(m_dlSpeedLbl);
    layout->addWidget(statusSep3);
    layout->addWidget(m_upSpeedLbl);

    this->setSizeGripEnabled(false);

    addPermanentWidget(container);
    setStyleSheet("QWidget {margin: 0;}");
    container->adjustSize();
    adjustSize();
}

StatusBar::~StatusBar()
{
}

void StatusBar::refresh(QString localAddr, QList<int> ports, QList<QString> stats)
{
    updatePortStatus(localAddr, ports[0], ports[1], ports[2]);
    updateSpeedLabels(stats[0], stats[1], stats[2], stats[3]);
}

void StatusBar::updatePortStatus(QString localAddr, int socks5Port, int httpPort, int pacPort)
{
    m_SOCKS5Lbl->setText(tr("SOCKS5   %1:%2").arg(localAddr).arg(QString::number(socks5Port)));
    m_HTTPLbl->setText(tr("HTTP   %1:%2").arg(localAddr).arg(QString::number(httpPort)));
    m_PACLbl->setText(tr("PAC   %1:%2").arg("127.0.0.1").arg(pacPort));
}

void StatusBar::updateSpeedLabels(QString down, QString up, QString downTotal, QString upTotal)
{
    QString dlSpeedLbl = down + "/s";
    dlSpeedLbl += " (" + downTotal + ')';
    m_dlSpeedLbl->setText(dlSpeedLbl);

    QString upSpeedLbl = up + "/s";
    upSpeedLbl += " (" + upTotal + ')';
    m_upSpeedLbl->setText(upSpeedLbl);
}

