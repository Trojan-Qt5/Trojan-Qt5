/*
 * Copyright (C) 2019-2020 TheWanderingCoel <thewanderingcoel@protonmail.com>
 *
 * Trojan-Qt5 is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Trojan-Qt5 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Trojan-Qt5; see the file LICENSE. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef SSTHREAD_H
#define SSTHREAD_H

#include <QThread>

class SSThread: public QThread
{
public:
    SSThread(QString clientAddr,
             QString serverAddr,
             QString method,
             QString password,
             QString plugin,
             QString pluginParam,
             bool enableAPI,
             QString apiAddr);
    ~SSThread();

private:
    QString clientAddr;
    QString serverAddr;
    QString method;
    QString password;
    QString plugin;
    QString pluginParam;
    bool enableAPI;
    QString apiAddr;

protected:
  void run();

public slots:
  void stop();
};

#endif // SSTHREAD_H
