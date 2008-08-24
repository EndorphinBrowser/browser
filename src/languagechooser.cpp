#include "languagechooser.h"

#include <QList>
#include <QDir>
#include <QFileInfoList>
#include <QFileInfo>
#include <QHash>
#include <QLocale>
#include <QApplication>
#include <QLibraryInfo>
#include <QLatin1String>
#include <QInputDialog>
#include <QRegExp>
#include <QDebug>

// private class definition
class LanguageChooser::Private
{
public:
	Private( LanguageChooser *parent );
	~Private();
	
	void	loadUpAvailableLangs();
	bool	isLanguageAvailable( const QString &lang ) const;
	
	LanguageChooser		*m_parent;
	QHash<QString,QLocale>	m_langs;
	QString			m_currentLang;
};

// private class implementation

// TODO do we really need *parent
LanguageChooser::Private::Private( LanguageChooser *parent )
{
	m_parent = parent;
	
	loadUpAvailableLangs();
}

// TODO is it needed?
LanguageChooser::Private::~Private()
{
}

/// Used to initialize the internal language list
/// Will look for all *.qm files in the data directory of Arora, and will try to 
/// see if a translation for Qt exists.
/// The only languages that are "valid" are those who exists on both dirs. If 
/// only a Qt translation exist - we cannot use it. If only Arora translation 
/// exists - we cannot use it.
void LanguageChooser::Private::loadUpAvailableLangs()
{
	QString appLangsDirName = m_parent->dataDirectory() + QDir::separator() + QLatin1String("locale");
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
bool	LanguageChooser::Private::isLanguageAvailable( const QString &lang  ) const
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

// public class
LanguageChooser::LanguageChooser()
	:d( new LanguageChooser::Private(this) )
{
	// what else is needed?
}

LanguageChooser::~LanguageChooser()
{
	delete d;
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
	int counter = 0;
	
	foreach(QLocale l, d->m_langs)
	{
		QString s = d->m_langs.key(l);
		if (s == d->m_currentLang)
			defaultItem = items.count();
		s = QString( QLatin1String("%1 (%2) %3") )
			.arg(QLocale::languageToString(l.language()))
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
		
	// now, lets see which item has been choosen
	QRegExp regExp(  QLatin1String("\\((\\w+)\\)") );
	if (regExp.indexIn(item) == -1)
	{
		// this is BAD, the string did not match!
		qDebug()
			<< __FILE__ << ":" << __LINE__
			<< "Something bad happed, the language was not chosen from the combobox"; 
		return false;
	}
	
	QString newLang = regExp.cap(1);
	
	if (!d->isLanguageAvailable(newLang))
	{
		qDebug()
			<< __FILE__ << ":" << __LINE__
			<< "Something bad happed, choosen a non exising language: " 
			<< newLang;
		return false;
	}
	QLocale l3(newLang);
	newLang = d->m_langs.key(l3);
// 	qDebug() << "Choosen " << newLang;
		
	d->m_currentLang = newLang;
	return true;
}

void LanguageChooser::setCurrentLanguage( const QString &name )
{
	// TODO is this a valid language...?
	d->m_currentLang = name;
}

QString LanguageChooser::currentLanguage()
{
	if (!d->m_currentLang.isEmpty())
		return d->m_currentLang;
	
	const QString sysLanguage = QLocale::system().name();
	if (d->isLanguageAvailable(sysLanguage))
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

