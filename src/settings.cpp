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

#include "acceptlanguagedialog.h"
#include "browserapplication.h"
#include "browsermainwindow.h"
#include "historymanager.h"
#include "tabwidget.h"
#include "webpage.h"
#include "webview.h"

#include <QDesktopServices>
#include <QFile>
#include <QFontDialog>
#include <QMetaObject>
#include <QMessageBox>
#include <QSettings>
#include <QFileDialog>

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
    , m_cacheEnabled(false)
{
    setupUi(this);
    connect(setHomeToCurrentPageButton, SIGNAL(clicked()), this, SLOT(setHomeToCurrentPage()));
    connect(standardFontButton, SIGNAL(clicked()), this, SLOT(chooseFont()));
    connect(fixedFontButton, SIGNAL(clicked()), this, SLOT(chooseFixedFont()));
    connect(languageButton, SIGNAL(clicked()), this, SLOT(chooseAcceptLanguage()));
    connect(downloadDirectoryButton, SIGNAL(clicked()), this, SLOT(chooseDownloadDirectory()));
    connect(externalDownloadBrowse, SIGNAL(clicked()), this, SLOT(chooseDownloadProgram()));
    connect(styleSheetBrowseButton, SIGNAL(clicked()), this, SLOT(chooseStyleSheet()));
    connect(newSettings, SIGNAL(clicked()), this, SLOT(openModernSettings()));

    loadDefaults();
    loadFromSettings();
}

void SettingsDialog::loadDefaults()
{
    QWebEngineSettings *defaultSettings = QWebEngineSettings::globalSettings();
    QString standardFontFamily = defaultSettings->fontFamily(QWebEngineSettings::StandardFont);
    int standardFontSize = defaultSettings->fontSize(QWebEngineSettings::DefaultFontSize);
    m_standardFont = QFont(standardFontFamily, standardFontSize);
    standardLabel->setText(QString(QStringLiteral("%1 %2")).arg(m_standardFont.family()).arg(m_standardFont.pointSize()));

    QString fixedFontFamily = defaultSettings->fontFamily(QWebEngineSettings::FixedFont);
    int fixedFontSize = defaultSettings->fontSize(QWebEngineSettings::DefaultFixedFontSize);
    m_fixedFont = QFont(fixedFontFamily, fixedFontSize);
    fixedLabel->setText(QString(QStringLiteral("%1 %2")).arg(m_fixedFont.family()).arg(m_fixedFont.pointSize()));

    downloadsLocation->setText(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));

    blockPopupWindows->setChecked(!defaultSettings->testAttribute(QWebEngineSettings::JavascriptCanOpenWindows));
    enableJavascript->setChecked(defaultSettings->testAttribute(QWebEngineSettings::JavascriptEnabled));
    enableImages->setChecked(defaultSettings->testAttribute(QWebEngineSettings::AutoLoadImages));
    enableLocalStorage->setChecked(defaultSettings->testAttribute(QWebEngineSettings::LocalStorageEnabled));

    minimFontSizeCheckBox->setChecked(false);
    minimumFontSizeSpinBox->setValue(9);
}

