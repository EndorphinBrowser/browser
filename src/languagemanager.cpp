/*
 * Copyright 2008 Diego Iastrubni, elcuco, at, kde.org
 * Copyright 2008 Benjamin C. Meyer <ben@meyerhome.net>
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

#include "languagemanager.h"

#include "browserapplication.h"

#include <qapplication.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qinputdialog.h>
#include <qlibraryinfo.h>
#include <qlocale.h>
#include <qsettings.h>
#include <qtranslator.h>

#include <qdebug.h>

LanguageManager::LanguageManager(QObject *parent)
    : QObject(parent)
    , m_sysTranslator(0)
    , m_appTranslator(0)
    , m_loaded(false)
{
    QSettings settings;
    settings.beginGroup(QLatin1String("LanguageManager"));
    if (settings.contains(QLatin1String("language"))) {
        setCurrentLanguage(settings.value(QLatin1String("language")).toString());
    } else if (!currentLanguage().isEmpty()) {
        setCurrentLanguage(currentLanguage());
    }
}

QString LanguageManager::currentLanguage() const
{
    if (!m_currentLanguage.isEmpty())
        return m_currentLanguage;

    const QString sysLanguage = QLocale::system().name();
    if (isLanguageAvailable(sysLanguage))
        return sysLanguage;
    return QString();
}

bool LanguageManager::isLanguageAvailable(const QString &language) const
{
    loadAvailableLanguages();
    return language.isEmpty() || m_languages.contains(language);
}

void LanguageManager::setCurrentLanguage(const QString &language)
{
    if (m_currentLanguage == language
        || !isLanguageAvailable(language))
        return;

    m_currentLanguage = language;

    QSettings settings;
    settings.beginGroup(QLatin1String("LanguageManager"));
    settings.setValue(QLatin1String("language"), m_currentLanguage);

    if (m_currentLanguage.isEmpty()) {
        delete m_appTranslator;
        delete m_sysTranslator;
        m_appTranslator = 0;
        m_sysTranslator = 0;
        return;
    }

    QTranslator *newAppTranslator = new QTranslator(this);
    QString resourceDir = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
    bool loaded = newAppTranslator->load(m_currentLanguage, translationLocation());

    QTranslator *newSysTranslator = new QTranslator(this);
    QString translatorFileName = QLatin1String("qt_") + m_currentLanguage;
    if (!newSysTranslator->load(translatorFileName, resourceDir)) {
        delete newSysTranslator;
        newSysTranslator = 0;
    }

    if (!loaded) {
        qWarning() << "Failed to load translation:" << currentLanguage();
        delete newAppTranslator;
        delete newSysTranslator;
        return;
    }

    // A new language event is sent to all widgets in the application
    // They need to catch it and re-translate
    delete m_appTranslator;
    delete m_sysTranslator;
    qApp->installTranslator(newAppTranslator);
    qApp->installTranslator(newSysTranslator);
    m_appTranslator = newAppTranslator;
    m_sysTranslator = newSysTranslator;
}

QStringList LanguageManager::languages() const
{
    loadAvailableLanguages();
    return m_languages;
}

void LanguageManager::chooseNewLanguage()
{
    loadAvailableLanguages();

    QStringList items;
    QString systemLocaleString = QLocale::system().name();
    systemLocaleString = tr("System locale (%1) %2")
        .arg(systemLocaleString)
        // this is for pretty RTL support
        .arg(QChar(0x200E)); // LRM = 0x200E;
    items << systemLocaleString;

    int defaultItem = 0;
    foreach (QString name, m_languages) {
        QLocale locale(name);
        if (name == m_currentLanguage)
            defaultItem = items.count();
        QString string = QString(QLatin1String("%1, %2 (%3) %4"))
            .arg(QLocale::languageToString(locale.language()))
            .arg(QLocale::countryToString(locale.country()))
            .arg(name)
            // this is for pretty RTL support
            .arg(QChar(0x200E)); // LRM = 0x200E
        items << string;
    }

    bool ok;
    QString item = QInputDialog::getItem(0,
        tr("Choose language"),
        tr("<p>You can run with a different language than<br>"
        "the operating system default.</p>"
        "<p>Please choose the language which should be used</p>"),
        items, defaultItem, false, &ok);
    if (!ok)
        return;

    int selection = items.indexOf(item);
    if (selection == 0) {
        setCurrentLanguage(QString());
    } else {
        setCurrentLanguage(m_languages.value(selection - 1));
    }
}

QString LanguageManager::translationLocation() const
{
    QString directory = BrowserApplication::dataDirectory() + QLatin1Char('/') + QLatin1String("locale");
    // work without installing
    if (!QFile::exists(directory))
        directory = QLatin1String(".qm/locale");
    return directory;
}

/*!
    Find all *.qm files in the data directory that have a Qt translation.
 */
void LanguageManager::loadAvailableLanguages() const
{
    if (m_loaded)
        return;
    m_loaded = true;
    QDir appLangsDir(translationLocation());
    QStringList nameFilters(QLatin1String("*.qm"));
    QFileInfoList list = appLangsDir.entryInfoList(nameFilters);

    QString sysLangsDirName = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
    sysLangsDirName += QLatin1Char('/') + QLatin1String("qt_");
    for (int i = 0; i < list.size(); ++i) {
        QFileInfo appFileInfo = list.at(i);
        QString language = appFileInfo.completeBaseName();
        QFileInfo sysFileInfo(sysLangsDirName + language + QLatin1String(".qm") );
        if (!sysFileInfo.exists()) {
            //continue;
        }
        m_languages.append(language);
    }
}

