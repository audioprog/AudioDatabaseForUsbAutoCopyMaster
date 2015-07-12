#ifndef FOLDERSEARCHER_H
#define FOLDERSEARCHER_H

#include <QObject>

#include <QDate>
#include <QFileInfo>
#include <QFuture>

struct sFolderData
{
	QFileInfo folder;
	QDate	date;
	QString daytime;
};

class FolderSearcher : public QObject
{
	Q_OBJECT
public:
	explicit FolderSearcher(QObject *parent = 0);

private:
	QFuture<sFolderData> futur;

};

#endif // FOLDERSEARCHER_H
