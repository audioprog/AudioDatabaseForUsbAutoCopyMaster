#ifndef PAINTERWIDGET_H
#define PAINTERWIDGET_H

#include <QWidget>

#include "printInterface.h"

class PainterWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PainterWidget(QWidget *parent = 0);
    ~PainterWidget();

	void installPrintClass( printInterface* newPrintInterface ) { this->currentPrintInterface = newPrintInterface; }

	// QWidget interface
protected:
	void paintEvent( QPaintEvent* newPaintEvent );

	printInterface* currentPrintInterface;
};

#endif // PAINTERWIDGET_H
