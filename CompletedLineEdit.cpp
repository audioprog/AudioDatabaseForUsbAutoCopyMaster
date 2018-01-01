#include "CompletedLineEdit.h"

#include <QAbstractItemView>
#include <QApplication>
#include <QCompleter>
#include <QKeyEvent>
#include <QScrollBar>
#include <QStringListModel>

QMap<QString,QStringList> CompletedLineEdit::completerList;

CompletedLineEdit::CompletedLineEdit(QWidget *parent)
	: QLineEdit(parent)
	, c(0)
{
}

CompletedLineEdit::~CompletedLineEdit()
{
}


//! [2]
void CompletedLineEdit::setCompleter(QCompleter *completer)
{
	if (c)
		QObject::disconnect(c, 0, this, 0);

	c = completer;

	if (!c)
		return;

	c->setWidget(this);
	c->setCompletionMode(QCompleter::PopupCompletion);
	c->setCaseSensitivity(Qt::CaseInsensitive);
	QObject::connect(c, SIGNAL(activated(QString)),
					 this, SLOT(insertCompletion(QString)));
}
//! [2]

//! [3]
QCompleter *CompletedLineEdit::completer() const
{
	return c;
}

void CompletedLineEdit::readWordsAndCreateCompleter(bool isReread)
{
	QSqlQuery query(this->db);

	if (isReread || ! CompletedLineEdit::completerList.contains(this->tableName))
	{
		QString q = QStringLiteral("SELECT * FROM %1").arg(this->tableName);

		if ( ! query.exec(q))
		{
			return;
		}
	}

	if (NULL == this->c)
	{
		this->setCompleter(new QCompleter(this));
	}

#ifndef QT_NO_CURSOR
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
#endif
	QStringList words;

	if ( ( ! isReread ) && CompletedLineEdit::completerList.contains(this->tableName))
	{
		words = CompletedLineEdit::completerList.value(this->tableName);
	}
	else
	{
		while (query.next())
		{
			QString row = query.value(0).toString();
			if ( ! words.contains(row) )
			{
				words << row;
			}
		}

		CompletedLineEdit::completerList[this->tableName] = words;
	}

#ifndef QT_NO_CURSOR
	QApplication::restoreOverrideCursor();
#endif
	QAbstractItemModel* model = new QStringListModel(words, this->c);
	this->c->setModel(model);
	this->c->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
	this->c->setCaseSensitivity(Qt::CaseInsensitive);
	this->c->setFilterMode(Qt::MatchContains);
	this->c->setWrapAround(false);
}

bool CompletedLineEdit::event(QEvent* event)
{
	if (event->type() != QEvent::KeyPress)
	{
		return QLineEdit::event(event);
	}

	if (reinterpret_cast<QKeyEvent*>(event)->key() != Qt::Key_Tab)
	{
		return QLineEdit::event(event);
	}

    if (c && c->popup() && c->popup()->isVisible() && c->popup()->currentIndex().isValid())
	{
		insertCompletion(c->popup()->model()->data(c->popup()->currentIndex()).toString());
		c->popup()->hide();
		event->accept();
		return true;
	}
	else
	{
		return QLineEdit::event(event);
	}

	return false;
}

void CompletedLineEdit::createFromSimpleDatabaseTable(QSqlDatabase db, const QString& tableName)
{
	this->db = db;
	this->tableName = tableName;

	this->readWordsAndCreateCompleter();
}
//! [3]

//! [4]
void CompletedLineEdit::insertCompletion(const QString& completion)
{
	if (c->widget() != this)
		return;

	QPair<int,int> range = this->wordRangeUnderCursor();

	int extra = range.second - range.first;

	this->setSelection(range.first, extra);

	this->insert(completion);
}
//! [4]

QPair<int,int> CompletedLineEdit::wordRangeUnderCursor() const
{
	if (selectionStart() > -1)
	{
		return QPair<int,int>(-1, -1);
	}
	int start = this->cursorPosition();
	QString text = this->text();

	if (start < 0)
	{
		start = 0;
	}

	if (text.isEmpty())
	{
		return QPair<int,int>(-1, -1);
	}

	static QString eow(" ~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-="); // end of word

	for (int i = start > 0 ? start - 1 : 0; i > -1; i--)
	{
		const QChar& chr = text.at(i);
		if (eow.contains(chr))
		{
			break;
		}
		else
		{
			start = i;
		}
	}

	int end = start > -1 ? start : 0;
	for (int i = start; i < text.count(); i++)
	{
		const QChar& chr = text.at(i);
		if (eow.contains(chr))
		{
			break;
		}
		else
		{
			end = i + 1;
		}
	}

	return QPair<int,int>(start, end);
}

//! [5]
QString CompletedLineEdit::textUnderCursor() const
{
	QPair<int,int> wordRange = this->wordRangeUnderCursor();

	if (wordRange.first < 0)
	{
		return "";
	}

	return this->text().mid(wordRange.first, wordRange.second - wordRange.first);
}
//! [5]

//! [6]
void CompletedLineEdit::focusInEvent(QFocusEvent *e)
{
	if (c)
		c->setWidget(this);
	QLineEdit::focusInEvent(e);
}
//! [6]

//! [7]
void CompletedLineEdit::keyPressEvent(QKeyEvent *e)
{
	if (c && c->popup()->isVisible()) {
		// The following keys are forwarded by the completer to the widget
	   switch (e->key()) {
	   case Qt::Key_Enter:
	   case Qt::Key_Return:
	   case Qt::Key_Escape:
	   case Qt::Key_Tab:
	   case Qt::Key_Backtab:
			e->ignore();
			return; // let the completer do default behavior
	   default:
		   break;
	   }
	}

	bool isShortcut = ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_E); // CTRL+E
	if (!c || !isShortcut) // do not process the shortcut when we have a completer
		QLineEdit::keyPressEvent(e);
//! [7]

//! [8]
	const bool ctrlOrShift = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
	if (!c || (ctrlOrShift && e->text().isEmpty()))
		return;

	static QString eow("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-="); // end of word
	bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
	QString completionPrefix = textUnderCursor();

	if (!isShortcut && (hasModifier || e->text().isEmpty()|| completionPrefix.length() < 1
					  || eow.contains(e->text().right(1)))) {
		c->popup()->hide();
		return;
	}

	if (completionPrefix != c->completionPrefix()) {
		c->setCompletionPrefix(completionPrefix);
		c->popup()->setCurrentIndex(c->completionModel()->index(0, 0));
	}
	QRect cr = cursorRect();
	cr.setWidth(c->popup()->sizeHintForColumn(0)
				+ c->popup()->verticalScrollBar()->sizeHint().width());
	c->complete(cr); // popup it up!
}
//! [8]
