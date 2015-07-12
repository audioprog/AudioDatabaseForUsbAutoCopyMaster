#ifndef LISTSELECTDIALOG_H
#define LISTSELECTDIALOG_H

#include <QDialog>

namespace Ui {
class ListSelectDialog;
}

class ListSelectDialog : public QDialog
{
	Q_OBJECT

public:
	explicit ListSelectDialog( const QStringList& toShow, QWidget *parent = 0 );
	~ListSelectDialog();

signals:
	void signalAccepted( QStringList accepted );

private slots:
	void on_ListSelectDialog_accepted();

private:
	Ui::ListSelectDialog *ui;
};

#endif // LISTSELECTDIALOG_H
