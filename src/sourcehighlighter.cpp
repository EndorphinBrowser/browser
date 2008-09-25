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

#include "sourcehighlighter.h"

SourceHighlighter::SourceHighlighter(QTextDocument *document)
    : QSyntaxHighlighter(document)
{
    QTextCharFormat entityFormat;
    entityFormat.setForeground(QColor(30,180,30));
    entityFormat.setFontWeight(QFont::Normal);
    setFormatFor(Entity, entityFormat);

    QTextCharFormat tagFormat;
    tagFormat.setForeground(QColor(30,30,60));
    tagFormat.setFontWeight(QFont::Bold);
    setFormatFor(Tag, tagFormat);

    QTextCharFormat commentFormat;
    commentFormat.setForeground(QColor(120,120,160));
    commentFormat.setFontWeight(QFont::Normal);
    setFormatFor(Comment, commentFormat);

    QTextCharFormat attributeFormat;
    attributeFormat.setForeground(QColor(200,30,30));
    attributeFormat.setFontWeight(QFont::Normal);
    setFormatFor(Attribute, attributeFormat);
}

QTextCharFormat SourceHighlighter::getFormatFor(Construct construct)
{
    return formats[construct];
}

void SourceHighlighter::setFormatFor(Construct construct,
                         QTextCharFormat &format)
{
    formats[construct] = format;
}

void SourceHighlighter::highlightBlock(const QString &text)
{
    int state = previousBlockState();
    int len = text.length();
    int start = 0;
    int pos = 0;
    QRegExp regex;

    while (pos >= 0 && pos < len && len>0) {
        switch (state) {
        default:
        case Normal:
            regex.setPattern(QLatin1String("[<&]"));
            pos = regex.indexIn(text,pos);
            if (pos>=0) {
                if (text.at(pos) == QLatin1Char('<')) {
                    start = pos;
                    if (text.mid(pos,4) == QLatin1String("<!--")) {
                        state = InComment;
                    } else {
                        state = InTag;
                    }
                    pos++;
                } else if (text.at(pos) == QLatin1Char('&')) {
                    regex.setPattern(QLatin1String("&[a-zA-Z0-9]+;"));
                    if (regex.indexIn(text,pos) == pos) {
                        setFormat(pos,regex.matchedLength(),formats[Entity]);
                    }
                    pos++;
                }
            }
            break;
        case InComment:
            regex.setPattern(QLatin1String("-->"));
            pos = regex.indexIn(text,pos);
            if (pos>=0) {
                state = Normal;
                pos+=3;
                setFormat(start,pos-start,formats[Comment]);
                pos++;
            } else {
                setFormat(start,len-start,formats[Comment]);
            }
            break;
         case InTag:
            regex.setPattern(QLatin1String("[>\"]"));
            pos = regex.indexIn(text,pos);
            if (pos>=0) {
                if (text.at(pos) == QLatin1Char('>')) {
                    state = Normal;
                    pos++;
                    setFormat(start,pos-start,formats[Tag]);
                } else if (text.at(pos)== QLatin1Char('"')) {
                    setFormat(start,pos-start,formats[Tag]);
                    start = pos;
                    state = InAttribute;
                    pos++;
                }
            } else {
                setFormat(start,len-start,formats[Tag]);
            }
            break;
        case InAttribute:
            regex.setPattern(QLatin1String("\""));
            pos = regex.indexIn(text,pos);
            if (pos>=0) {
                setFormat(start,pos-start,formats[Attribute]);
                state = InTag;
                start = ++pos;
            } else {
                setFormat(start,len-start,formats[Attribute]);
            }
            break;
        }
    }
    setCurrentBlockState(state);
}
