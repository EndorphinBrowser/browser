#ifndef LANGUAGE_MANAGER_H
#define LANGUAGE_MANAGER_H

#include <qstring.h>
#include <qhash.h>
#include <qlocale.h>
#include <qobject.h>

// this class inherits QObject for "garbage collector" reasons
// no need to define it as a Q_OBJECT :)
class LanguageManager: public QObject
{
public:
	LanguageManager();
	~LanguageManager();
	
	bool	getLanguageFromUser();
	void	setCurrentLanguage( const QString &name );
	QString	currentLanguage();
	void	loadUpAvailableLangs();
	bool	isLanguageAvailable( const QString &lang ) const;

private:
	QHash<QString,QLocale>	m_langs;
	QString			m_currentLang;
};

#endif //LANGUAGE_MANAGER_H
