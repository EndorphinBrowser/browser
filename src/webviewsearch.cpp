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

#include "webviewsearch.h"

#include <qevent.h>
#include <qshortcut.h>
#include <qtimeline.h>

#include <qwebframe.h>
#include <qwebview.h>

#include <qdebug.h>

WebViewSearch::WebViewSearch(QWidget *parent)
    : SearchBar(parent)
{
    connect(ui.nextButton, SIGNAL(clicked()),
            this, SLOT(findNext()));
    connect(ui.previousButton, SIGNAL(clicked()),
            this, SLOT(findPrevious()));
    connect(ui.searchLineEdit, SIGNAL(returnPressed()),
            this, SLOT(findNext()));
    connect(ui.searchLineEdit, SIGNAL(textChanged(QString&)),
            this, SLOT(findNext()));
    connect(ui.doneButton, SIGNAL(clicked()),
            this, SLOT(animateHide()));
}

void WebViewSearch::setWebView(QWebView *webView)
{
    setObject(webView);
}

QWebView *WebViewSearch::webView() const
{
    return (QWebView*)getObject();
}

void WebViewSearch::findNext()
{
    find(QWebPage::FindWrapsAroundDocument);
}

void WebViewSearch::findPrevious()
{
    find(QWebPage::FindBackward | QWebPage::FindWrapsAroundDocument);
}

void WebViewSearch::find(QWebPage::FindFlags flags)
{
    QString searchString = ui.searchLineEdit->text();
    if (!m_object || searchString.isEmpty())
        return;
    QString infoString;
    if (!((QWebView*)m_object)->findText(searchString, flags))
        infoString = tr("Not Found");
    ui.searchInfo->setText(infoString);
}

WebViewWithSearch::WebViewWithSearch(WebView *webView, QWidget *parent)
    : QWidget(parent)
    , m_webView(webView)
{
    m_webView->setParent(this);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    m_webViewSearch = new WebViewSearch(this);
    layout->addWidget(m_webViewSearch);
    m_webViewSearch->setWebView(m_webView);
    layout->addWidget(m_webView);
    setLayout(layout);
}
