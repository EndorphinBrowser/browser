/*
 *Copyright 2008 Benjamin C. Meyer <ben@meyerhome.net>
 *
 *This program is free software; you can redistribute it and/or modify
 *it under the terms of the GNU General Public License as published by
 *the Free Software Foundation; either version 2 of the License, or
 *(at your option) any later version.
 *
 *This program is distributed in the hope that it will be useful,
 *but WITHOUT ANY WARRANTY; without even the implied warranty of
 *MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *GNU General Public License for more details.
 *
 *You should have received a copy of the GNU General Public License
 *along with this program; if not, write to the Free Software
 *Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *Boston, MA  02110-1301  USA
 */

#ifndef PROXYSTYLE_H
#define PROXYSTYLE_H

#include <qstyle.h>
#include <qstylefactory.h>

/*
    A base class that can be used to write a style that modifise the currently style.
    For more details see: http://doc.trolltech.com/qq/qq09-q-and-a.html#style
*/
class ProxyStyle : public QStyle
{
    Q_OBJECT

public:
    explicit ProxyStyle(const QString &baseStyle)
    {
        style = QStyleFactory::create(baseStyle);
        if (!style)
            style = QStyleFactory::create(QLatin1String("windows"));
    }
    ~ProxyStyle()
        { delete style; }

    virtual void drawComplexControl(ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget = 0) const
        { style->drawComplexControl(control, option, painter, widget); }
    virtual void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = 0) const
        { style->drawControl(element, option, painter, widget); }
    virtual void drawItemPixmap(QPainter *painter, const QRect &rectangle, int alignment, const QPixmap &pixmap) const
        { style->drawItemPixmap(painter, rectangle, alignment, pixmap); }
    virtual void drawItemText(QPainter *painter, const QRect &rectangle, int alignment, const QPalette &palette, bool enabled, const QString &text, QPalette::ColorRole textRole = QPalette::NoRole) const
        { style->drawItemText(painter, rectangle, alignment, palette, enabled, text, textRole); }
    virtual void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = 0) const
        { style->drawPrimitive(element, option, painter, widget); }
    virtual QPixmap generatedIconPixmap(QIcon::Mode iconMode, const QPixmap &pixmap, const QStyleOption *option) const
        { return style->generatedIconPixmap(iconMode, pixmap, option); }
    virtual SubControl hitTestComplexControl(ComplexControl control, const QStyleOptionComplex *option, const QPoint &position, const QWidget *widget = 0) const
        { return style->hitTestComplexControl(control, option, position, widget); }
    virtual QRect itemPixmapRect(const QRect &rectangle, int alignment, const QPixmap &pixmap) const
        { return style->itemPixmapRect(rectangle, alignment, pixmap); }
    virtual QRect itemTextRect(const QFontMetrics &metrics, const QRect &rectangle, int alignment, bool enabled, const QString &text) const
        { return style->itemTextRect(metrics, rectangle, alignment, enabled, text); }
    virtual int pixelMetric(PixelMetric metric, const QStyleOption *option = 0, const QWidget *widget = 0) const
        { return style->pixelMetric(metric, option, widget); }
    virtual void polish(QWidget *widget)
        { style->polish(widget); }
    virtual void polish(QApplication *application)
        { style->polish(application); }
    virtual void polish(QPalette &palette)
        { style->polish(palette); }
    virtual QSize sizeFromContents(ContentsType type, const QStyleOption *option, const QSize &contentsSize, const QWidget *widget = 0) const
        { return style->sizeFromContents(type, option, contentsSize, widget); }
    virtual QPalette standardPalette () const
        { return style->standardPalette(); }
    virtual int styleHint(StyleHint hint, const QStyleOption *option = 0, const QWidget *widget = 0, QStyleHintReturn *returnData = 0) const
        { return style->styleHint(hint, option, widget, returnData); }
    virtual QRect subControlRect(ComplexControl control, const QStyleOptionComplex *option, SubControl subControl, const QWidget *widget = 0) const
        { return style->subControlRect(control, option, subControl, widget); }
    virtual void unpolish(QWidget *widget)
        { style->unpolish(widget); }
    virtual void unpolish(QApplication *application)
        { style->unpolish(application); }
    virtual QPixmap standardPixmap(StandardPixmap standardPixmap, const QStyleOption *opt = 0, const QWidget *widget = 0) const
        { return style->standardPixmap(standardPixmap, opt, widget); }
    virtual QRect subElementRect(SubElement element, const QStyleOption *option, const QWidget *widget = 0) const
        { return style->subElementRect(element, option, widget); }
    virtual bool event(QEvent *e)
        { return style->event(e); }
    virtual bool eventFilter(QObject *o, QEvent *e)
        { return style->eventFilter(o, e); }

protected slots:
    int layoutSpacingImplementation ( QSizePolicy::ControlType control1, QSizePolicy::ControlType control2, Qt::Orientation orientation, const QStyleOption * option = 0, const QWidget * widget = 0 ) const
        { return style->layoutSpacing(control1, control2, orientation, option, widget); }

    QIcon standardIconImplementation ( StandardPixmap standardIcon, const QStyleOption * option = 0, const QWidget * widget = 0 ) const
        { return style->standardIcon(standardIcon, option, widget); }

private:
    QStyle* style;

};

#endif // PROXYSTYLE_H

