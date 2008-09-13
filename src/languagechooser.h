#ifndef __LANGUAGE_CHOOSER_H_
#define __LANGUAGE_CHOOSER_H_

#include <QString>
#include <QHash>
#include <QLocale>

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

#endif //__LANGUAGE_CHOOSER_H_