void SettingsDialog::loadFromSettings()
{
    QSettings settings;
    settings.beginGroup(QStringLiteral("Settings"));
    tabWidget->setCurrentIndex(settings.value(QStringLiteral("currentTab"), 0).toInt());
    settings.endGroup();

    settings.beginGroup(QStringLiteral("MainWindow"));
    QString defaultHome = QStringLiteral("about:home");
    homeLineEdit->setText(settings.value(QStringLiteral("home"), defaultHome).toString());
    startupBehavior->setCurrentIndex(settings.value(QStringLiteral("startupBehavior"), 0).toInt());
    settings.endGroup();

    settings.beginGroup(QStringLiteral("history"));
    int historyExpire = settings.value(QStringLiteral("historyLimit")).toInt();
    int idx = 0;
    switch (historyExpire) {
    case 1:
        idx = 0;
        break;
    case 7:
        idx = 1;
        break;
    case 14:
        idx = 2;
        break;
    case 30:
        idx = 3;
        break;
    case 365:
        idx = 4;
        break;
    case -1:
        idx = 5;
        break;
    case -2:
        idx = 6;
        break;
    default:
        idx = 5;
    }
    expireHistory->setCurrentIndex(idx);
    settings.endGroup();

    settings.beginGroup(QStringLiteral("urlloading"));
    bool search = settings.value(QStringLiteral("searchEngineFallback"), true).toBool();
    searchEngineFallback->setChecked(search);
    settings.endGroup();

    settings.beginGroup(QStringLiteral("downloadmanager"));
    bool alwaysPromptForFileName = settings.value(QStringLiteral("alwaysPromptForFileName"), false).toBool();
    downloadAsk->setChecked(alwaysPromptForFileName);
    QString downloadDirectory = settings.value(QStringLiteral("downloadDirectory"), downloadsLocation->text()).toString();
    downloadsLocation->setText(downloadDirectory);
    externalDownloadButton->setChecked(settings.value(QStringLiteral("external"), false).toBool());
    externalDownloadPath->setText(settings.value(QStringLiteral("externalPath")).toString());
    settings.endGroup();

    // Appearance
    settings.beginGroup(QStringLiteral("websettings"));
    m_fixedFont = settings.value(QStringLiteral("fixedFont"), m_fixedFont).value<QFont>();
    m_standardFont = settings.value(QStringLiteral("standardFont"), m_standardFont).value<QFont>();

    standardLabel->setText(QString(QStringLiteral("%1 %2")).arg(m_standardFont.family()).arg(m_standardFont.pointSize()));
    fixedLabel->setText(QString(QStringLiteral("%1 %2")).arg(m_fixedFont.family()).arg(m_fixedFont.pointSize()));

    blockPopupWindows->setChecked(settings.value(QStringLiteral("blockPopupWindows"), blockPopupWindows->isChecked()).toBool());
    enableJavascript->setChecked(settings.value(QStringLiteral("enableJavascript"), enableJavascript->isChecked()).toBool());
    enableImages->setChecked(settings.value(QStringLiteral("enableImages"), enableImages->isChecked()).toBool());
    enableLocalStorage->setChecked(settings.value(QStringLiteral("enableLocalStorage"), enableLocalStorage->isChecked()).toBool());
    userStyleSheet->setText(QString::fromUtf8(settings.value(QStringLiteral("userStyleSheet")).toUrl().toEncoded()));
    int minimumFontSize = settings.value(QStringLiteral("minimumFontSize"), 0).toInt();
    minimFontSizeCheckBox->setChecked(minimumFontSize != 0);
    if (minimumFontSize != 0)
        minimumFontSizeSpinBox->setValue(minimumFontSize);
    settings.endGroup();
    // UI Appearance
    settings.beginGroup(QStringLiteral("appearance"));
    urlBarType->setCurrentIndex(settings.value(QStringLiteral("urlBarType"), 0).toInt());
    settings.endGroup();
    if(urlBarType->currentIndex() == 1) {
        // Required so URL bar accepts search terms
        settings.beginGroup(QStringLiteral("urlloading"));
        settings.setValue(QStringLiteral("searchEngineFallback"), true);
        searchEngineFallback->setChecked(true);
        searchEngineFallback->setEnabled(false);
    } else {
        searchEngineFallback->setEnabled(true);
    }
    settings.endGroup();

    // Network
    settings.beginGroup(QStringLiteral("network"));
    m_cacheEnabled = settings.value(QStringLiteral("cacheEnabled"), true).toBool();
    networkCache->setChecked(m_cacheEnabled);
    networkCacheMaximumSizeSpinBox->setValue(settings.value(QStringLiteral("maximumCacheSize"), 50).toInt());
    settings.endGroup();

    // Proxy
    settings.beginGroup(QStringLiteral("proxy"));
    proxySupport->setChecked(settings.value(QStringLiteral("enabled"), false).toBool());
    proxyType->setCurrentIndex(settings.value(QStringLiteral("type"), 0).toInt());
    proxyHostName->setText(settings.value(QStringLiteral("hostName")).toString());
    proxyPort->setValue(settings.value(QStringLiteral("port"), 1080).toInt());
    proxyUserName->setText(settings.value(QStringLiteral("userName")).toString());
    proxyPassword->setText(settings.value(QStringLiteral("password")).toString());
    settings.endGroup();

    // Tabs
    settings.beginGroup(QStringLiteral("tabs"));
    selectTabsWhenCreated->setChecked(settings.value(QStringLiteral("selectNewTabs"), false).toBool());
    confirmClosingMultipleTabs->setChecked(settings.value(QStringLiteral("confirmClosingMultipleTabs"), true).toBool());
    oneCloseButton->setChecked(settings.value(QStringLiteral("oneCloseButton"),false).toBool());
    quitAsLastTabClosed->setChecked(settings.value(QStringLiteral("quitAsLastTabClosed"), true).toBool());
    openTargetBlankLinksIn->setCurrentIndex(settings.value(QStringLiteral("openTargetBlankLinksIn"), TabWidget::NewSelectedTab).toInt());
    openLinksFromAppsIn->setCurrentIndex(settings.value(QStringLiteral("openLinksFromAppsIn"), TabWidget::NewSelectedTab).toInt());
    settings.endGroup();

    // Accessibility
    settings.beginGroup(QStringLiteral("WebView"));
    enableAccessKeys->setChecked(settings.value(QStringLiteral("enableAccessKeys"), true).toBool());
    settings.endGroup();
}

