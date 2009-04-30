/**
 * Copyright (c) 2008 - 2009, Benjamin C. Meyer  <ben@meyerhome.net>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Benjamin Meyer nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef LINEEDIT_P_H
#define LINEEDIT_P_H

#include <qwidget.h>

class SideWidget : public QWidget
{
    Q_OBJECT

signals:
    void sizeHintChanged();

public:
    SideWidget(QWidget *parent = 0);

protected:
    bool event(QEvent *event);

};

#if QT_VERSION < 0x040500

#include "qapplication.h"
#include "proxystyle.h"
#include "lineedit.h"

class LineEditStyle : public ProxyStyle
{

public:
    explicit LineEditStyle()
        : ProxyStyle(QApplication::style()->objectName()) {
    }

    QRect subElementRect(SubElement element, const QStyleOption *option, const QWidget *widget = 0) const {
        QRect r = ProxyStyle::subElementRect(element, option, widget);
        if (element == SE_LineEditContents) {
            if (const LineEdit *le = qobject_cast<const LineEdit*>(widget)) {
                int left = le->textMargin(LineEdit::LeftSide);
                int right = le->textMargin(LineEdit::RightSide);
                r.adjust(left, 0, -right, 0);
            }
        }
        return r;
    }

};

#endif // QT_VERSION

#endif // LINEEDIT_P_H

