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

#include <QtGui/QtGui>
#include <QtWebKit/QtWebKit>

#include "webviewsearch.h"

class WebViewWSearch : public QWebView
{
    Q_OBJECT

public:
    WebViewWSearch(QWidget *parent = 0) : QWebView(parent)
    {
    }

protected:

void resizeEvent(QResizeEvent *event)
{
    int offset = event->size().height() - event->oldSize().height();
    int currentValue = page()->mainFrame()->scrollBarValue(Qt::Vertical);
    setUpdatesEnabled(false);
    page()->mainFrame()->setScrollBarValue(Qt::Vertical, currentValue - offset);
    setUpdatesEnabled(true);
    QWebView::resizeEvent(event);
}

public slots:
    void findText(const QString &text) {
        findText(text);
    }
};

int main(int argc, char **argv)
{
    QApplication application(argc, argv);
    QMainWindow mainWindow;

    WebViewSearch *webViewSearch = new WebViewSearch;

    QWidget *window = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(webViewSearch);
    WebViewWSearch *webView = new WebViewWSearch(&mainWindow);
    webView->load(QUrl("http://reddit.com"));
    webViewSearch->setWebView(webView);
    layout->addWidget(webView);
    window->setLayout(layout);
    mainWindow.setCentralWidget(window);
    mainWindow.show();

    QMenuBar bar;
    QMenu *editMenu = bar.addMenu(("&Edit"));
    QAction *m_find = editMenu->addAction(("&Find"));
    m_find->setShortcuts(QKeySequence::Find);
    QObject::connect(m_find, SIGNAL(triggered()), webViewSearch, SLOT(showFind()));
    new QShortcut(QKeySequence(Qt::Key_Slash), webViewSearch, SLOT(showFind()));

    return application.exec();
}

#include "main.moc"
