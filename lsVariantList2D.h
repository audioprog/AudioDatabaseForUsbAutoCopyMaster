#ifndef LSVARIANTLIST2D_H
#define LSVARIANTLIST2D_H

#include <QVariant>

class lsVariantList2DPrivate;

class lsVariantList2D
{
	Q_DECLARE_PRIVATE(lsVariantList2D)

public:
	lsVariantList2D( const QStringList& columns );
    ~lsVariantList2D();

	const QStringList& columns() const { return this->columnNames; }

	const int columnCount() const { return this->columnCountInt; }

	int rowCount() const { return this->dataCount / this->columnCountInt + 1; }

	const QVariant& cell( const QString& column, int row ) const;

	const QVariant& cell( int column, int row ) const;

	bool setCell( const QString& column, int row, const QVariant& newCellData );

	bool setCell( int column, int row, const QVariant& newCellData );

private:
	QVariant none;

	QStringList columnNames;

	int columnCountInt;

	QVariantList data;

	int dataCount;
};

#endif // LSVARIANTLIST2D_H
