/*
 * Copyright 2009 Benjamin K. Stuhl <bks24@cornell.edu>
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

#include "historycompleter.h"

HistoryCompletionModel::HistoryCompletionModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    setDynamicSortFilter(true);

    // do a case-insensitive, full-text substring match -- but don't
    // allow the user to accidentally use regexp metacharacters
    m_searchMatcher.setCaseSensitivity(Qt::CaseInsensitive);
    m_searchMatcher.setPatternSyntax(QRegExp::FixedString);
}

QVariant HistoryCompletionModel::data(const QModelIndex &index, int role) const
{
    // tell QCompleter that everything we have matches what the user typed
    if (role == HistoryCompletionRole && index.isValid())
        return QLatin1String("a");

    // show urls, not titles
    if (role == Qt::DisplayRole)
        role = HistoryModel::UrlStringRole;

    return QSortFilterProxyModel::data(index, role);
}

QString HistoryCompletionModel::searchString() const
{
    return m_searchString;
}

void HistoryCompletionModel::setSearchString(const QString& str)
{
    if (str == m_searchString)
        return;

    m_searchString = str;
    m_searchMatcher.setPattern(str);
    invalidateFilter();
}

bool HistoryCompletionModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    // do a substring match against both the url and title
    QModelIndex idx = sourceModel()->index(source_row, 0, source_parent);
    QString url = sourceModel()->data(idx, HistoryModel::UrlStringRole).toString();

    if (m_searchMatcher.indexIn(url) != -1)
        return true;

    QString title = sourceModel()->data(idx, HistoryModel::TitleRole).toString();

    if (m_searchMatcher.indexIn(title) != -1)
        return true;

    return false;
}

bool HistoryCompletionModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    int frecency_l = sourceModel()->data(left, HistoryFilterModel::FrecencyRole).toInt();
    int frecency_r = sourceModel()->data(right, HistoryFilterModel::FrecencyRole).toInt();

    // sort descending in frecency
    return (frecency_r < frecency_l);
}

HistoryCompleter::HistoryCompleter(QObject *parent)
    : QCompleter(parent)
{
    setCompletionRole(HistoryCompletionModel::HistoryCompletionRole);
}

HistoryCompleter::HistoryCompleter(QAbstractItemModel *m, QObject *parent)
    : QCompleter(m, parent)
{
    // we want to complete against our own faked role
    setCompletionRole(HistoryCompletionModel::HistoryCompletionRole);

    // and we are effectively case-sensitively sorted, since we
    // want QCompleter to do case-sensitive matching against our
    // faked completion role
    setModelSorting(QCompleter::CaseSensitivelySortedModel);
}

QString HistoryCompleter::pathFromIndex(const QModelIndex &index) const
{
    // we want to return the actual url from the history for the
    // data the QCompleter finally returns
    return model()->data(index, HistoryModel::UrlStringRole).toString();
}

QStringList HistoryCompleter::splitPath(const QString &path) const
{
    HistoryCompletionModel *hcm = qobject_cast<HistoryCompletionModel *>(model());
    Q_ASSERT(hcm);

    // tell the HistoryCompletionModel about the new prefix
    hcm->setSearchString(path);

    // and re-sort it
    hcm->sort(0);

    // the actual filtering is done by the HistoryCompletionModel; we just
    // return a short dummy here so that QCompleter thinks we match everything
    return QStringList() << QLatin1String("a");
}
