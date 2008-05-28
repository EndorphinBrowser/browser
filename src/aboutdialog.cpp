/*
 * Copyright 2007-2008 Benjamin C. Meyer <ben@meyerhome.net>
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

#include "aboutdialog.h"

#include <qfile.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qtextedit.h>
#include <qtextstream.h>
#include <qdialogbuttonbox.h>

AboutDialog::AboutDialog(QWidget *parent) : QDialog(parent)
{
    setupUi(this);
}

void AboutDialog::displayFile(const QString& fileName, const QString& title)
{
    QDialog *dialog = new QDialog(this);
    QLayout *layout = new QVBoxLayout(dialog);
    QTextEdit *textEdit = new QTextEdit(dialog);
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Close, Qt::Horizontal, dialog);
    QFont font;

    textEdit->setStyleSheet(QLatin1String("font-family: monospace"));
    textEdit->setPlainText(loadText(fileName));
    connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(close()));
    buttonBox->setCenterButtons(true);
    layout->addWidget(textEdit);
    layout->addWidget(buttonBox);

    dialog->setLayout(layout);
    dialog->setWindowTitle(title);
    dialog->resize(600, 350);
    dialog->exec();
}

QString AboutDialog::loadText(const QString& fileName)
{
    QString text;
    QFile f(fileName);

    if (f.open(QIODevice::ReadOnly))
    {
        text = QTextStream(&f).readAll();
    }

    return text;
}

void AboutDialog::on_authorsButton_clicked()
{
    displayFile(QLatin1String(":AUTHORS"), tr("Authors"));
}

void AboutDialog::on_licenseButton_clicked()
{
    displayFile(QLatin1String(":LICENSE.GPL3"), tr("License"));
}

void AboutDialog::on_closeButton_clicked()
{
    close();
}
