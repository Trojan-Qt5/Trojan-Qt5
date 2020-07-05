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

#ifndef TROJANEDITDIALOG_H
#define TROJANEDITDIALOG_H

#include <QDialog>
#include "connection.h"
#include "trojangowidget.h"

namespace Ui {
class TrojanEditDialog;
}

class TrojanEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TrojanEditDialog(Connection *_connection, QWidget *parent = 0);
    ~TrojanEditDialog();

private:
    Ui::TrojanEditDialog *ui;
    Connection *connection;
    TrojanGoWidget *trojanGoWidget;

private slots:
    void save();
};

#endif // TROJANEDITDIALOG_H
