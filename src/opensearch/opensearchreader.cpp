/*
 * Copyright 2009 Jakub Wieczorek <faw217@gmail.com>
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

#include "opensearchreader.h"

#include "opensearchengine.h"

#include <qiodevice.h>

/*!
    \class OpenSearchReader
    \brief A class reading a search engine description from an external source

    OpenSearchReader is a class that can be used to read search engine descriptions
    formed using the OpenSearch format.

    It inherits QXmlStreamReader and thus provides additional functions, such as
    QXmlStreamReader::error(), QXmlStreamReader::hasError() that can be used to make sure
    the reading procedure succeeded.

    For more information see:
    http://www.opensearch.org/Specifications/OpenSearch/1.1/Draft_4#OpenSearch_description_document

    \sa OpenSearchEngine, OpenSearchWriter
*/

/*!
    Constructs a new reader.

    \note One instance can be used to read multiple files, one by one.
*/
OpenSearchReader::OpenSearchReader()
    : QXmlStreamReader()
{
}

/*!
    Reads an OpenSearch engine from the \a device and returns an OpenSearchEngine object,
    filled in with all the data that has been retrieved from the document.

    If the \a device is closed, it will be opened.

    To make sure if the procedure succeeded, check QXmlStreamReader::error().

    \return a new constructed OpenSearchEngine object

    \note The function returns an object of the OpenSearchEngine class even if the document
          is bad formed or doesn't conform to the specification. It needs to be manually
          deleted afterwards, if intended.
    \note The lifetime of the returned OpenSearchEngine object is up to the user.
          The object should be deleted once it is not used anymore to avoid memory leaks.
*/
OpenSearchEngine *OpenSearchReader::read(QIODevice *device)
{
    clear();

    if (!device->isOpen())
        device->open(QIODevice::ReadOnly);

    setDevice(device);
    return read();
}

OpenSearchEngine *OpenSearchReader::read()
{
    OpenSearchEngine *engine = new OpenSearchEngine();

    while (!isStartElement() && !atEnd())
        readNext();

    if (name() != QStringLiteral("OpenSearchDescription")
            || namespaceUri() != QStringLiteral("http://a9.com/-/spec/opensearch/1.1/")) {
        raiseError(QObject::tr("The file is not an OpenSearch 1.1 file."));
        return engine;
    }

    while (!atEnd()) {
        readNext();

        if (!isStartElement())
            continue;

        if (name() == QStringLiteral("ShortName")) {
            engine->setName(readElementText());
        } else if (name() == QStringLiteral("Description")) {
            engine->setDescription(readElementText());
        } else if (name() == QStringLiteral("Url")) {

            QString type = attributes().value(QStringLiteral("type")).toString();
            QString url = attributes().value(QStringLiteral("template")).toString();
            QString method = attributes().value(QStringLiteral("method")).toString();

            if (type == QStringLiteral("application/x-suggestions+json")
                    && !engine->suggestionsUrlTemplate().isEmpty())
                continue;

            if ((type.isEmpty()
                    || type == QStringLiteral("text/html")
                    || type == QStringLiteral("application/xhtml+xml"))
                    && !engine->searchUrlTemplate().isEmpty())
                continue;

            if (url.isEmpty())
                continue;

            QList<OpenSearchEngine::Parameter> parameters;

            readNext();

            while (!(isEndElement() && name() == QStringLiteral("Url"))) {
                if (!isStartElement() || (name() != QStringLiteral("Param") && name() != QStringLiteral("Parameter"))) {
                    readNext();
                    continue;
                }

                QString key = attributes().value(QStringLiteral("name")).toString();
                QString value = attributes().value(QStringLiteral("value")).toString();

                if (!key.isEmpty() && !value.isEmpty())
                    parameters.append(OpenSearchEngine::Parameter(key, value));

                while (!isEndElement())
                    readNext();
            }

            if (type == QStringLiteral("application/x-suggestions+json")) {
                engine->setSuggestionsUrlTemplate(url);
                engine->setSuggestionsParameters(parameters);
                engine->setSuggestionsMethod(method);
            } else if (type.isEmpty() || type == QStringLiteral("text/html") || type == QStringLiteral("application/xhtml+xml")) {
                engine->setSearchUrlTemplate(url);
                engine->setSearchParameters(parameters);
                engine->setSearchMethod(method);
            }

        } else if (name() == QStringLiteral("Image")) {
            engine->setImageUrl(readElementText());
        }

        if (!engine->name().isEmpty()
                && !engine->description().isEmpty()
                && !engine->suggestionsUrlTemplate().isEmpty()
                && !engine->searchUrlTemplate().isEmpty()
                && !engine->imageUrl().isEmpty())
            break;
    }

    return engine;
}

