/**
 * Copyright (c) 2010, William C. Witt
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Arora nor the names of its contributors
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

#include "useragentmenu.h"

#include "useragentmenuaction.h"
#include <QtXml>

UserAgentMenu::UserAgentMenu(QWidget *parent)
    : QMenu(parent)
{
    buildUserAgentMenu();
}

void UserAgentMenu::buildUserAgentMenu()
{
    DefaultUA *defaultUa = new DefaultUA(this);
    defaultUa->setText(tr("Default"));
    connect(defaultUa, SIGNAL(triggered()), defaultUa, SLOT(onAction()));
    addAction(defaultUa);

    addSeparator();

    QDomDocument doc(tr("useragentswitcher"));
    QFile file(tr(":/useragents/useragents.xml"));
    if (!doc.setContent(&file)) {
        file.close();
    }
    file.close();
    QDomElement root = doc.documentElement();
    QDomNode n = root.firstChild();
    while (!n.isNull()) {
        QDomElement e = n.toElement();
        if (!e.isNull()) {
            if (e.tagName() == tr("useragent")) {
                UserAgent ua(e.attribute(tr("description"), tr("")), e.attribute(tr("useragent"),tr("")));
                UserAgentMenuAction *uama = new UserAgentMenuAction(this);
                uama->setUserAgent(ua);
                connect(uama, SIGNAL(triggered()), uama, SLOT(onAction()));
                addAction(uama);
            }
        }
        n = n.nextSibling();
    }
}


DefaultUA::DefaultUA(QObject *parent)
    : QAction(parent)
{
}

void DefaultUA::onAction()
{
    QSettings setting;
    setting.remove(QLatin1String("useragent"));
}
