/*
 * Copyright 2008 Benjamin C. Meyer <ben@meyerhome.net>
 * Copyright 2009 Jakub Wieczorek <faw217@gmail.com>
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
#include "browsermainwindow.h"
#include "networkaccessmanager.h"
#include "opensearchengine.h"
#include "opensearchdialog.h"
#include "opensearchmanager.h"
#include "searchbutton.h"
#include "tabwidget.h"

#include <qabstractitemview.h>
#include <qaction.h>
#include <qcompleter.h>
#include <qcoreapplication.h>
#include <qmenu.h>
#include <qsettings.h>
#include <qstandarditemmodel.h>
#include <qtimer.h>
#include <qurl.h>
#include <qwebsettings.h>

/*
    ToolbarSearch is a very basic search widget that also contains a small history.
    Searches are turned into urls that use Google to perform search
 */
ToolbarSearch::ToolbarSearch(QWidget *parent)
    : SearchLineEdit(parent)
    , m_openSearchManager(0)
    , m_suggestionsEnabled(true)
    , m_autosaver(new AutoSaver(this))
    , m_maxSavedSearches(10)
    , m_model(new QStandardItemModel(this))
    , m_suggestionsItem(0)
    , m_recentSearchesItem(0)
    , m_suggestTimer(0)
    , m_completer(0)
{
    m_openSearchManager = BrowserApplication::instance()->openSearchManager();

    connect(m_openSearchManager, SIGNAL(currentChanged()),
            this, SLOT(currentEngineChanged()));

    m_completer = new QCompleter(this);
    m_completer->setModel(m_model);
    m_completer->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
    setCompleter(m_completer);

    searchButton()->setShowMenuTriangle(true);

    connect(searchButton(), SIGNAL(clicked()),
            this, SLOT(showEnginesMenu()));
    connect(this, SIGNAL(returnPressed()),
            this, SLOT(searchNow()));

    load();

    currentEngineChanged();
}

void ToolbarSearch::currentEngineChanged()
{
    if (m_suggestionsEnabled) {
        if (m_openSearchManager->engineExists(m_currentEngine)) {
            OpenSearchEngine *oldEngine = m_openSearchManager->engine(m_currentEngine);
            disconnect(oldEngine, SIGNAL(suggestions(const QStringList &)),
                       this, SLOT(newSuggestions(const QStringList &)));
            disconnect(oldEngine, SIGNAL(iconChanged()),
                       this, SLOT(engineIconChanged()));
        }

        OpenSearchEngine *newEngine = m_openSearchManager->current();
        connect(newEngine, SIGNAL(suggestions(const QStringList &)),
                this, SLOT(newSuggestions(const QStringList &)));
        connect(newEngine, SIGNAL(iconChanged()),
                this, SLOT(engineIconChanged()));
    }

    setInactiveText(m_openSearchManager->currentName());
    searchButton()->setImage(m_openSearchManager->current()->icon().toImage());
    m_currentEngine = m_openSearchManager->currentName();
}

void ToolbarSearch::engineIconChanged()
{
    searchButton()->setImage(m_openSearchManager->current()->icon().toImage());
}

void ToolbarSearch::completerActivated(const QModelIndex &index)
{
    if (completerHighlighted(index))
        searchNow();
}

bool ToolbarSearch::completerHighlighted(const QModelIndex &index)
{
    if (m_suggestionsItem && m_suggestionsItem->index().row() == index.row())
        return false;
    if (m_recentSearchesItem && m_recentSearchesItem->index().row() == index.row())
        return false;
    setText(index.data().toString());
    return true;
}

void ToolbarSearch::focusInEvent(QFocusEvent *event)
{
    SearchLineEdit::focusInEvent(event);

    // Every time we get a focus in event QLineEdit re-connects...
    disconnect(completer(), SIGNAL(activated(QString)),
               this, SLOT(setText(QString)));
    disconnect(completer(), SIGNAL(highlighted(QString)),
               this, SLOT(_q_completionHighlighted(QString)));

    // And every time it gets a focus out it disconnects everything from the completer to this :(
    // So we have to re-connect
    connect(completer(), SIGNAL(activated(const QModelIndex &)),
            this, SLOT(completerActivated(const QModelIndex &)));
    connect(completer(), SIGNAL(highlighted(const QModelIndex &)),
            this, SLOT(completerHighlighted(const QModelIndex &)));
}

