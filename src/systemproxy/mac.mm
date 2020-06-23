#include "mac.h"

#include <QJsonObject>
#include <QJsonArray>

#import <Foundation/Foundation.h>
#import <SystemConfiguration/SystemConfiguration.h>

using namespace std;

Mac::Mac()
{}

Mac::~Mac()
{}

QJsonArray Mac::networkServicesList() {
    QJsonArray results;

    SCPreferencesRef prefRef = SCPreferencesCreate(nil, CFSTR("Trojan-Qt5"), nil);
    NSDictionary *sets = (__bridge NSDictionary *)SCPreferencesGetValue(prefRef, kSCPrefNetworkServices);

    for (NSString *key in [sets allKeys]) {
        NSMutableDictionary *service = [sets objectForKey:key];
        NSString *userDefinedName = [service valueForKey:(__bridge NSString *)kSCPropUserDefinedName];

        BOOL isActive = ![service objectForKey:(NSString *)kSCResvInactive];
        if (isActive) {
            if (isActive && userDefinedName) {
                std::string _key = [key UTF8String];
                std::string _userDefinedName = [userDefinedName UTF8String];
                QJsonObject v;
                v["key"] = QString::fromStdString(_key);
                v["userDefinedName"] = QString::fromStdString(_userDefinedName);
                results.append(v);
            }
        }
    }

    return results;
}
