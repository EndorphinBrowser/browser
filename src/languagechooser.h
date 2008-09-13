#ifndef LANGUAGE_CHOOSER_H
#define LANGUAGE_CHOOSER_H

#include <qstring.h>
#include <qhash.h>
#include <qlocale.h>

class LanguageChooser
{
public:
	LanguageChooser();
	~LanguageChooser();
	
	bool	getLanguageFromUser();
	void	setCurrentLanguage( const QString &name );
	QString	currentLanguage();
	QString	dataDirectory() const;
	void	loadUpAvailableLangs();
	bool	isLanguageAvailable( const QString &lang ) const;

private:
	QHash<QString,QLocale>	m_langs;
	QString			m_currentLang;
};

#endif //LANGUAGE_CHOOSER_H
