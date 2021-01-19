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

#include "webviewsearch.h"

#include <QEvent>
#include <QShortcut>
#include <QTimeLine>

#include <QWebEngineView>

#include <QDebug>

WebViewSearch::WebViewSearch(QWebEngineView *webView, QWidget *parent)
    : SearchBar(parent)
{
    setSearchObject(webView);
}

void WebViewSearch::findNext()
{
    find(QWebEnginePage::FindFlags());
}

void WebViewSearch::findPrevious()
{
    find(QWebEnginePage::FindBackward);
}

void WebViewSearch::find(QWebEnginePage::FindFlags flags)
{
    if(!ui.searchLineEdit->text().isEmpty()) {
        QString searchString = ui.searchLineEdit->text();
        webView()->findText(searchString, flags, [this](bool found) {
            handleSearchResult(found);
        });
    }
}

void WebViewSearch::handleSearchResult(bool found)
{
    QString infoString;
    if (!found)
        infoString = tr("Not Found");
    
    ui.searchInfo->setText(infoString);
}

QWebEngineView *WebViewSearch::webView() const
{
    return qobject_cast<QWebEngineView*>(searchObject());
}

WebViewWithSearch::WebViewWithSearch(WebView *webView, QWidget *parent)
    : QWidget(parent)
    , m_webView(webView)
{
    m_webView->setParent(this);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    m_webViewSearch = new WebViewSearch(m_webView, this);
    layout->addWidget(m_webViewSearch);
    layout->addWidget(m_webView);
    setLayout(layout);
}

