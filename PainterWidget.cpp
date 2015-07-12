#include "PainterWidget.h"

#include <QPainter>

PainterWidget::PainterWidget(QWidget *parent) : QWidget(parent)
{
	// Font Size Factor 2.6455
}

PainterWidget::~PainterWidget()
{
}

void PainterWidget::paintEvent(QPaintEvent* newPaintEvent)
{
	QPainter painter(this);
	this->currentPrintInterface->print(&painter, newPaintEvent);
}