void SettingsDialog::saveToSettings()
{
    QSettings settings;
    settings.beginGroup(QStringLiteral("Settings"));
    settings.setValue(QStringLiteral("currentTab"), tabWidget->currentIndex());
    settings.endGroup();

    settings.beginGroup(QStringLiteral("MainWindow"));
    settings.setValue(QStringLiteral("home"), homeLineEdit->text());
    settings.setValue(QStringLiteral("startupBehavior"), startupBehavior->currentIndex());
    settings.endGroup();

    settings.beginGroup(QStringLiteral("downloadmanager"));
    settings.setValue(QStringLiteral("alwaysPromptForFileName"), downloadAsk->isChecked());
    settings.setValue(QStringLiteral("downloadDirectory"), downloadsLocation->text());
    settings.setValue(QStringLiteral("external"), externalDownloadButton->isChecked());
    settings.setValue(QStringLiteral("externalPath"), externalDownloadPath->text());
    settings.endGroup();

    settings.beginGroup(QStringLiteral("history"));
    int historyExpire = expireHistory->currentIndex();
    int idx = -1;
    switch (historyExpire) {
    case 0:
        idx = 1;
        break;
    case 1:
        idx = 7;
        break;
    case 2:
        idx = 14;
        break;
    case 3:
        idx = 30;
        break;
    case 4:
        idx = 365;
        break;
    case 5:
        idx = -1;
        break;
    case 6:
        idx = -2;
        break;
    }
    settings.setValue(QStringLiteral("historyLimit"), idx);
    settings.endGroup();

    settings.beginGroup(QStringLiteral("urlloading"));
    settings.setValue(QStringLiteral("searchEngineFallback"), searchEngineFallback->isChecked());
    settings.endGroup();

    // Appearance
    settings.beginGroup(QStringLiteral("websettings"));
    settings.setValue(QStringLiteral("fixedFont"), m_fixedFont);
    settings.setValue(QStringLiteral("standardFont"), m_standardFont);

    settings.setValue(QStringLiteral("blockPopupWindows"), blockPopupWindows->isChecked());
    settings.setValue(QStringLiteral("enableJavascript"), enableJavascript->isChecked());
    settings.setValue(QStringLiteral("enableImages"), enableImages->isChecked());
    settings.setValue(QStringLiteral("enableLocalStorage"), enableLocalStorage->isChecked());
    QString userStyleSheetString = userStyleSheet->text();
    if (QFile::exists(userStyleSheetString))
        settings.setValue(QStringLiteral("userStyleSheet"), QUrl::fromLocalFile(userStyleSheetString));
    else
        settings.setValue(QStringLiteral("userStyleSheet"), QUrl::fromEncoded(userStyleSheetString.toUtf8()));

    if (minimFontSizeCheckBox->isChecked())
        settings.setValue(QStringLiteral("minimumFontSize"), minimumFontSizeSpinBox->value());
    else
        settings.setValue(QStringLiteral("minimumFontSize"), 0);
    settings.endGroup();
    // UI Appearance
    settings.beginGroup(QStringLiteral("appearance"));
    settings.setValue(QStringLiteral("urlBarType"), urlBarType->currentIndex());
    settings.endGroup();
    if(urlBarType->currentIndex() == 1) {
        // Required so URL bar accepts search terms
        settings.beginGroup(QStringLiteral("urlloading"));
        settings.setValue(QStringLiteral("searchEngineFallback"), true);
        searchEngineFallback->setChecked(true);
        searchEngineFallback->setEnabled(false);
    } else {
        searchEngineFallback->setEnabled(true);
    }
    settings.endGroup();


    // Network
    settings.beginGroup(QStringLiteral("network"));
    settings.setValue(QStringLiteral("cacheEnabled"), networkCache->isChecked());
    settings.setValue(QStringLiteral("maximumCacheSize"), networkCacheMaximumSizeSpinBox->value());
    settings.endGroup();

    // proxy
    settings.beginGroup(QStringLiteral("proxy"));
    settings.setValue(QStringLiteral("enabled"), proxySupport->isChecked());
    settings.setValue(QStringLiteral("type"), proxyType->currentIndex());
    settings.setValue(QStringLiteral("hostName"), proxyHostName->text());
    settings.setValue(QStringLiteral("port"), proxyPort->text());
    settings.setValue(QStringLiteral("userName"), proxyUserName->text());
    settings.setValue(QStringLiteral("password"), proxyPassword->text());
    settings.endGroup();

    // Tabs
    settings.beginGroup(QStringLiteral("tabs"));
    settings.setValue(QStringLiteral("selectNewTabs"), selectTabsWhenCreated->isChecked());
    settings.setValue(QStringLiteral("confirmClosingMultipleTabs"), confirmClosingMultipleTabs->isChecked());
    settings.setValue(QStringLiteral("oneCloseButton"), oneCloseButton->isChecked());
    settings.setValue(QStringLiteral("quitAsLastTabClosed"), quitAsLastTabClosed->isChecked());
    settings.setValue(QStringLiteral("openTargetBlankLinksIn"), openTargetBlankLinksIn->currentIndex());
    settings.setValue(QStringLiteral("openLinksFromAppsIn"), openLinksFromAppsIn->currentIndex());
    settings.endGroup();

    // Accessibility
    settings.beginGroup(QStringLiteral("WebView"));
    settings.setValue(QStringLiteral("enableAccessKeys"), enableAccessKeys->isChecked());
    settings.endGroup();

    BrowserApplication::instance()->loadSettings();
    BrowserApplication::historyManager()->loadSettings();

    QList<BrowserMainWindow*> list = BrowserApplication::instance()->mainWindows();
    Q_FOREACH (BrowserMainWindow *mainWindow, list) {
        mainWindow->tabWidget()->loadSettings();
    }
}

