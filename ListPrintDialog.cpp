#include "ListPrintDialog.h"
#include "ui_ListPrintDialog.h"

#include "Einzelbeitrag.h"

#include <QPainter>
#include <QPaintEvent>

ListPrintDialog::ListPrintDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::ListPrintDialog)
{
	ui->setupUi(this);

	ui->printBox->installPrintClass(this);
}

ListPrintDialog::~ListPrintDialog()
{
	delete ui;
}

void ListPrintDialog::print(QPainter* painter, QPaintEvent* )
{
	QRect rect(QPoint(0,0), ui->printBox->size());

	painter->fillRect(rect, Qt::white);

	QHash<int,int> columnWidhs;

	foreach (Einzelbeitrag* titleItem, this->titleItems)
	{
		int width = painter->boundingRect(rect, QString::number(titleItem->titelNr())).width() + 5;
		columnWidhs[0] = qMax(columnWidhs[0], width);
		width = painter->boundingRect(rect, titleItem->name()).width() + 5;
		columnWidhs[1] = qMax(columnWidhs[1], width);
	}

	QRect retBoundingRect;

	painter->drawText(rect, Qt::AlignLeft | Qt::AlignTop, this->date + "  " + this->headText, &retBoundingRect);

	int nextLineY = retBoundingRect.bottom() + 5;

	foreach (Einzelbeitrag* titleItem, this->titleItems)
	{
		int currentLineY = nextLineY;

		painter->drawText(0, currentLineY, columnWidhs.value(0), rect.height() - currentLineY,
						  Qt::AlignLeft | Qt::AlignTop, QString::number(titleItem->titelNr()), &retBoundingRect);
		nextLineY = qMax(nextLineY, retBoundingRect.bottom() + 5);


		painter->drawText(columnWidhs.value(0) + 5, currentLineY, columnWidhs.value(1), rect.height() - currentLineY,
						  Qt::AlignLeft | Qt::AlignTop, titleItem->name(), &retBoundingRect);
		nextLineY = qMax(nextLineY, retBoundingRect.bottom() + 5);

		int x = columnWidhs.value(0) + columnWidhs.value(1) + 10;

		painter->drawText(x, currentLineY, rect.width() - x, rect.height() - currentLineY,
						  Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, titleItem->text() + " " + titleItem->bibelstelle(), &retBoundingRect);
		nextLineY = qMax(nextLineY, retBoundingRect.bottom() + 5);
	}
}
