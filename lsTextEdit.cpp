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

#include "lsTextEdit.h"

#include <QPainter>

lsTextEdit::lsTextEdit(QObject *parent)
	: QObject(parent)
	, m_isRegionDirty(true)
	, m_isVisible(true)
{
}

lsTextEdit::~lsTextEdit()
{
}

void lsTextEdit::paint(QPainter* painter)
{
	int countParagraphs = this->paragraphs;
	for (int iParagraph = 0; iParagraph < countParagraphs; iParagraph++)
	{
		lsTextParagraph& paragraph = this->paragraphs[iParagraph];

		int countSections = paragraph.sections.count();

		if ( ! paragraph.isMeasured())
		{
			for (int iSection = 0; iSection < countSections; iSection++)
			{
				STextSection& section = paragraph.sections[iSection];

				if ( ! section.isMeasured)
				{
					section.isMeasured = true;

					painter->setFont(section.font);

					section.size = painter->boundingRect(this->rect, Qt::TextSingleLine, section.text);
					section.minSize = painter->boundingRect(this->rect, Qt::TextSingleLine, section.text.trimmed());
				}
			}

			paragraph.isMeasured = true;
		}


		QPointF startPoint;
		if (this->m_isRegionDirty)
		{
			QRect lineRect(this->rect.x(), this->rect.y(), this->rect.width(), paragraph.sections.first().size.height());
			QRegion lineRegion = this->region.intersected(lineRect);
			QRect lineRegion = lineRegion.boundingRect();
			//
		}
	}
}

