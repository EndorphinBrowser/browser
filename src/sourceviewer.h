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

#ifndef SOURCEVIEWER_H
#define SOURCEVIEWER_H

#include <QDialog>

class QVBoxLayout;
class SourceHighlighter;
class PlainTextEditSearch;
class QPlainTextEdit;
class QMenuBar;
class QMenu;
class QUrl;
class QAction;
class QNetworkReply;
class SourceViewer : public QDialog
{
    Q_OBJECT

public:
    SourceViewer(const QString &source, const QString &title,
                 const QUrl &url, QWidget *parent = nullptr);
    ~SourceViewer();

private:
    QPlainTextEdit *m_edit;
    SourceHighlighter *m_highlighter;
    PlainTextEditSearch *m_plainTextEditSearch;
    QVBoxLayout *m_layout;
    QMenuBar *m_menuBar;
    QMenu *m_editMenu;
    QAction *m_findAction;
    QMenu *m_viewMenu;
    QNetworkReply *m_reply;
    QString m_source;

private slots:
    void loadingFinished();
};

#endif
