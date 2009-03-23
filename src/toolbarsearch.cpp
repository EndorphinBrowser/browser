/*
 * Copyright 2008 Benjamin C. Meyer <ben@meyerhome.net>
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

/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License versions 2.0 or 3.0 as published by the Free Software
** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file.  Alternatively you may (at
** your option) use any later version of the GNU General Public
** License if such license has been publicly approved by Trolltech ASA
** (or its successors, if any) and the KDE Free Qt Foundation. In
** addition, as a special exception, Trolltech gives you certain
** additional rights. These rights are described in the Trolltech GPL
** Exception version 1.2, which can be found at
** http://www.trolltech.com/products/qt/gplexception/ and in the file
** GPL_EXCEPTION.txt in this package.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/. If
** you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** In addition, as a special exception, Trolltech, as the sole
** copyright holder for Qt Designer, grants users of the Qt/Eclipse
** Integration plug-in the right for the Qt/Eclipse Integration to
** link to functionality provided by Qt Designer and its related
** libraries.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not expressly
** granted herein.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "toolbarsearch.h"

#include "autosaver.h"
#include "browserapplication.h"
#include "googlesuggest.h"
#include "networkaccessmanager.h"

#include <qabstractitemview.h>
#include <qcompleter.h>
#include <qcoreapplication.h>
#include <qsettings.h>
#include <qstandarditemmodel.h>
#include <qurl.h>
#include <qwebsettings.h>

/*
    ToolbarSearch is a very basic search widget that also contains a small history.
    Searches are turned into urls that use Google to perform search
 */
ToolbarSearch::ToolbarSearch(QWidget *parent)
    : SearchLineEdit(new QCompleter(parent), parent)
    , m_autosaver(new AutoSaver(this))
    , m_maxSavedSearches(10)
    , m_model(new QStandardItemModel(this))
    , m_suggestionsItem(0)
{
    completer()->setModel(m_model);
    completer()->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
    connect(completer(), SIGNAL(activated(const QString &)),
            this, SLOT(searchNow()));

    connect(this, SIGNAL(returnPressed()), SLOT(searchNow()));
    setInactiveText(QLatin1String("Google"));

    load();
}

ToolbarSearch::~ToolbarSearch()
{
    m_autosaver->saveIfNeccessary();
}

void ToolbarSearch::save()
{
    QSettings settings;
    settings.beginGroup(QLatin1String("toolbarsearch"));
    settings.setValue(QLatin1String("recentSearches"), m_recentSearches);
    settings.setValue(QLatin1String("maximumSaved"), m_maxSavedSearches);
    settings.endGroup();
}

void ToolbarSearch::load()
{
    QSettings settings;
    settings.beginGroup(QLatin1String("toolbarsearch"));
    m_recentSearches = settings.value(QLatin1String("recentSearches")).toStringList();
    m_maxSavedSearches = settings.value(QLatin1String("maximumSaved"), m_maxSavedSearches).toInt();
    bool useGoogleSuggest = settings.value(QLatin1String("useGoogleSuggest"), true).toBool();
    if (useGoogleSuggest) {
        m_googleSuggest = new GoogleSuggest(this);
        connect(m_googleSuggest, SIGNAL(suggestions(const QStringList &, const QString &)),
                this, SLOT(newSuggestions(const QStringList &)));
        connect(this, SIGNAL(textChanged(const QString &)),
                this, SLOT(textChanged(const QString &)));
    }
    settings.endGroup();
    setupMenu();
}

void ToolbarSearch::textChanged(const QString &text)
{
    // delay settings this to prevent BrowserApplication from creating
    // the object when it isn't needed on startup
    if (!m_googleSuggest->networkAccessManager())
        m_googleSuggest->setNetworkAccessManager(BrowserApplication::networkAccessManager());
    m_googleSuggest->suggest(text);
}

void ToolbarSearch::searchNow()
{
    QString searchText = text();
    QStringList newList = m_recentSearches;
    if (newList.contains(searchText))
        newList.removeAt(newList.indexOf(searchText));
    newList.prepend(searchText);
    if (newList.size() >= m_maxSavedSearches)
        newList.removeLast();

    QWebSettings *globalSettings = QWebSettings::globalSettings();
    if (!globalSettings->testAttribute(QWebSettings::PrivateBrowsingEnabled)) {
        m_recentSearches = newList;
        m_autosaver->changeOccurred();
    }

    QUrl url(QLatin1String("http://www.google.com/search"));
    url.addEncodedQueryItem(QUrl::toPercentEncoding(QLatin1String("q")),
            QUrl::toPercentEncoding(searchText));
    url.addQueryItem(QLatin1String("ie"), QLatin1String("UTF-8"));
    url.addQueryItem(QLatin1String("oe"), QLatin1String("UTF-8"));
    url.addQueryItem(QLatin1String("client"), QCoreApplication::applicationName());
    emit search(url);
}

void ToolbarSearch::newSuggestions(const QStringList &suggestions)
{
    m_suggestions = suggestions;
    setupMenu();
}

void ToolbarSearch::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
        retranslate();
    SearchLineEdit::changeEvent(event);
}

void ToolbarSearch::retranslate()
{
    if (m_suggestionsItem)
        m_suggestionsItem->setText(tr("Suggestions"));
}

void ToolbarSearch::setupMenu()
{
    if (m_suggestions.isEmpty()
        || (m_model->rowCount() > 0
            && m_model->item(0) != m_suggestionsItem)) {
        m_model->clear();
        m_suggestionsItem = 0;
    } else {
        m_model->removeRows(1, m_model->rowCount() -1 );
    }

    if (!m_suggestions.isEmpty()) {
        if (m_model->rowCount() == 0) {
            if (!m_suggestionsItem) {
                m_suggestionsItem = new QStandardItem();
                m_suggestionsItem->setEnabled(false);
                retranslate();
            }
            m_model->appendRow(m_suggestionsItem);
        }
        for (int i = 0; i < m_suggestions.count(); ++i) {
            const QString &text = m_suggestions.at(i);
            m_model->appendRow(new QStandardItem(text));
        }
    }

    if (m_recentSearches.isEmpty()) {
        QStandardItem *item = new QStandardItem(tr("No Recent Searches"));
        item->setEnabled(false);
        m_model->appendRow(item);
    } else {
        QStandardItem *recentSearchesItem = new QStandardItem(tr("Recent Searches"));
        recentSearchesItem->setEnabled(false);
        m_model->appendRow(recentSearchesItem);
        for (int i = 0; i < m_recentSearches.count(); ++i) {
            QString text = m_recentSearches.at(i);
            m_model->appendRow(new QStandardItem(text));
        }
    }

    QAbstractItemView *view = completer()->popup();
    view->setFixedHeight(view->sizeHintForRow(0) * m_model->rowCount() + view->frameWidth() * 2);
}

void ToolbarSearch::clear()
{
    m_recentSearches.clear();
    m_autosaver->changeOccurred();
    QLineEdit::clear();
    clearFocus();
}

