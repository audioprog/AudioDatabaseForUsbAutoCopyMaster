#ifndef LSKEYANDFILTERLISTEDITOR_H
#define LSKEYANDFILTERLISTEDITOR_H

#include <QWidget>

#include <QtSql>

namespace Ui {
class lsKeyAndFilterListEditor;
}

class lsKeyAndFilterListEditor : public QWidget
{
	Q_OBJECT

public:
	explicit lsKeyAndFilterListEditor(QWidget *parent = 0);
	~lsKeyAndFilterListEditor();

	void addNames( const QStringList& newNames );

	QStringList filterList() const;

	bool openTable( QSqlDatabase db, const QString& tableName );

	QStringList readSqlTable();

private slots:
	void on_listWidget_signalSelected( QString selectedText );

	void on_toolButtonSort_clicked();

	void on_toolButtonAdd_clicked();

	void on_lineEdit_textChanged(const QString &arg1);

	void on_toolButtonRemove_clicked();

	void on_toolButtonSave_clicked();

private:
	Ui::lsKeyAndFilterListEditor *ui;

	QSqlDatabase db;

	QString tableName;

	QString columnName;
};

#endif // LSKEYANDFILTERLISTEDITOR_H
