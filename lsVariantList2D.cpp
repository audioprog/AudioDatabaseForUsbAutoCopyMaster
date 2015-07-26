#include "lsVariantList2D.h"

class lsVariantList2DPrivate: public QSharedData
{
public:
	lsVariantList2DPrivate(const QStringList& columns)
		: columnNames(columns)
		, columnCountInt(columns.count())
	{}

	QVariant none;

	QStringList columnNames;

	int columnCountInt;

	QVariantList data;

	int dataCount;
};

lsVariantList2D::lsVariantList2D()
	: d(new lsVariantList2DPrivate(QStringList()))
{}

lsVariantList2D::lsVariantList2D(const QStringList& columns)
	: d(new lsVariantList2DPrivate(columns))
{}

lsVariantList2D::lsVariantList2D(const lsVariantList2D& other)
	: d(other.d)
{
}

lsVariantList2D::~lsVariantList2D()
{
}

const QStringList&lsVariantList2D::columns() const
{
	return d->columnNames;
}

const int lsVariantList2D::columnCount() const
{
	return d->columnCountInt;
}

int lsVariantList2D::rowCount() const
{
	return d->dataCount / d->columnCountInt + 1;
}

const QVariant& lsVariantList2D::cell(const QString& column, int row) const
{
	int columnIndex = d->columnNames.indexOf(column);
	if (columnIndex == -1)
	{
		return d->none;
	}
	if (row < 0)
	{
		return d->none;
	}

	return this->cell(columnIndex, row);
}

const QVariant& lsVariantList2D::cell(int column, int row) const
{
	if (column < 0 || column >= d->columnCountInt)
	{
		return d->none;
	}
	if (row < 0)
	{
		return d->none;
	}
	int maxRowCount = d->dataCount / d->columnCountInt;
	if (row > maxRowCount)
	{
		if (row == maxRowCount)
		{
			int lastRowColumnCount = d->dataCount - maxRowCount * d->columnCountInt;
			if (column >= lastRowColumnCount)
			{
				return d->none;
			}
		}
		else
		{
			return d->none;
		}
	}

	return d->data[column + row * d->columnCountInt];
}

bool lsVariantList2D::setCell(const QString& column, int row, const QVariant& newCellData)
{
	int columnIndex = d->columnNames.indexOf(column);
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
	if (column < 0 || column >= d->columnCountInt)
	{
		return false;
	}
	if (row < 0)
	{
		return false;
	}

	int writeIndex = column + row * d->columnCountInt;

	while (d->dataCount < writeIndex)
	{
		d->data.append(QVariant());
		d->dataCount++;
	}

	if (writeIndex < d->dataCount)
	{
		d->data[writeIndex] = newCellData;
	}
	else
	{
		d->data.append(newCellData);
		d->dataCount = d->data.count();
	}

	return true;
}
