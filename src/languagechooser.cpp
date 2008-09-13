#include "languagechooser.h"

#include <qapplication.h>
#include <qdir.h>
#include <qdebug.h>
#include <qfileinfo.h>
#include <qlibraryinfo.h>
#include <qlocale.h>
#include <qinputdialog.h>
#include <qregexp.h>

// public class
LanguageChooser::LanguageChooser()
{
	loadUpAvailableLangs();
}

LanguageChooser::~LanguageChooser()
{
}

bool LanguageChooser::getLanguageFromUser()
{
	QStringList items;
	QLatin1String("Winter");
	QLatin1String message(
		"<p>You can run Arora with a different language <br>"
		"then the operating system default.</p>"
		"<p>Please choose the language which should be used for Arora</p>");
	
	bool ok;
	int defaultItem = 0;
	
	QString systemLocaleString = QLocale::system().name();
	systemLocaleString = qApp->tr("System locale (%1) %2")
		.arg(systemLocaleString)
		// this is for pretty RTL support, don't ask
		.arg(QChar(0x200E)); // LRM = 0x200E;
	items << systemLocaleString;
	
	foreach(QLocale l, m_langs)
	{
		QString s = m_langs.key(l);
		if (s == m_currentLang)
			defaultItem = items.count();
		s = QString( QLatin1String("%1, %2 (%3) %4") )
			.arg(QLocale::languageToString(l.language()))
			.arg(QLocale::countryToString(l.country()))
			.arg(s)
			// this is for pretty RTL support, don't ask
			.arg(QChar(0x200E) // LRM = 0x200E
		);
		
		items << s;
	}
	
	QString item = QInputDialog::getItem(0,
		QLatin1String("Choose language"), message, 
		items, defaultItem, false, &ok
	);
	
	if (!ok)
		return false;
	
	if (item == systemLocaleString)
	{	// user choose to use the system locale
		m_currentLang.clear();
	}
	else
	{	// the user specified a specific locale
		// lets see which item has been choosen
		QRegExp regExp( QLatin1String("\\((\\w+)\\)") );
		if (regExp.indexIn(item) == -1)
		{
			// this is BAD, the string did not match!
			qDebug()
				<< __FILE__ << ":" << __LINE__
				<< "Something bad happed, the language was not chosen from the combobox"; 
			return false;
		}
	
		QString newLang = regExp.cap(1);
	
		if (!isLanguageAvailable(newLang))
		{
			qDebug()
				<< __FILE__ << ":" << __LINE__
				<< "Something bad happed, choosen a non exising language: " 
				<< newLang;
			return false;
		}
		QLocale l3(newLang);
		newLang = m_langs.key(l3);
	// 	qDebug() << "Choosen " << newLang;
		
		m_currentLang = newLang;
	}
	
	return true;
}

void LanguageChooser::setCurrentLanguage( const QString &name )
{
	// TODO is this a valid language...?
	m_currentLang = name;
}

QString LanguageChooser::currentLanguage()
{
	if (!m_currentLang.isEmpty())
		return m_currentLang;
	
	const QString sysLanguage = QLocale::system().name();
	if (isLanguageAvailable(sysLanguage))
		return sysLanguage;
	else
		return QString();
}

/// Get the directory to read the applications borrowed from 
/// BrowserApplication::dataDirectory()
// TODO how should we refactor this?
QString LanguageChooser::dataDirectory() const
{
	#if defined(Q_WS_X11)
	return QLatin1String(PKGDATADIR);
	#else
	return qApp->applicationDirPath();
	#endif
}

/// Used to initialize the internal language list
/// Will look for all *.qm files in the data directory of Arora, and will try to 
/// see if a translation for Qt exists.
/// The only languages that are "valid" are those who exists on both dirs. If 
/// only a Qt translation exist - we cannot use it. If only Arora translation 
/// exists - we cannot use it.
void LanguageChooser::loadUpAvailableLangs()
{
	QString appLangsDirName = dataDirectory() + QDir::separator() + QLatin1String("locale");
	QString sysLangsDirName = QLibraryInfo::location(QLibraryInfo::TranslationsPath) + QDir::separator();
	
	QDir appLangsDir( appLangsDirName );
	QFileInfoList list;
	QStringList filters;
	 
	filters  << QLatin1String("*.qm");
	appLangsDir.setNameFilters( filters );
	list = appLangsDir.entryInfoList();
	
	for (int i = 0; i < list.size(); ++i) 
	{
		QFileInfo	appFileInfo = list.at(i);
		QString		lang = appFileInfo.baseName();
		QFileInfo	sysFileInfo(sysLangsDirName + QLatin1String("qt_") + lang + QLatin1String(".qm") );
		
// 		if (!sysFileInfo.exists())
// 			continue;
		m_langs[lang] = QLocale(lang);
	}
}

/// Checks if a language is available for Arora to load
bool	LanguageChooser::isLanguageAvailable( const QString &lang  ) const
{
	bool found = false;
	QLocale l1(lang);
	foreach(QLocale l2, m_langs)
	{
		if (l1 == l2)
		{
			found = true;
			break;
		}
	}
	return found;
}
