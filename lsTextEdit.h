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

class QContextMenuEvent;
class QKeyEvent;
class QPainter;

struct STextSection
{
	STextSection() : isMeasured(false) {}

	QColor color;

	QFont font;

	bool isMeasured;

	QSizeF minSize;

	QPointF pos;

	QSizeF size;

	QVariant sourceId;

	QString text;
};

class lsTextParagraph
{
	friend class lsTextEdit;

public:
	lsTextParagraph() : alignment(Qt::AlignLeft) {}

	bool isMeasured() const { return isMeasured; }

	inline void setMeasureDirty() { for (int i = 0; i < sections.count(); i++) { sections[i].isMeasured = false; } isMeasured = false; }

	Qt::Alignment alignment;

	QList<STextSection> sections;

private:
	bool isMeasured;
};

class lsTextEdit : public QObject
{
    Q_OBJECT

public:
    explicit lsTextEdit(QObject *parent = 0);
    ~lsTextEdit();

	bool isVisible() const { return this->m_isVisible; }

	void paint( QPainter* painter );

	void setVisible( bool visible ) {
		this->m_isVisible = visible;
	}

	QRectF usedArea() const { return this->m_usedArea; }

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

	bool m_isVisible;

	bool m_isRegionDirty;

	QRectF m_usedArea;

	QList<lsTextParagraph> paragraphs;

	QRectF rect;

	QRegion region;
};

#endif // LSTEXTEDIT_H
