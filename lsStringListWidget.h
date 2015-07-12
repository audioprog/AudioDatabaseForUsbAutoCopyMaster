#ifndef LSSTRINGLISTWIDGET_H
#define LSSTRINGLISTWIDGET_H

#include <QListWidget>

class lsStringListWidget : public QListWidget
{
	Q_OBJECT

public:
	explicit lsStringListWidget(QWidget *parent = 0);

signals:
	void signalSelected( QString selectedText );

public:

	int indexOf( const QString& searchText ) const;

	bool replace( int row, const QString& newText );

	bool select( const QString& toSelect );

	bool select( int row );

	int selectedRow() const;

	QString selectedText() const;

	QStringList strings() const;

private slots:
	void slotSelected();
};

#endif // LSSTRINGLISTWIDGET_H
