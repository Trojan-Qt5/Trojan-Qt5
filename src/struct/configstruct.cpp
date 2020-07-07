#include "configstruct.h"

#include <QDataStream>

QDataStream& operator << (QDataStream &out, const GeneralSettings &g)
{
    out << g.theme << g.systemTheme << g.toolBarStyle << g.language << g.logLevel << g.systemTrayMaximumServer << g.startAtLogin << g.hideWindowOnStartup << g.onlyOneInstace << g.checkPortAvailability << g.enableNotification << g.hideDockIcon << g.showToolbar << g.showFilterBar << g.nativeMenuBar << g.showAirportAndDonation;
    return out;
}

QDataStream& operator >> (QDataStream &in, GeneralSettings &g)
{
    in >> g.theme >> g.systemTheme >> g.toolBarStyle >> g.language >> g.logLevel >> g.systemTrayMaximumServer >> g.startAtLogin >> g.hideWindowOnStartup >> g.onlyOneInstace >> g.checkPortAvailability >> g.enableNotification >> g.hideDockIcon >> g.showToolbar >> g.showFilterBar >> g.nativeMenuBar >> g.showAirportAndDonation;
    return in;
}

QDataStream& operator << (QDataStream &out, const InboundSettings &i)
{
    out << i.enableHttpMode << i.shareOverLan << i.enableIpv6Support << i.inboundSniffing << i.socks5LocalPort << i.httpLocalPort << i.pacLocalPort << i.haproxyStatusPort << i.haproxyPort;
    return out;
}

QDataStream& operator >> (QDataStream &in, InboundSettings &i)
{
    in >> i.enableHttpMode >> i.shareOverLan >> i.enableIpv6Support >> i.inboundSniffing >> i.socks5LocalPort >> i.httpLocalPort >> i.pacLocalPort >> i.haproxyStatusPort >> i.haproxyPort;
    return in;
}

QDataStream& operator << (QDataStream &out, const OutboundSettings &o)
{
    out << o.bypassBittorrent << o.bypassPrivateAddress << o.bypassChinaMainland << o.forwardProxy << o.forwardProxyType << o.forwardProxyAddress << o.forwardProxyPort << o.forwardProxyAuthentication << o.forwardProxyUsername << o.forwardProxyPassword;
    return out;
}

QDataStream& operator >> (QDataStream &in, OutboundSettings &o)
{
    in >> o.bypassBittorrent >> o.bypassPrivateAddress >> o.bypassChinaMainland >> o.forwardProxy >> o.forwardProxyType >> o.forwardProxyAddress >> o.forwardProxyPort >> o.forwardProxyAuthentication >> o.forwardProxyUsername >> o.forwardProxyPassword;
    return in;
}

QDataStream& operator << (QDataStream &out, const TestSettings &t)
{
    out << t.method << t.latencyTestUrl << t.speedTestUrl;
    return out;
}

QDataStream& operator >> (QDataStream &in, TestSettings &t)
{
    in >> t.method >> t.latencyTestUrl >> t.speedTestUrl;
    return in;
}

QDataStream& operator << (QDataStream &out, const SubscribeSettings &s)
{
    out << s.gfwListUrl << s.updateUserAgent << s.filterKeyword << s.maximumSubscribe << s.autoFetchGroupName << s.overwriteAllowInsecure << s.overwriteAllowInsecureCiphers << s.overwriteTcpFastOpen;
    return out;
}

QDataStream& operator >> (QDataStream &in, SubscribeSettings &s)
{
    in >> s.gfwListUrl >> s.updateUserAgent >> s.filterKeyword >> s.maximumSubscribe >> s.autoFetchGroupName >> s.overwriteAllowInsecure >> s.overwriteAllowInsecureCiphers >> s.overwriteTcpFastOpen;
    return in;
}

QDataStream& operator << (QDataStream &out, const GraphSettings &g)
{
    out << g.detailOutboundProxy << g.detailOutboundDirect << g.proxyDownloadSpeedColor << g.proxyUploadSpeedColor << g.directDownloadSpeedColor << g.directUploadSpeedColor;
    return out;
}

QDataStream& operator >> (QDataStream &in, GraphSettings &g)
{
    in >> g.detailOutboundProxy >> g.detailOutboundDirect >> g.proxyDownloadSpeedColor >> g.proxyUploadSpeedColor >> g.directDownloadSpeedColor >> g.directUploadSpeedColor;
    return in;
}

QDataStream& operator << (QDataStream &out, const RouterSettings &r)
{
    out << r.domainStrategy << r.domainDirect << r.domainProxy << r.domainBlock << r.ipDirect << r.ipProxy << r.ipBlock;
    return out;
}

QDataStream& operator >> (QDataStream &in, RouterSettings &r)
{
    in >> r.domainStrategy >> r.domainDirect >> r.domainProxy >> r.domainBlock >> r.ipDirect >> r.ipProxy >> r.ipBlock;
    return in;
}

QDataStream& operator << (QDataStream &out, const CoreSettings &c)
{
    out << c.fingerprint << c.enableAPI << c.enableRouter << c.countInboundTraffic << c.countOutboundTraffic << c.geoPath << c.apiPort << c.trojanCertPath << c.trojanCipher << c.trojanCipherTLS13 << c.bufferSize;
    return out;
}

QDataStream& operator >> (QDataStream &in, CoreSettings &c)
{
    in >> c.fingerprint >> c.enableAPI >> c.enableRouter >> c.countInboundTraffic >> c.countOutboundTraffic >> c.geoPath >> c.apiPort >> c.trojanCertPath >> c.trojanCipher >> c.trojanCipherTLS13 >> c.bufferSize;
    return in;
}
