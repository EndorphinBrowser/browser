/*
 * Copyright 2008 Jason A. Donenfeld <Jason@zx2c4.com>
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

#ifndef CLEARPRIVATEDATA_H
#define CLEARPRIVATEDATA_H

#include <qdialog.h>

class QCheckBox;

class ClearPrivateData : public QDialog
{
    Q_OBJECT

public:
    ClearPrivateData(QWidget *parent = 0);

public slots:
    virtual void accept();

private:
    QCheckBox *m_browsingHistory;
    QCheckBox *m_downloadHistory;
    QCheckBox *m_searchHistory;
    QCheckBox *m_cookies;
    QCheckBox *m_cache;
    QCheckBox *m_favIcons;
};

#endif // CLEARPRIVATEDATA_H

