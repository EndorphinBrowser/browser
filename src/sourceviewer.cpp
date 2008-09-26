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

#include "sourceviewer.h"
#include "qaction.h"
#include "qshortcut.h"

SourceViewer::SourceViewer(QString &source, QString &title)
    : QWidget()
    , m_edit(source)
    , m_highlighter(m_edit.document())
    , m_plainTextEditSearch(this)
    , m_menuBar(this)
    , m_editMenu(tr("&Edit"),&m_menuBar)
    , m_findAction(tr("&Find"),&m_editMenu)
    , layout(this)
{
    setWindowTitle(QString(tr("Source of Page ")).append(title));
    setMinimumWidth(640); setMinimumHeight(480);

    m_edit.setLineWrapMode(QPlainTextEdit::NoWrap);
    m_edit.setReadOnly(true);
    m_edit.setFont(QFont(QLatin1String("Monospace"),9,QFont::Normal));

    m_plainTextEditSearch.setPlainTextEdit(&m_edit);

    m_menuBar.addMenu(&m_editMenu);
    m_editMenu.addAction(&m_findAction);
    m_findAction.setShortcuts(QKeySequence::Find);
    connect(&m_findAction, SIGNAL(triggered()), this, SLOT(slotEditFind()));

    layout.setSpacing(0);
    layout.setContentsMargins(0, 0, 0, 0);
    layout.addWidget(&m_menuBar);
    layout.addWidget(&m_plainTextEditSearch);
    layout.addWidget(&m_edit);
    setLayout(&layout);
    show();
}

void SourceViewer::slotEditFind()
{
    m_plainTextEditSearch.showFind();
}
