/*
 * Copyright 2020 Aaron Dewes <aaron.dewes@web.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301  USA
 */

#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <qdialog.h>
#include "ui_aboutdialog.h"

class AboutDialog : public QDialog, private Ui_AboutDialog
{
    Q_OBJECT

public:
    AboutDialog(QWidget *parent = nullptr);

private slots:
    void authorsButtonClicked();
    void licenseButtonClicked();

private:
    void displayFile(const QString &fileName, const QString &title);
};

#endif // ABOUTDIALOG_H
