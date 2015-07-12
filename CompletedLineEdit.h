#ifndef COMPLETEDLINEEDIT_H
#define COMPLETEDLINEEDIT_H

#include <QLineEdit>
#include <QtSql>

class QAbstractItemModel;

class CompletedLineEdit : public QLineEdit
{
	Q_OBJECT

public:
	CompletedLineEdit(QWidget *parent = 0);
	~CompletedLineEdit();

	void setCompleter(QCompleter *c);
	QCompleter *completer() const;

	void createFromSimpleDatabaseTable( QSqlDatabase db, const QString& tableName );

	void readWordsAndCreateCompleter( bool isReread = false );

protected:
	void keyPressEvent(QKeyEvent *e) Q_DECL_OVERRIDE;
	void focusInEvent(QFocusEvent *e) Q_DECL_OVERRIDE;

private slots:
	void insertCompletion(const QString &completion);

private:
	QString textUnderCursor() const;
	QPair<int, int> wordRangeUnderCursor() const;

private:
	QCompleter *c;

	static QMap<QString,QStringList> completerList;

	QSqlDatabase db;

	QString tableName;
};

#endif // COMPLETEDLINEEDIT_H
