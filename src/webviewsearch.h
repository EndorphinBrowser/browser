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

#ifndef WEBVIEWSEARCH_H
#define WEBVIEWSEARCH_H

#include <QtGui/QWidget>

#include <QtWebKit/QWebPage>

#include "ui_searchbanner.h";

QT_BEGIN_NAMESPACE
class QTimeLine;
class QWebView;
QT_END_NAMESPACE

class WebViewSearch : public QWidget
{
    Q_OBJECT

public:
    WebViewSearch(QWidget *parent = 0);

    void setWebView(QWebView *webView);
    QWebView *webView() const;

public slots:
    void animateHide();
    void clear();
    void showFind();
    void findNext();
    void findPrevious();

protected:
    void resizeEvent(QResizeEvent *event);
    void showEvent(QShowEvent *event);

private slots:
    void frameChanged(int frame);
    void find(QWebPage::FindFlags flags);

private:
    void initializeSearchWidget();

    Ui_SearchBanner ui;
    QWidget *m_widget;

    QWebView *m_webView;
    QTimeLine *m_timeLine;
};

#include "webview.h"

class WebViewWithSearch : public QWidget
{
    Q_OBJECT

public:
    WebViewWithSearch(WebView *webView, QWidget *parent = 0);
    WebView *m_webView;
    WebViewSearch *m_webViewSearch;
};


#endif // WEBVIEWSEARCH_H

