#ifndef CONFIGSTRUCT_H
#define CONFIGSTRUCT_H

#include <QObject>
#include <QColor>
#include <QVariant>

struct GeneralSettings {
    QString theme = "Fusion";
    int systemTheme = 2;
    int toolBarStyle = 3;
    QString language = "Follow System";
    int logLevel = 1;
    int systemTrayMaximumServer = 0;
    bool startAtLogin = false;
    bool hideWindowOnStartup = false;
    bool onlyOneInstace = false;
    bool checkPortAvailability = true;
    bool enableNotification = true;
    bool hideDockIcon = false;
    bool showToolbar = true;
    bool showFilterBar = true;
    bool nativeMenuBar = false;
    bool showAirportAndDonation = true;
    operator QVariant() const
    {
        return QVariant::fromValue(*this);
    }
};
Q_DECLARE_METATYPE(GeneralSettings)

struct InboundSettings {
    bool enableHttpMode = true;
    bool shareOverLan = false;
    bool enableIpv6Support = false;
    int socks5LocalPort = 51837;
    int httpLocalPort = 58591;
    int pacLocalPort = 54400;
    int haproxyStatusPort = 2080;
    int haproxyPort = 7777;
    operator QVariant() const
    {
        return QVariant::fromValue(*this);
    }
};
Q_DECLARE_METATYPE(InboundSettings)

struct OutboundSettings {
    bool forwardProxy = false;
    int forwardProxyType = 0;
    QString forwardProxyAddress = "127.0.0.1";
    int forwardProxyPort = 1086;
    bool forwardProxyAuthentication = false;
    QString forwardProxyUsername;
    QString forwardProxyPassword;
    operator QVariant() const
    {
        return QVariant::fromValue(*this);
    }
};
Q_DECLARE_METATYPE(OutboundSettings)

struct TestSettings {
    int method = 0;
    QString latencyTestUrl = "https://www.google.com/generate_204";
    QString speedTestUrl = "http://speedtest-sgp1.digitalocean.com/10mb.test";
    operator QVariant() const
    {
        return QVariant::fromValue(*this);
    }
};
Q_DECLARE_METATYPE(TestSettings)

struct SubscribeSettings {
    int gfwListUrl = 2;
    QString updateUserAgent = QString("Trojan-Qt5/%1").arg(APP_VERSION);
    QString filterKeyword;
    int maximumSubscribe = 0;
    bool autoFetchGroupName = true;
    bool overwriteAllowInsecure = false;
    bool overwriteAllowInsecureCiphers = false;
    bool overwriteTcpFastOpen = false;
    operator QVariant() const
    {
        return QVariant::fromValue(*this);
    }
};
Q_DECLARE_METATYPE(SubscribeSettings)

struct GraphSettings {
    QString downloadSpeedColor = QColor::fromRgb(134, 196, 63).name();
    QString uploadSpeedColor = QColor::fromRgb(50, 153, 255).name();
    operator QVariant() const
    {
        return QVariant::fromValue(*this);
    }
};
Q_DECLARE_METATYPE(GraphSettings)

struct RouterSettings {
    QString domainStrategy = "AsIs";
    QStringList domainDirect;
    QStringList domainProxy;
    QStringList domainBlock;
    QStringList ipDirect;
    QStringList ipProxy;
    QStringList ipBlock;
    operator QVariant() const
    {
        return QVariant::fromValue(*this);
    }
};
Q_DECLARE_METATYPE(RouterSettings)

struct TrojanSettings {
    int fingerprint = 2;
    bool enableTrojanAPI = true;
    bool enableTrojanRouter = false;
    QString geoPath;
    int trojanAPIPort = 57721;
    QString trojanCertPath;
    QString trojanCipher = "ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-RSA-AES128-GCM-SHA256:ECDHE-ECDSA-CHACHA20-POLY1305:ECDHE-RSA-CHACHA20-POLY1305:ECDHE-ECDSA-AES256-GCM-SHA384:ECDHE-RSA-AES256-GCM-SHA384:ECDHE-ECDSA-AES256-SHA:ECDHE-ECDSA-AES128-SHA:ECDHE-RSA-AES128-SHA:ECDHE-RSA-AES256-SHA:DHE-RSA-AES128-SHA:DHE-RSA-AES256-SHA:AES128-SHA:AES256-SHA:DES-CBC3-SHA";
    QString trojanCipherTLS13 = "TLS_AES_128_GCM_SHA256:TLS_CHACHA20_POLY1305_SHA256:TLS_AES_256_GCM_SHA384";
    int bufferSize = 32;
    operator QVariant() const
    {
        return QVariant::fromValue(*this);
    }
};
Q_DECLARE_METATYPE(TrojanSettings)

QDataStream& operator << (QDataStream &out, const GeneralSettings &g);
QDataStream& operator >> (QDataStream &in, GeneralSettings &g);

QDataStream& operator << (QDataStream &out, const InboundSettings &i);
QDataStream& operator >> (QDataStream &in, InboundSettings &i);

QDataStream& operator << (QDataStream &out, const OutboundSettings &o);
QDataStream& operator >> (QDataStream &in, OutboundSettings &o);

QDataStream& operator << (QDataStream &out, const TestSettings &t);
QDataStream& operator >> (QDataStream &in, TestSettings &t);

QDataStream& operator << (QDataStream &out, const SubscribeSettings &s);
QDataStream& operator >> (QDataStream &in, SubscribeSettings &s);

QDataStream& operator << (QDataStream &out, const GraphSettings &g);
QDataStream& operator >> (QDataStream &in, GraphSettings &g);

QDataStream& operator << (QDataStream &out, const RouterSettings &r);
QDataStream& operator >> (QDataStream &in, RouterSettings &r);

QDataStream& operator << (QDataStream &out, const TrojanSettings &t);
QDataStream& operator >> (QDataStream &in, TrojanSettings &t);

#endif // CONFIGSTRUCT_H
