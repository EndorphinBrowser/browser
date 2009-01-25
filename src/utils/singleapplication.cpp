/**
 * Copyright (c) 2008, Benjamin C. Meyer
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Benjamin Meyer nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "singleapplication.h"

#include <qlocalserver.h>
#include <qlocalsocket.h>
#include <qtextstream.h>
#include <qfile.h>

#ifndef Q_OS_WIN
#include <unistd.h>
#endif

SingleApplication::SingleApplication(int &argc, char **argv)
    : QApplication(argc, argv)
    , m_localServer(0)
{
}

bool SingleApplication::sendMessage(const QString &message)
{
    QLocalSocket socket;
    socket.connectToServer(serverName());
    if (socket.waitForConnected(500)) {
        QTextStream stream(&socket);
        stream << message;
        stream.flush();
        if (socket.waitForBytesWritten())
            return true;
    }
    return false;
}

bool SingleApplication::startSingleServer()
{
    if (m_localServer)
        return false;

    m_localServer = new QLocalServer(this);
    connect(m_localServer, SIGNAL(newConnection()),
            this, SLOT(newConnection()));
    bool success = false;
    if (!m_localServer->listen(serverName())) {
        if (QAbstractSocket::AddressInUseError == m_localServer->serverError()) {
            // cleanup from a segfaulted server
#ifdef Q_OS_UNIX
            QString fullServerName = QLatin1String("/tmp/") + serverName();
            if (QFile::exists(fullServerName))
                QFile::remove(fullServerName);
#endif
            if (!m_localServer->listen(serverName())) {
                qWarning() << "SingleApplication: Unable to start single server.";
            } else {
                success = true;
            }
        }
    } else {
        success = true;
    }

    QFile file(m_localServer->fullServerName());
    if (!file.setPermissions(QFile::ReadUser | QFile::WriteUser))
        qWarning() << "SingleApplication: Unable to set permissions on:"
                   << file.fileName();
    if (!success) {
        delete m_localServer;
        m_localServer = 0;
    }
    return success;
}

bool SingleApplication::isRunning() const
{
    return (0 != m_localServer);
}

void SingleApplication::newConnection()
{
    QLocalSocket *socket = m_localServer->nextPendingConnection();
    if (!socket)
        return;
    socket->waitForReadyRead();
    QTextStream stream(socket);
    QString message;
    stream >> message;
    emit messageRecieved(message);
    delete socket;
}

QString SingleApplication::serverName() const
{
    QString serverName = QCoreApplication::applicationName();
    Q_ASSERT(!serverName.isEmpty());
#ifdef Q_WS_QWS
    serverName += QLatin1String("_qws");
#endif
#ifndef Q_OS_WIN
    serverName += QString(QLatin1String("_%1_%2")).arg(getuid()).arg(getgid());
#else
    static QString login;
    if(login.isEmpty())
    {
        QT_WA({
            wchar_t buffer[256];
            DWORD bufferSize = sizeof(buffer) / sizeof(wchar_t) - 1;
            GetUserNameW(buffer, &bufferSize);
            login = QString::fromUtf16((ushort*)buffer);
        },
        {
            char buffer[256];
            DWORD bufferSize = sizeof(buffer) / sizeof(char) - 1;
            GetUserNameA(buffer, &bufferSize);
            login = QString::fromLocal8Bit(buffer);
        });
    }
    serverName += QString("_%1").arg(login);
#endif
    return serverName;
}

