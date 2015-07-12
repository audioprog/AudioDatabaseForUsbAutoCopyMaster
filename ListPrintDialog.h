#ifndef LISTPRINTDIALOG_H
#define LISTPRINTDIALOG_H

#include <QDialog>

#include "printInterface.h"

#include <QList>

namespace Ui {
class ListPrintDialog;
}

class Einzelbeitrag;


class ListPrintDialog : public QDialog, public printInterface
{
	Q_OBJECT

public:
	explicit ListPrintDialog(QWidget *parent = 0);
	~ListPrintDialog();

public:
	void print(QPainter* painter, QPaintEvent* newPaintEvent);

	void setTitleItems(QList<Einzelbeitrag*> newTitleItems) { this->titleItems = newTitleItems; }

	QString headText;

	QString date;

private:
	Ui::ListPrintDialog *ui;

	QList<Einzelbeitrag*> titleItems;

};

#endif // LISTPRINTDIALOG_H