void SettingsDialog::accept()
{
    saveToSettings();
    QDialog::accept();
}

void SettingsDialog::chooseDownloadDirectory()
{
    QString fileName = QFileDialog::getExistingDirectory(this, tr("Choose Directory"), downloadsLocation->text());
    downloadsLocation->setText(fileName);
}

void SettingsDialog::chooseDownloadProgram()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Choose Program"), externalDownloadPath->text());
    if (fileName.contains(QChar(' ')))
        fileName = QString(QStringLiteral("\"%1\"")).arg(fileName);
    externalDownloadPath->setText(fileName);
}

void SettingsDialog::chooseFont()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, m_standardFont, this);
    if (ok) {
        m_standardFont = font;
        standardLabel->setText(QString(QStringLiteral("%1 %2")).arg(font.family()).arg(font.pointSize()));
    }
}

void SettingsDialog::chooseFixedFont()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, m_fixedFont, this);
    if (ok) {
        m_fixedFont = font;
        fixedLabel->setText(QString(QStringLiteral("%1 %2")).arg(font.family()).arg(font.pointSize()));
    }
}

void SettingsDialog::setHomeToCurrentPage()
{
    BrowserMainWindow *mw = static_cast<BrowserMainWindow*>(parent());
    WebView *webView = mw->currentTab();
    if (webView)
        homeLineEdit->setText(QString::fromUtf8(webView->url().toEncoded()));
}

void SettingsDialog::chooseAcceptLanguage()
{
    AcceptLanguageDialog dialog;
    dialog.exec();
}

void SettingsDialog::chooseStyleSheet()
{
    QUrl url = QUrl::fromEncoded(userStyleSheet->text().toUtf8());
    QString fileName = QFileDialog::getOpenFileName(this, tr("Choose CSS File"), url.toLocalFile());
    userStyleSheet->setText(QString::fromUtf8(QUrl::fromLocalFile(fileName).toEncoded()));
}

void SettingsDialog::openModernSettings() {
    BrowserMainWindow *mw = static_cast<BrowserMainWindow*>(parent());
    mw->tabWidget()->loadUrl(QUrl("qrc:/settings.html"), TabWidget::NewSelectedTab, "Settings");
    close();
}
