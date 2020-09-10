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

#include <qevent.h>
#include <qshortcut.h>
#include <qtimeline.h>

#include <qwebengineview.h>

#include <qdebug.h>

WebViewSearch::WebViewSearch(QWebEngineView *webView, QWidget *parent)
    : SearchBar(parent)
{
    setSearchObject(webView);
    ui.highlightAllButton->setVisible(true);
    connect(ui.highlightAllButton, SIGNAL(toggled(bool)),
            this, SLOT(highlightAll()));
    connect(ui.searchLineEdit, SIGNAL(textEdited(const QString &)),
            this, SLOT(highlightAll()));
}

void WebViewSearch::findNext()
{
    find(nullptr);
}

void WebViewSearch::findPrevious()
{
    find(QWebEnginePage::FindBackward);
}

void WebViewSearch::highlightAll()
{
    webView()->findText(QString(), nullptr);

/*
    if (ui.highlightAllButton->isChecked())
        find(QWebEnginePage::HighlightAllOccurrences);
*/
}

void WebViewSearch::find(QWebEnginePage::FindFlags flags)
{
    QString searchString = ui.searchLineEdit->text();
    if (!searchObject() || searchString.isEmpty())
        return;
    QString infoString;
    //if (!webView()->findText(searchString, flags))
        //infoString = tr("Not Found");
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

