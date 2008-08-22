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
#include <QDebug>

// private class definition
class LanguageChooser::Private
{
public:
	Private( LanguageChooser *parent );
	~Private();
	
	void	loadUpAvailableLangs();
	QString	dataDirectory() const;
	
	LanguageChooser		*m_parent;
	QHash<QString,QLocale>	m_langs;
	QString			currentLang;
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
		
		if (!sysFileInfo.exists())
			continue;
			
		qDebug() << "Language [" << lang << "] is available";
		m_langs[lang] = QLocale(lang);
	}
}

/// Get the directory to read the applications borrowed from 
/// BrowserApplication::dataDirectory()
// TODO how should we refactor this?
QString LanguageChooser::Private::dataDirectory() const
{
	#if defined(Q_WS_X11)
	return QLatin1String(PKGDATADIR);
	#else
	return qApp->applicationDirPath();
	#endif
}

// public class
LanguageChooser::LanguageChooser()
	:d( new LanguageChooser::Private(this) )
{
	// what else is needed?
}

LanguageChooser::~LanguageChooser()
{
}

bool LanguageChooser::getLanguageFromUser()
{
}

void LanguageChooser::setCurrentLanguage( const QString &name )
{
	// TODO is this a valid language...?
	d->currentLang = name;
}

QString LanguageChooser::getCurrentLanguage()
{
	return d->currentLang;
}
