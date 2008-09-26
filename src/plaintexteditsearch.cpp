/*
 * Copyright 2008 Christian Franke <cfchris6@ts2server.com>
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

#include "plaintexteditsearch.h"

#include <qevent.h>
#include <qshortcut.h>
#include <qtimeline.h>

#include <qplaintextedit.h>


PlainTextEditSearch::PlainTextEditSearch(QWidget *parent)
    : SearchBar(parent)
{
/*    connect(ui.nextButton, SIGNAL(clicked()),
            this, SLOT(findNext()));
    connect(ui.previousButton, SIGNAL(clicked()),
            this, SLOT(findPrevious()));
    connect(ui.searchLineEdit, SIGNAL(returnPressed()),
            this, SLOT(findNext()));
    connect(ui.doneButton, SIGNAL(clicked()),
            this, SLOT(animateHide()));*/
}

void PlainTextEditSearch::setPlainTextEdit(QPlainTextEdit *plainTextEdit)
{
    setObject(plainTextEdit);
}

QPlainTextEdit *PlainTextEditSearch::plainTextEdit() const
{
    return (QPlainTextEdit*)getObject();
}

void PlainTextEditSearch::findNext()
{
/*    find(QWebPage::FindWrapsAroundDocument);*/
}

void PlainTextEditSearch::findPrevious()
{
/*    find(QWebPage::FindBackward | QWebPage::FindWrapsAroundDocument);*/
}

void PlainTextEditSearch::find(QTextDocument::FindFlags flags)
{
/*    QString searchString = ui.searchLineEdit->text();
    if (!m_view || searchString.isEmpty())
        return;
    QString infoString;
    if (!((QWebView*)m_view)->findText(searchString, flags))
        infoString = tr("Not Found");
    ui.searchInfo->setText(infoString);*/
}
