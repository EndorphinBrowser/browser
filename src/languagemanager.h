/*
 * Copyright (c) 2008, Diego Iastrubni, elcuco, at, kde.org
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

#ifndef LANGUAGE_MANAGER_H
#define LANGUAGE_MANAGER_H

#include <qstring.h>
#include <qhash.h>
#include <qlocale.h>
#include <qobject.h>

// this class inherits QObject for "garbage collector" reasons
// no need to define it as a Q_OBJECT :)
class LanguageManager: public QObject
{
    Q_OBJECT
public:
    LanguageManager(QObject *parent = 0);
    ~LanguageManager();

    void setCurrentLanguage( const QString &name );
    QString currentLanguage();
    void loadUpAvailableLangs();
    bool isLanguageAvailable( const QString &lang ) const;

public slots:
    bool getLanguageFromUser();

private:
    QHash<QString,QLocale> m_langs;
    QString m_currentLang;
};

#endif //LANGUAGE_MANAGER_H

// kate: space-indent on; tab-indent off; tab-width 4; indent-width 4; mixedindent off; indent-mode cstyle;
// kate: syntax: c++; auto-brackets on; auto-insert-doxygen: on; end-of-line: unix; show-tabs: on;