ToolbarSearch::~ToolbarSearch()
{
    m_autosaver->saveIfNeccessary();
    delete m_completer;
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

    m_suggestionsEnabled = settings.value(QLatin1String("useSuggestions"), true).toBool();
    if (m_suggestionsEnabled) {
        connect(this, SIGNAL(textEdited(const QString &)),
                this, SLOT(textEdited(const QString &)));
    }

    settings.endGroup();
    setupList();
}

void ToolbarSearch::textEdited(const QString &text)
{
    Q_UNUSED(text);
    // delay settings this to prevent BrowserApplication from creating
    // the object when it isn't needed on startup
    if (!m_suggestTimer) {
        m_suggestTimer = new QTimer(this);
        m_suggestTimer->setSingleShot(true);
        m_suggestTimer->setInterval(200);
        connect(m_suggestTimer, SIGNAL(timeout()),
                this, SLOT(getSuggestions()));
    }
    m_suggestTimer->start();
}

void ToolbarSearch::getSuggestions()
{
    m_openSearchManager->current()->requestSuggestions(text());
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

    QUrl searchUrl = m_openSearchManager->current()->searchUrl(searchText);
    emit search(searchUrl, TabWidget::CurrentTab);
}

void ToolbarSearch::newSuggestions(const QStringList &suggestions)
{
    m_suggestions = suggestions;
    setupList();
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

void ToolbarSearch::showEnginesMenu()
{
    QMenu menu;

    QWidget *parent = searchButton()->parentWidget();
    if (!parent)
        return;

    QPoint pos = parent->mapToGlobal(QPoint(0, parent->height()));

    QList<QString> list = m_openSearchManager->nameList();
    for (int i = 0; i < list.count(); ++i) {
        QString name = list.at(i);
        QAction *action = menu.addAction(name, this, SLOT(changeCurrentEngine()));
        action->setData(name);
        action->setIcon(QIcon(m_openSearchManager->engine(name)->icon()));

        if (m_openSearchManager->currentName() == name) {
            action->setCheckable(true);
            action->setChecked(true);
        }
    }

    menu.addSeparator();
    QAction *showManager = menu.addAction(tr("Manage Search Engines..."));
    connect(showManager, SIGNAL(triggered()),
            this, SLOT(showDialog()));

    if (!m_recentSearches.empty())
        menu.addAction(tr("Clear Recent Searches"), this, SLOT(clear()));

    menu.exec(pos);
}

void ToolbarSearch::changeCurrentEngine()
{
    if (QAction *action = qobject_cast<QAction *>(sender())) {
        QString name = action->data().toString();
        m_openSearchManager->setCurrentName(name);
    }
}

void ToolbarSearch::showDialog()
{
    BrowserMainWindow *window = BrowserMainWindow::parentWindow(this);

    if (!window)
        return;

    OpenSearchDialog dialog(window);
    dialog.exec();
}

void ToolbarSearch::setupList()
{
    if (m_suggestions.isEmpty()
        || (m_model->rowCount() > 0
            && m_model->item(0) != m_suggestionsItem)) {
        m_model->clear();
        m_suggestionsItem = 0;
    } else {
        m_model->removeRows(1, m_model->rowCount() -1 );
    }

    QFont lightFont;
    lightFont.setWeight(QFont::Light);
    if (!m_suggestions.isEmpty()) {
        if (m_model->rowCount() == 0) {
            if (!m_suggestionsItem) {
                m_suggestionsItem = new QStandardItem();
                m_suggestionsItem->setFont(lightFont);
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
        m_recentSearchesItem = new QStandardItem(tr("No Recent Searches"));
        m_recentSearchesItem->setFont(lightFont);
        m_model->appendRow(m_recentSearchesItem);
    } else {
        m_recentSearchesItem = new QStandardItem(tr("Recent Searches"));
        m_recentSearchesItem->setFont(lightFont);
        m_model->appendRow(m_recentSearchesItem);
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
    setupList();
    QLineEdit::clear();
    clearFocus();
}

