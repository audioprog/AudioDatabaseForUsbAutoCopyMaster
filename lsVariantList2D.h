#ifndef LSVARIANTLIST2D_H
#define LSVARIANTLIST2D_H

#include <QVariant>

#include <QSharedData>

class lsVariantList2DPrivate;

class lsVariantList2D
{
public:
	explicit lsVariantList2D();
	explicit lsVariantList2D( const QStringList& columns );
	lsVariantList2D( const lsVariantList2D& other );
    ~lsVariantList2D();

	const QStringList& columns() const;

	const int columnCount() const;

	int rowCount() const;

	const QVariant& cell( const QString& column, int row ) const;

	const QVariant& cell( int column, int row ) const;

	bool setCell( const QString& column, int row, const QVariant& newCellData );

	bool setCell( int column, int row, const QVariant& newCellData );

private:
	QExplicitlySharedDataPointer<lsVariantList2DPrivate> d;
};

#endif // LSVARIANTLIST2D_H
