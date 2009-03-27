/*
 * Copyright 2008 Benjamin C. Meyer <ben@meyerhome.net>
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

#include "browserapplication.h"

#ifdef Q_OS_WIN
#include "explorerstyle.h"
#endif

#if defined(Q_WS_X11)
#include <qdebug.h>
#include <qstyle.h>
#include <qstylefactory.h>
#endif

int main(int argc, char **argv)
{
    Q_INIT_RESOURCE(htmls);
    Q_INIT_RESOURCE(data);

#if defined(Q_WS_X11)
    // See if -style oxygen has been passed as a command line argument
    int hasStyle = -1;
    int hasOxygen = -1;
    for (int i = 0; i < argc; ++i) {
        if (QLatin1String(argv[i]) == QLatin1String("-style"))
            hasStyle = i;
        if (QLatin1String(argv[i]) == QLatin1String("oxygen"))
            hasOxygen = i;
    }
    bool keepOxygen = (hasStyle + 1 == hasOxygen);
#endif
    BrowserApplication application(argc, argv);
    if (!application.isRunning())
        return 0;

#if defined(Q_WS_X11)
    if (!keepOxygen
        && application.style()->objectName() == QLatin1String("oxygen")) {
        qWarning() << "Oxygen style has been detected, loading Plastique style." << endl
                   << " - KDE's 4.2's Oxygen style has too many issues with Qt 4.5 features that Arora uses and while many of the issues have been fixed for KDE 4.3, KDE 4.2 users get a really ugly Arora."
                   << "Once KDE 4.3 is released this check will be removed."
                   << "If you are still want to use Oxygen add the arguments -style oxygen on the command line.";
        application.setStyle(QLatin1String("plastique"));
    }
#endif
#ifdef Q_OS_WIN
    application.setStyle(new ExplorerStyle);
#endif
    application.newMainWindow();
    return application.exec();
}

