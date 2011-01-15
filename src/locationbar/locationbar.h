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

#ifndef LOCATIONBAR_H
#define LOCATIONBAR_H

#include "lineedit.h"

#include <qpointer.h>
#include <QUrl>

class WebView;
class LocationBarSiteIcon;
class PrivacyIndicator;
class LocationBar : public LineEdit
{
    Q_OBJECT

public:
    LocationBar(QWidget *parent = nullptr);
    void setWebView(WebView *webView);
    WebView *webView() const;
    static void resetFirstSelectAll();

protected:
    void paintEvent(QPaintEvent *event);
    void focusOutEvent(QFocusEvent *event);
    void mouseReleaseEvent (QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

private slots:
    void webViewUrlChanged(const QUrl &url);

private:
    /**
     * Guard to imitate Chrome select all on first click
     */
    static bool s_firstSelectAll;
    QPointer<WebView> m_webView;

    LocationBarSiteIcon *m_siteIcon;
    PrivacyIndicator *m_privacyIndicator;
};

#endif // LOCATIONBAR_H

