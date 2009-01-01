/*
 * Copyright 2008 Christian Franke <cfchris6@ts2server.com>
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

#include "sourceviewer.h"

#include <qlayout.h>
#include <qmenubar.h>
#include <qnetworkcookie.h>
#include <qnetworkreply.h>
#include <qnetworkrequest.h>
#include <qplaintextedit.h>
#include <qshortcut.h>
#include <qwebframe.h>
#include <qwebpage.h>

#include "browserapplication.h"
#include "networkaccessmanager.h"
#include "plaintexteditsearch.h"
#include "sourcehighlighter.h"

SourceViewer::SourceViewer(const QString &source,
        const QString &title, const QUrl &url, QWidget *parent)
    : QDialog(parent)
    , m_edit(new QPlainTextEdit(tr("Loading..."),this))
    , m_highlighter(new SourceHighlighter(m_edit->document()))
    , m_plainTextEditSearch(new PlainTextEditSearch(m_edit, this))
    , layout(new QVBoxLayout(this))
    , m_menuBar(new QMenuBar(this))
    , m_editMenu(new QMenu(tr("&Edit"), m_menuBar))
    , m_findAction(new QAction(tr("&Find"), m_editMenu))
    , m_viewMenu(new QMenu(tr("&View"), m_menuBar))
    , m_setWrappingAction(new QAction(tr("&Wrap lines"), m_viewMenu))
{
    setWindowTitle(QString(tr("Source of Page ")).append(title));
    resize(640, 480);

    m_source = new QString(source);

    m_edit->setLineWrapMode(QPlainTextEdit::NoWrap);
    m_edit->setReadOnly(true);
    QFont mFont = m_edit->font();
    mFont.setFamily(QLatin1String("Monospace"));
    m_edit->setFont(mFont);
    m_edit->setLineWidth(0);
    m_edit->setFrameShape(QFrame::NoFrame);

    m_menuBar->addMenu(m_editMenu);
    m_editMenu->addAction(m_findAction);
    m_findAction->setShortcuts(QKeySequence::Find);
    connect(m_findAction, SIGNAL(triggered()),
            m_plainTextEditSearch, SLOT(showFind()));

    m_menuBar->addMenu(m_viewMenu);
    m_viewMenu->addAction(m_setWrappingAction);
    m_setWrappingAction->setShortcut(tr("Ctrl+W"));
    m_setWrappingAction->setCheckable(true);
    connect(m_setWrappingAction, SIGNAL(triggered(bool)),
            this, SLOT(setWrapping(bool)));

    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_menuBar);
    layout->addWidget(m_plainTextEditSearch);
    layout->addWidget(m_edit);
    setLayout(layout);

    m_request = new QNetworkRequest(url);
    m_request->setAttribute(QNetworkRequest::CacheLoadControlAttribute,
            QNetworkRequest::PreferCache);
    m_reply = BrowserApplication::networkAccessManager()->get(*m_request);
    connect(m_reply, SIGNAL(finished()), this, SLOT(loadingFinished()));
    m_reply->setParent(this);
}

void SourceViewer::loadingFinished()
{
    QWebPage page;
    QByteArray response = m_reply->readAll();
    page.mainFrame()->setContent(response,QString(),m_request->url());
    /* If original request was POST or a different problem is there, fall
       back to modified version of QWebFrame.toHtml() */
    if (page.mainFrame()->toHtml() != *m_source)
        m_edit->setPlainText(*m_source);
    else m_edit->setPlainText(QLatin1String(response));
    m_reply->close();
    delete m_request;
    delete m_source;
}

void SourceViewer::setWrapping(bool wrap)
{
    if (wrap)
        m_edit->setLineWrapMode(QPlainTextEdit::WidgetWidth);
    else
        m_edit->setLineWrapMode(QPlainTextEdit::NoWrap);
}
