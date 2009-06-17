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

#include "locationbar.h"

#include "browserapplication.h"
#include "clearbutton.h"
#include "locationbarsiteicon.h"
#include "privacyindicator.h"
#include "searchlineedit.h"
#include "webview.h"

#include <qdrag.h>
#include <qevent.h>
#include <qpainter.h>
#include <qstyleoption.h>

#include <qdebug.h>

LocationBar::LocationBar(QWidget *parent)
    : LineEdit(parent)
    , m_webView(0)
    , m_siteIcon(0)
    , m_privacyIndicator(0)
{
    // Urls are always LeftToRight
    setLayoutDirection(Qt::LeftToRight);

    setUpdatesEnabled(false);
    // site icon on the left
    m_siteIcon = new LocationBarSiteIcon(this);
    addWidget(m_siteIcon, LeftSide);

    // privacy indicator at rightmost position
    m_privacyIndicator = new PrivacyIndicator(this);
    addWidget(m_privacyIndicator, RightSide);

    // clear button on the right
    ClearButton *m_clearButton = new ClearButton(this);
    connect(m_clearButton, SIGNAL(clicked()),
            this, SLOT(clear()));
    connect(this, SIGNAL(textChanged(const QString&)),
            m_clearButton, SLOT(textChanged(const QString&)));
    addWidget(m_clearButton, RightSide);

    updateTextMargins();
    setUpdatesEnabled(true);

    m_defaultBaseColor = palette().color(QPalette::Base);

    QPalette p = palette();
    p.setColor(QPalette::Base, QColor(255, 255, 255, 100));
    setPalette(p);
}

void LocationBar::setWebView(WebView *webView)
{
    Q_ASSERT(webView);
    m_webView = webView;
    m_siteIcon->setWebView(webView);
    connect(webView, SIGNAL(urlChanged(const QUrl &)),
            this, SLOT(webViewUrlChanged(const QUrl &)));
    connect(webView, SIGNAL(loadProgress(int)),
            this, SLOT(update()));
}

WebView *LocationBar::webView() const
{
    return m_webView;
}

void LocationBar::webViewUrlChanged(const QUrl &url)
{
    if (hasFocus())
        return;
    setText(QString::fromUtf8(url.toEncoded()));
    setCursorPosition(0);
}

static QLinearGradient generateGradient(const QColor &top, const QColor &middle, int height)
{
    QLinearGradient gradient(0, 0, 0, height);
    gradient.setColorAt(0, top);
    gradient.setColorAt(0.15, middle.lighter(120));
    gradient.setColorAt(0.5, middle);
    gradient.setColorAt(0.85, middle.lighter(120));
    gradient.setColorAt(1, top);
    return gradient;
}

void LocationBar::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    QPalette p = palette();
    QColor backgroundColor = m_defaultBaseColor;
    if (m_webView && m_webView->url().scheme() == QLatin1String("https")
        && p.color(QPalette::Text).value() < 128) {
        QColor lightYellow(248, 248, 210);
        backgroundColor = lightYellow;
    }

    // paint the text background
    QStyleOptionFrameV2 panel;
    initStyleOption(&panel);
    QRect backgroundRect = style()->subElementRect(QStyle::SE_LineEditContents, &panel, this);
#if QT_VERSION < 0x040500
    int left = textMargin(LineEdit::LeftSide);
    int right = textMargin(LineEdit::RightSide);
    backgroundRect.adjust(-left, 0, right, 0);
#endif
    painter.setBrush(backgroundColor);
    painter.setPen(backgroundColor);
    painter.drawRect(backgroundRect);

    // paint the progressbar
    if (m_webView && !hasFocus()) {
        int progress = m_webView->progress();
        QColor loadingColor = QColor(116, 192, 250);
        if (p.color(QPalette::Text).value() >= 128)
            loadingColor = m_defaultBaseColor.darker(200);

        painter.setBrush(generateGradient(m_defaultBaseColor, loadingColor, height()));
        painter.setPen(Qt::transparent);

        int mid = backgroundRect.width() * progress / 100;
        QRect progressRect = QRect(backgroundRect.x(), backgroundRect.y(), mid, backgroundRect.height());
        painter.drawRect(progressRect);
    }
    painter.end();

    LineEdit::paintEvent(event);
}

void LocationBar::focusOutEvent(QFocusEvent *event)
{
    if (text().isEmpty() && m_webView)
        webViewUrlChanged(m_webView->url());
    QLineEdit::focusOutEvent(event);
}

void LocationBar::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        selectAll();
    else
        QLineEdit::mouseDoubleClickEvent(event);
}

void LocationBar::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape && m_webView) {
        setText(QString::fromUtf8(m_webView->url().toEncoded()));
        selectAll();
    } else {
        QLineEdit::keyPressEvent(event);
    }
}

void LocationBar::dragEnterEvent(QDragEnterEvent *event)
{
    const QMimeData *mimeData = event->mimeData();
    if (mimeData->hasUrls() || mimeData->hasText())
        event->acceptProposedAction();

    LineEdit::dragEnterEvent(event);
}

void LocationBar::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();

    QUrl url;
    if (mimeData->hasUrls())
        url = mimeData->urls().at(0);
    else if (mimeData->hasText())
        url = QUrl::fromEncoded(mimeData->text().toUtf8(), QUrl::TolerantMode);

    if (url.isEmpty() || !url.isValid()) {
        LineEdit::dropEvent(event);
        return;
    }

    setText(QString::fromUtf8(url.toEncoded()));
    selectAll();

    event->acceptProposedAction();
}
