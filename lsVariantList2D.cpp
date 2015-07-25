#include "lsVariantList2D.h"

lsVariantList2D::lsVariantList2D(const QStringList& columns)
	: columnNames(columns)
	, columnCountInt(columns.count())
{
}

lsVariantList2D::~lsVariantList2D()
{
}

const QVariant& lsVariantList2D::cell(const QString& column, int row) const
{
	int columnIndex = this->columnNames.indexOf(column);
	if (columnIndex == -1)
	{
		return this->none;
	}
	if (row < 0)
	{
		return this->none;
	}

	return this->cell(columnIndex, row);
}

const QVariant& lsVariantList2D::cell(int column, int row) const
{
	if (column < 0 || column >= this->columnCountInt)
	{
		return this->none;
	}
	if (row < 0)
	{
		return this->none;
	}
	int maxRowCount = this->dataCount / this->columnCountInt;
	if (row > maxRowCount)
	{
		if (row == maxRowCount)
		{
			int lastRowColumnCount = this->dataCount - maxRowCount * this->columnCountInt;
			if (column >= lastRowColumnCount)
			{
				return this->none;
			}
		}
		else
		{
			return this->none;
		}
	}

	return this->data.value(column + row * this->columnCountInt);
}

bool lsVariantList2D::setCell(const QString& column, int row, const QVariant& newCellData)
{
	int columnIndex = this->columnNames.indexOf(column);
	if (columnIndex == -1)
	{
		return false;
	}
	if (row < 0)
	{
		return false;
	}

	return this->setCell(columnIndex, row, newCellData);
}

bool lsVariantList2D::setCell(int column, int row, const QVariant& newCellData)
{
	if (column < 0 || column >= this->columnCountInt)
	{
		return false;
	}
	if (row < 0)
	{
		return false;
	}

	int writeIndex = column + row * this->columnCountInt;

	while (this->dataCount < writeIndex)
	{
		this->data.append(QVariant());
		this->dataCount++;
	}

	if (writeIndex < this->dataCount)
	{
		this->data[writeIndex] = newCellData;
	}
	else
	{
		this->data.append(newCellData);
		this->dataCount = this->data.count();
	}

	return true;
}
