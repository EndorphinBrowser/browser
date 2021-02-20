/*
 * Copyright 2008-2009 Jason A. Donenfeld <Jason@zx2c4.com>
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

#include "clearprivatedata.h"

#include "browserapplication.h"
#include "browsermainwindow.h"
#include "downloadmanager.h"
#include "historymanager.h"
#include "toolbarsearch.h"

#include <QCheckBox>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLayout>
#include <QList>
#include <QPushButton>
#include <QSettings>
#include <QWebEngineSettings>

ClearPrivateData::ClearPrivateData(QWidget *parent)
    : QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint)
{
    setWindowTitle(tr("Clear Private Data"));

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(new QLabel(tr("Clear the following items:")));

    QSettings settings;
    settings.beginGroup(QStringLiteral("clearprivatedata"));

    m_browsingHistory = new QCheckBox(tr("&Browsing History"));
    m_browsingHistory->setChecked(settings.value(QStringLiteral("browsingHistory"), true).toBool());
    layout->addWidget(m_browsingHistory);

    m_downloadHistory = new QCheckBox(tr("&Download History"));
    m_downloadHistory->setChecked(settings.value(QStringLiteral("downloadHistory"), true).toBool());
    layout->addWidget(m_downloadHistory);

    m_searchHistory = new QCheckBox(tr("&Search History"));
    m_searchHistory->setChecked(settings.value(QStringLiteral("searchHistory"), true).toBool());
    layout->addWidget(m_searchHistory);

    settings.endGroup();

    QPushButton *acceptButton = new QPushButton(tr("Clear &Private Data"));
    acceptButton->setDefault(true);
    QPushButton *rejectButton = new QPushButton(tr("&Cancel"));
    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    buttonBox->addButton(acceptButton, QDialogButtonBox::AcceptRole);
    buttonBox->addButton(rejectButton, QDialogButtonBox::RejectRole);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    layout->addWidget(buttonBox);

    setLayout(layout);
    acceptButton->setFocus(Qt::OtherFocusReason);
}

void ClearPrivateData::accept()
{
    QSettings settings;
    settings.beginGroup(QStringLiteral("clearprivatedata"));

    settings.setValue(QStringLiteral("browsingHistory"), m_browsingHistory->isChecked());
    settings.setValue(QStringLiteral("downloadHistory"), m_downloadHistory->isChecked());
    settings.setValue(QStringLiteral("searchHistory"), m_searchHistory->isChecked());
    settings.setValue(QStringLiteral("cache"), m_cache->isChecked());

    settings.endGroup();

    if (m_browsingHistory->isChecked()) {
        BrowserApplication::historyManager()->clear();
    }

    if (m_downloadHistory->isChecked()) {
        BrowserApplication::downloadManager()->cleanup();
        BrowserApplication::downloadManager()->hide();
    }

    if (m_searchHistory->isChecked()) {
        QList<BrowserMainWindow*> mainWindows = BrowserApplication::instance()->mainWindows();
        for (int i = 0; i < mainWindows.count(); ++i) {
            mainWindows.at(i)->toolbarSearch()->setText(QString());
        }
    }
    QDialog::accept();
}
