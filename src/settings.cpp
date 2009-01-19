/*
 * Copyright 2008-2009 Benjamin C. Meyer <ben@meyerhome.net>
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

/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License versions 2.0 or 3.0 as published by the Free Software
** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file.  Alternatively you may (at
** your option) use any later version of the GNU General Public
** License if such license has been publicly approved by Trolltech ASA
** (or its successors, if any) and the KDE Free Qt Foundation. In
** addition, as a special exception, Trolltech gives you certain
** additional rights. These rights are described in the Trolltech GPL
** Exception version 1.2, which can be found at
** http://www.trolltech.com/products/qt/gplexception/ and in the file
** GPL_EXCEPTION.txt in this package.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/. If
** you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** In addition, as a special exception, Trolltech, as the sole
** copyright holder for Qt Designer, grants users of the Qt/Eclipse
** Integration plug-in the right for the Qt/Eclipse Integration to
** link to functionality provided by Qt Designer and its related
** libraries.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not expressly
** granted herein.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "settings.h"

#include "browserapplication.h"
#include "browsermainwindow.h"
#include "cookiedialog.h"
#include "cookieexceptionsdialog.h"
#include "cookiejar.h"
#include "history.h"
#include "networkaccessmanager.h"
#include "webview.h"

#include <qdesktopservices.h>
#include <qfile.h>
#include <qfontdialog.h>
#include <qmetaobject.h>
#include <qsettings.h>

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);
    connect(exceptionsButton, SIGNAL(clicked()), this, SLOT(showExceptions()));
    connect(setHomeToCurrentPageButton, SIGNAL(clicked()), this, SLOT(setHomeToCurrentPage()));
    connect(cookiesButton, SIGNAL(clicked()), this, SLOT(showCookies()));
    connect(standardFontButton, SIGNAL(clicked()), this, SLOT(chooseFont()));
    connect(fixedFontButton, SIGNAL(clicked()), this, SLOT(chooseFixedFont()));

#if QT_VERSION < 0x040500
    oneCloseButton->setVisible(false); // no other mode than one close button with qt <4.5
#endif
    loadDefaults();
    loadFromSettings();
}

void SettingsDialog::loadDefaults()
{
    QWebSettings *defaultSettings = QWebSettings::globalSettings();
    QString standardFontFamily = defaultSettings->fontFamily(QWebSettings::StandardFont);
    int standardFontSize = defaultSettings->fontSize(QWebSettings::DefaultFontSize);
    standardFont = QFont(standardFontFamily, standardFontSize);
    standardLabel->setText(QString(QLatin1String("%1 %2")).arg(standardFont.family()).arg(standardFont.pointSize()));

    QString fixedFontFamily = defaultSettings->fontFamily(QWebSettings::FixedFont);
    int fixedFontSize = defaultSettings->fontSize(QWebSettings::DefaultFixedFontSize);
    fixedFont = QFont(fixedFontFamily, fixedFontSize);
    fixedLabel->setText(QString(QLatin1String("%1 %2")).arg(fixedFont.family()).arg(fixedFont.pointSize()));

    downloadsLocation->setText(QDesktopServices::storageLocation(QDesktopServices::DesktopLocation));

    enableJavascript->setChecked(defaultSettings->testAttribute(QWebSettings::JavascriptEnabled));
    enablePlugins->setChecked(defaultSettings->testAttribute(QWebSettings::PluginsEnabled));
    enableImages->setChecked(defaultSettings->testAttribute(QWebSettings::AutoLoadImages));
}

void SettingsDialog::loadFromSettings()
{
    QSettings settings;
    settings.beginGroup(QLatin1String("MainWindow"));
    QString defaultHome = QLatin1String("http://www.arora-browser.org");
    homeLineEdit->setText(settings.value(QLatin1String("home"), defaultHome).toString());
    startupBehavior->setCurrentIndex(settings.value(QLatin1String("startupBehavior"), 0).toInt());
    settings.endGroup();

    settings.beginGroup(QLatin1String("history"));
    int historyExpire = settings.value(QLatin1String("historyLimit")).toInt();
    int idx = 0;
    switch (historyExpire) {
    case 1: idx = 0; break;
    case 7: idx = 1; break;
    case 14: idx = 2; break;
    case 30: idx = 3; break;
    case 365: idx = 4; break;
    case -1: idx = 5; break;
    case -2: idx = 6; break;
    default:
        idx = 5;
    }
    expireHistory->setCurrentIndex(idx);
    settings.endGroup();

    settings.beginGroup(QLatin1String("downloadmanager"));
    bool alwaysPromptForFileName = settings.value(QLatin1String("alwaysPromptForFileName"), false).toBool();
    if (alwaysPromptForFileName)
        downloadAsk->setChecked(true);
    QString downloadDirectory = settings.value(QLatin1String("downloadDirectory"), downloadsLocation->text()).toString();
    downloadsLocation->setText(downloadDirectory);
    settings.endGroup();

    settings.beginGroup(QLatin1String("general"));
    openLinksIn->setCurrentIndex(settings.value(QLatin1String("openLinksIn"), openLinksIn->currentIndex()).toInt());

    settings.endGroup();

    // Appearance
    settings.beginGroup(QLatin1String("websettings"));
    fixedFont = qVariantValue<QFont>(settings.value(QLatin1String("fixedFont"), fixedFont));
    standardFont = qVariantValue<QFont>(settings.value(QLatin1String("standardFont"), standardFont));

    standardLabel->setText(QString(QLatin1String("%1 %2")).arg(standardFont.family()).arg(standardFont.pointSize()));
    fixedLabel->setText(QString(QLatin1String("%1 %2")).arg(fixedFont.family()).arg(fixedFont.pointSize()));

    enableJavascript->setChecked(settings.value(QLatin1String("enableJavascript"), enableJavascript->isChecked()).toBool());
    enablePlugins->setChecked(settings.value(QLatin1String("enablePlugins"), enablePlugins->isChecked()).toBool());
    enableImages->setChecked(settings.value(QLatin1String("enableImages"), enableImages->isChecked()).toBool());
    userStyleSheet->setText(settings.value(QLatin1String("userStyleSheet")).toUrl().toString());
    settings.endGroup();

    // Privacy
    settings.beginGroup(QLatin1String("cookies"));

    CookieJar *jar = BrowserApplication::cookieJar();
    QByteArray value = settings.value(QLatin1String("acceptCookies"), QLatin1String("AcceptOnlyFromSitesNavigatedTo")).toByteArray();
    QMetaEnum acceptPolicyEnum = jar->staticMetaObject.enumerator(jar->staticMetaObject.indexOfEnumerator("AcceptPolicy"));
    CookieJar::AcceptPolicy acceptCookies = acceptPolicyEnum.keyToValue(value) == -1 ?
                        CookieJar::AcceptOnlyFromSitesNavigatedTo :
                        static_cast<CookieJar::AcceptPolicy>(acceptPolicyEnum.keyToValue(value));
    switch (acceptCookies) {
    case CookieJar::AcceptAlways:
        acceptCombo->setCurrentIndex(0);
        break;
    case CookieJar::AcceptNever:
        acceptCombo->setCurrentIndex(1);
        break;
    case CookieJar::AcceptOnlyFromSitesNavigatedTo:
        acceptCombo->setCurrentIndex(2);
        break;
    }

    value = settings.value(QLatin1String("keepCookiesUntil"), QLatin1String("Expire")).toByteArray();
    QMetaEnum keepPolicyEnum = jar->staticMetaObject.enumerator(jar->staticMetaObject.indexOfEnumerator("KeepPolicy"));
    CookieJar::KeepPolicy keepCookies = keepPolicyEnum.keyToValue(value) == -1 ?
                        CookieJar::KeepUntilExpire :
                        static_cast<CookieJar::KeepPolicy>(keepPolicyEnum.keyToValue(value));
    switch (keepCookies) {
    case CookieJar::KeepUntilExpire:
        keepUntilCombo->setCurrentIndex(0);
        break;
    case CookieJar::KeepUntilExit:
        keepUntilCombo->setCurrentIndex(1);
        break;
    case CookieJar::KeepUntilTimeLimit:
        keepUntilCombo->setCurrentIndex(2);
        break;
    }
    settings.endGroup();


    // Proxy
    settings.beginGroup(QLatin1String("proxy"));
    proxySupport->setChecked(settings.value(QLatin1String("enabled"), false).toBool());
    proxyType->setCurrentIndex(settings.value(QLatin1String("type"), 0).toInt());
    proxyHostName->setText(settings.value(QLatin1String("hostName")).toString());
    proxyPort->setValue(settings.value(QLatin1String("port"), 1080).toInt());
    proxyUserName->setText(settings.value(QLatin1String("userName")).toString());
    proxyPassword->setText(settings.value(QLatin1String("password")).toString());
    settings.endGroup();

    // Tabs
    settings.beginGroup(QLatin1String("tabs"));
    selectTabsWhenCreated->setChecked(settings.value(QLatin1String("selectNewTabs"), false).toBool());
    confirmClosingMultipleTabs->setChecked(settings.value(QLatin1String("confirmClosingMultipleTabs"), true).toBool());
#if QT_VERSION >= 0x040500
    oneCloseButton->setChecked(settings.value(QLatin1String("oneCloseButton"),false).toBool());
#endif
    settings.endGroup();
}

void SettingsDialog::saveToSettings()
{
    QSettings settings;
    settings.beginGroup(QLatin1String("MainWindow"));
    settings.setValue(QLatin1String("home"), homeLineEdit->text());
    settings.setValue(QLatin1String("startupBehavior"), startupBehavior->currentIndex());
    settings.endGroup();

    settings.beginGroup(QLatin1String("downloadmanager"));
    settings.setValue(QLatin1String("alwaysPromptForFileName"), downloadAsk->isChecked());
    settings.setValue(QLatin1String("downloadDirectory"), downloadsLocation->text());
    settings.endGroup();

    settings.beginGroup(QLatin1String("general"));
    settings.setValue(QLatin1String("openLinksIn"), openLinksIn->currentIndex());
    settings.endGroup();

    settings.beginGroup(QLatin1String("history"));
    int historyExpire = expireHistory->currentIndex();
    int idx = -1;
    switch (historyExpire) {
    case 0: idx = 1; break;
    case 1: idx = 7; break;
    case 2: idx = 14; break;
    case 3: idx = 30; break;
    case 4: idx = 365; break;
    case 5: idx = -1; break;
    case 6: idx = -2; break;
    }
    settings.setValue(QLatin1String("historyLimit"), idx);
    settings.endGroup();

    // Appearance
    settings.beginGroup(QLatin1String("websettings"));
    settings.setValue(QLatin1String("fixedFont"), fixedFont);
    settings.setValue(QLatin1String("standardFont"), standardFont);
    settings.setValue(QLatin1String("enableJavascript"), enableJavascript->isChecked());
    settings.setValue(QLatin1String("enablePlugins"), enablePlugins->isChecked());
    settings.setValue(QLatin1String("enableImages"), enableImages->isChecked());
    QString userStyleSheetString = userStyleSheet->text();
    if (QFile::exists(userStyleSheetString))
        settings.setValue(QLatin1String("userStyleSheet"), QUrl::fromLocalFile(userStyleSheetString));
    else
        settings.setValue(QLatin1String("userStyleSheet"), QUrl(userStyleSheetString));
    settings.endGroup();

    //Privacy
    settings.beginGroup(QLatin1String("cookies"));

    CookieJar::KeepPolicy keepCookies;
    switch (acceptCombo->currentIndex()) {
    default:
    case 0:
        keepCookies = CookieJar::KeepUntilExpire;
        break;
    case 1:
        keepCookies = CookieJar::KeepUntilExit;
        break;
    case 2:
        keepCookies = CookieJar::KeepUntilTimeLimit;
        break;
    }
    CookieJar *jar = BrowserApplication::cookieJar();
    QMetaEnum acceptPolicyEnum = jar->staticMetaObject.enumerator(jar->staticMetaObject.indexOfEnumerator("AcceptPolicy"));
    settings.setValue(QLatin1String("acceptCookies"), QLatin1String(acceptPolicyEnum.valueToKey(keepCookies)));

    CookieJar::KeepPolicy keepPolicy;
    switch (keepUntilCombo->currentIndex()) {
    default:
    case 0:
        keepPolicy = CookieJar::KeepUntilExpire;
        break;
    case 1:
        keepPolicy = CookieJar::KeepUntilExit;
        break;
    case 2:
        keepPolicy = CookieJar::KeepUntilTimeLimit;
        break;
    }

    QMetaEnum keepPolicyEnum = jar->staticMetaObject.enumerator(jar->staticMetaObject.indexOfEnumerator("KeepPolicy"));
    settings.setValue(QLatin1String("keepCookiesUntil"), QLatin1String(keepPolicyEnum.valueToKey(keepPolicy)));

    settings.endGroup();

    // proxy
    settings.beginGroup(QLatin1String("proxy"));
    settings.setValue(QLatin1String("enabled"), proxySupport->isChecked());
    settings.setValue(QLatin1String("type"), proxyType->currentIndex());
    settings.setValue(QLatin1String("hostName"), proxyHostName->text());
    settings.setValue(QLatin1String("port"), proxyPort->text());
    settings.setValue(QLatin1String("userName"), proxyUserName->text());
    settings.setValue(QLatin1String("password"), proxyPassword->text());
    settings.endGroup();

    // Tabs
    settings.beginGroup(QLatin1String("tabs"));
    settings.setValue(QLatin1String("selectNewTabs"), selectTabsWhenCreated->isChecked());
    settings.setValue(QLatin1String("confirmClosingMultipleTabs"), confirmClosingMultipleTabs->isChecked());
#if QT_VERSION >= 0x040500
    settings.setValue(QLatin1String("oneCloseButton"), oneCloseButton->isChecked());
#endif
    settings.endGroup();

    BrowserApplication::instance()->loadSettings();
    BrowserApplication::networkAccessManager()->loadSettings();
    BrowserApplication::cookieJar()->loadSettings();
    BrowserApplication::historyManager()->loadSettings();
}

void SettingsDialog::accept()
{
    saveToSettings();
    QDialog::accept();
}

void SettingsDialog::showCookies()
{
    CookieDialog *dialog = new CookieDialog(BrowserApplication::cookieJar(), this);
    dialog->exec();
}

void SettingsDialog::showExceptions()
{
    CookieExceptionsDialog *dialog = new CookieExceptionsDialog(BrowserApplication::cookieJar(), this);
    dialog->exec();
}

void SettingsDialog::chooseFont()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, standardFont, this);
    if (ok) {
        standardFont = font;
        standardLabel->setText(QString(QLatin1String("%1 %2")).arg(font.family()).arg(font.pointSize()));
    }
}

void SettingsDialog::chooseFixedFont()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, fixedFont, this);
    if (ok) {
        fixedFont = font;
        fixedLabel->setText(QString(QLatin1String("%1 %2")).arg(font.family()).arg(font.pointSize()));
    }
}

void SettingsDialog::setHomeToCurrentPage()
{
    BrowserMainWindow *mw = static_cast<BrowserMainWindow*>(parent());
    WebView *webView = mw->currentTab();
    if (webView)
        homeLineEdit->setText(webView->url().toString());
}

