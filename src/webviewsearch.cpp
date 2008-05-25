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
    : QWidget(parent)
    , m_widget(0)
    , m_webView(0)
    , m_timeLine(new QTimeLine(150, this))
{
    initializeSearchWidget();

    // we start off hidden
    setMaximumHeight(0);
    m_widget->setGeometry(0, -1 * m_widget->height(),
                          m_widget->width(), m_widget->height());
    hide();

    connect(m_timeLine, SIGNAL(frameChanged(int)),
            this, SLOT(frameChanged(int)));

    new QShortcut(QKeySequence(Qt::Key_Escape), this, SLOT(animateHide()));
}

void WebViewSearch::initializeSearchWidget()
{
    m_widget = new QWidget(this);
    m_widget->setContentsMargins(0, 0, 0, 0);
    ui.setupUi(m_widget);
    ui.previousButton->setText(QChar(9664));
    ui.nextButton->setText(QChar(9654));
    ui.searchInfo->setText(QString());
    connect(ui.nextButton, SIGNAL(clicked()),
            this, SLOT(findNext()));
    connect(ui.previousButton, SIGNAL(clicked()),
            this, SLOT(findPrevious()));
    connect(ui.searchLineEdit->lineEdit(), SIGNAL(returnPressed()),
            this, SLOT(findNext()));
    connect(ui.doneButton, SIGNAL(clicked()),
            this, SLOT(animateHide()));

    setMinimumWidth(m_widget->minimumWidth());
    setMaximumWidth(m_widget->maximumWidth());
    setMinimumHeight(m_widget->minimumHeight());
}

void WebViewSearch::setWebView(QWebView *webView)
{
    m_webView = webView;
}

QWebView *WebViewSearch::webView() const
{
    return m_webView;
}

void WebViewSearch::clear()
{
    ui.searchLineEdit->lineEdit()->setText(QString());
}

void WebViewSearch::showFind()
{
    if (isVisible())
        return;

    show();
    ui.searchLineEdit->setFocus();
    ui.searchLineEdit->lineEdit()->selectAll();

    m_timeLine->setFrameRange(-1 * m_widget->height(), 0);
    m_timeLine->setDirection(QTimeLine::Forward);
    disconnect(m_timeLine, SIGNAL(finished()),
               this, SLOT(hide()));
    m_timeLine->start();
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
    QString searchString = ui.searchLineEdit->lineEdit()->text();
    if (!m_webView || searchString.isEmpty())
        return;
    QString infoString;
    if (!m_webView->findText(searchString, flags))
        infoString = tr("Not Found");
    ui.searchInfo->setText(infoString);
}

void WebViewSearch::resizeEvent(QResizeEvent *event)
{
    if (event->size().width() != m_widget->width())
        m_widget->resize(event->size().width(), m_widget->height());
    QWidget::resizeEvent(event);
}

void WebViewSearch::animateHide()
{
    m_timeLine->setDirection(QTimeLine::Backward);
    m_timeLine->start();
    connect(m_timeLine, SIGNAL(finished()), this, SLOT(hide()));
}

void WebViewSearch::frameChanged(int frame)
{
    if (!m_widget)
        return;
    m_widget->move(0, frame);
    int height = qMax(0, m_widget->y() + m_widget->height());
    setMinimumHeight(height);
    setMaximumHeight(height);
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
