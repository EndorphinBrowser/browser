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

SourceViewer::SourceViewer(QString &source, QString &title)
    : QWidget()
{
    setWindowTitle(QString(QLatin1String("Source of Page ")).append(title));
    setMinimumWidth(640); setMinimumHeight(480);

    m_edit = new QPlainTextEdit(source);
    m_edit->setLineWrapMode(QPlainTextEdit::NoWrap);
    m_edit->setReadOnly(true);
    m_edit->setFont(QFont(QLatin1String("Monospace"),9,QFont::Normal));
    m_highlighter = new SourceHighlighter(m_edit->document());

    layout = new QVBoxLayout;
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    m_plainTextEditSearch = new PlainTextEditSearch(this);
    layout->addWidget(m_plainTextEditSearch);
    layout->addWidget(m_edit);
    setLayout(layout);
    show();
    m_plainTextEditSearch->showFind();
}

SourceViewer::~SourceViewer()
{
    delete layout;
    delete m_highlighter;
    delete m_edit;
    delete m_plainTextEditSearch;
}
