/*
 * (C) 2015 audioprog@users.noreply.github.com
 *
 ***************************************************************************
 *   This is free software; you can redistribute it and/or modify		   *
 *   it under the terms of the GNU Lesser General Public License version   *
 *   2.1 as published by the Free Software Foundation.                     *
 *                                                                         *
 *   This software is distributed in the hope that it will be useful, but  *
 *   WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA         *
 *   02110-1301  USA                                                       *
 *                                                                         *
 *   Alternatively, this file is available under the Mozilla Public        *
 *   License Version 1.1.  You may obtain a copy of the License at         *
 *   http://www.mozilla.org/MPL/                                           *
 ***************************************************************************/
#ifndef LSTEXTEDIT_H
#define LSTEXTEDIT_H

#include <QObject>

class QPainter;

class lsTextEdit : public QObject
{
    Q_OBJECT
public:
    explicit lsTextEdit(QObject *parent = 0);
    ~lsTextEdit();

	void paint( QPainter* painter );

	void setVisible(bool visible);

	QSize sizeHint() const;

protected:
	void mousePressEvent( const QPoint& pos );
	void mouseReleaseEvent( const QPoint& pos );
	void mouseDoubleClickEvent( const QPoint& pos );
	void mouseMoveEvent( const QPoint& pos );
	void keyPressEvent(QKeyEvent*);
	void keyReleaseEvent(QKeyEvent*);
	void enterEvent();
	void leaveEvent();
	void contextMenuEvent(QContextMenuEvent*);

private:
	int cursorPos;

	struct STextSection
	{
		QFont font;

		QColor color;

		QString text;

		QRectF rect;

		QRectF minRect;

		QVariant sourceId;
	};

	struct STextParagraph
	{
		Qt::Alignment alignment;

		QList<STextSection> sections;
	};

	QList<STextParagraph> paragraphs;

	QRectF rect;
};

#endif // LSTEXTEDIT_H
