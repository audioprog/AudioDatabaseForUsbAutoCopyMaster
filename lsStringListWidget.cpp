#include "lsStringListWidget.h"

lsStringListWidget::lsStringListWidget(QWidget *parent) :
	QListWidget(parent)
{
	connect(this, &QListWidget::itemSelectionChanged, this, &lsStringListWidget::slotSelected);
}

int lsStringListWidget::indexOf(const QString& searchText) const
{
	for (int i = 0; i < this->count(); i++)
	{
		if (this->item(i)->text() == searchText)
		{
			return i;
		}
	}

	return -1;
}

bool lsStringListWidget::replace(int row, const QString& newText)
{
	if (row < 0 || row >= this->count()) { return false; }

	this->item(row)->setText(newText);
	return true;
}

bool lsStringListWidget::select(const QString& toSelect)
{
	int row = this->indexOf(toSelect);
	if (row == -1)
	{
		return false;
	}

	return this->select(row);
}

bool lsStringListWidget::select(int row)
{
	if (row > -1)
	{
		this->selectionModel()->clear();
		this->selectionModel()->select(this->model()->index(row, 0), QItemSelectionModel::Select);
		return true;
	}
	return false;
}

int lsStringListWidget::selectedRow() const
{
	if (this->selectedItems().isEmpty())
	{
		return -1;
	}
	else
	{
		return this->selectionModel()->selectedRows().first().row();
	}
}

QString lsStringListWidget::selectedText() const
{
	if (this->selectedItems().count() == 0)
	{
		return QStringLiteral("");
	}
	else
	{
		return this->selectedItems().first()->text();
	}
}

QStringList lsStringListWidget::strings() const
{
	QStringList toReturn;

	for (int i = 0; i < this->count(); i++)
	{
		toReturn << this->item(i)->text();
	}

	return toReturn;
}

void lsStringListWidget::slotSelected()
{
	emit signalSelected( this->selectedText() );
}
