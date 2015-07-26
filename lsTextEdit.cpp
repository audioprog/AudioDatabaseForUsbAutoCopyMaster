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

lsTextEdit::lsTextEdit(QObject *parent) : QObject(parent)
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
		STextParagraph& paragraph = this->paragraphs[iParagraph];

		int countSections = paragraph.sections.count();

		for (int iSection = 0; iSection < countSections; iSection++)
		{
			STextSection& section = paragraph.sections[iSection];

			painter->setFont(section.font);

			section.rect = painter->boundingRect(this->rect, Qt::TextSingleLine, section.text);
			section.minRect = painter->boundingRect(this->rect, Qt::TextSingleLine, section.text.trimmed());
		}
	}
}

