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

#include <qlayout.h>
#include <qplaintextedit.h>
#include <qshortcut.h>
#include <qmenubar.h>

#include "plaintexteditsearch.h"
#include "sourcehighlighter.h"

SourceViewer::SourceViewer(const QString &source, const QString &title, QWidget *parent)
    : QDialog(parent)
    , m_edit(new QPlainTextEdit(source))
    , m_highlighter(new SourceHighlighter(m_edit->document()))
    , m_plainTextEditSearch(new PlainTextEditSearch(m_edit, this))
    , layout(new QVBoxLayout(this))
    , m_menuBar(new QMenuBar(this))
    , m_editMenu(new QMenu(tr("&Edit"), m_menuBar))
    , m_findAction(new QAction(tr("&Find"), m_editMenu))

{
    setWindowTitle(QString(tr("Source of Page ")).append(title));
    resize(640, 480);

    m_edit->setLineWrapMode(QPlainTextEdit::NoWrap);
    m_edit->setReadOnly(true);
    QFont mFont = m_edit->font();
    mFont.setFamily(QLatin1String("Monospace"));
    m_edit->setFont(mFont);
    m_edit->setLineWidth(0);
    m_edit->setFrameShape(QFrame::NoFrame);

    m_menuBar->addMenu(m_editMenu);
    m_editMenu->addAction(m_findAction);
    m_findAction->setShortcuts(QKeySequence::Find);
    connect(m_findAction, SIGNAL(triggered()),
            m_plainTextEditSearch, SLOT(showFind()));

    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_menuBar);
    layout->addWidget(m_plainTextEditSearch);
    layout->addWidget(m_edit);
    setLayout(layout);
}

