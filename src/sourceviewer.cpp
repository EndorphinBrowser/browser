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

    edit = new QPlainTextEdit(source);
    edit->setLineWrapMode(QPlainTextEdit::NoWrap);
    edit->setReadOnly(true);
    edit->setFont(QFont(QLatin1String("Monospace"),9,QFont::Normal));
    highlighter = new SourceHighlighter(edit->document());

    layout = new QVBoxLayout;
    layout->addWidget(edit);
    setLayout(layout);
    show();
}

SourceViewer::~SourceViewer()
{
    delete layout;
    delete highlighter;
    delete edit;
}
