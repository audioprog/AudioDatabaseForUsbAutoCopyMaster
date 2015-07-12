#ifndef PRINTINTERFACE_H
#define PRINTINTERFACE_H

class QPainter;
class QPaintEvent;

class printInterface
{
public:
	virtual void print( QPainter* painter, QPaintEvent* newPaintEvent ) = 0;
};

#endif // PRINTINTERFACE_H
