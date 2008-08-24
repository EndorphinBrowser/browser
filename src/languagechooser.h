#ifndef __LANGUAGE_CHOOSER_H_
#define __LANGUAGE_CHOOSER_H_

#include <QString>

class LanguageChooser
{
public:
	LanguageChooser();
	~LanguageChooser();
	
	bool getLanguageFromUser();
	void setCurrentLanguage( const QString &name );
	QString currentLanguage();
	QString	dataDirectory() const;
	
private:
	class Private;
	Private /*const */*d;
};

#endif //__LANGUAGE_CHOOSER_H_
