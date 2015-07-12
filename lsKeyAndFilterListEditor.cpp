#include "lsKeyAndFilterListEditor.h"
#include "ui_lsKeyAndFilterListEditor.h"

lsKeyAndFilterListEditor::lsKeyAndFilterListEditor(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::lsKeyAndFilterListEditor)
{
	ui->setupUi(this);
}

lsKeyAndFilterListEditor::~lsKeyAndFilterListEditor()
{
	delete ui;
}

void lsKeyAndFilterListEditor::addNames(const QStringList& newNames)
{
	QSqlQuery query(this->db);

	foreach (const QString& newName, newNames)
	{
		query.exec(QStringLiteral("INSERT INTO %1 (Name) VALUES ('%2')").arg(this->tableName).arg(newName));
	}
}

QStringList lsKeyAndFilterListEditor::filterList() const
{
	return this->ui->listWidget->strings();
}

bool lsKeyAndFilterListEditor::openTable(QSqlDatabase db, const QString& tableName)
{
	this->db = db;
	this->tableName = tableName;

	QStringList newData = this->readSqlTable();

	ui->listWidget->addItems(newData);

	return true;
}

QStringList lsKeyAndFilterListEditor::readSqlTable()
{
	QSqlQuery query(this->db);

	QString q = QStringLiteral("SELECT * FROM %1").arg(this->tableName);

	ui->listWidget->clear();
	ui->lineEdit->clear();

	QStringList newData;

	if (query.exec(q))
	{
		this->columnName = query.record().fieldName(0);

		while (query.next())
		{
			QString row = query.value(0).toString();
			if ( ! newData.contains(row) )
			{
				newData << row;
			}
		}
	}
	else
	{
		QSqlRecord record = db.record(this->tableName);
		if (record.count() == 1)
		{
			this->columnName = record.fieldName(0);
		}
		else
		{
			this->setEnabled(false);
		}
	}

	return newData;
}

void lsKeyAndFilterListEditor::on_listWidget_signalSelected(QString selectedText)
{
	ui->lineEdit->blockSignals(true);
	ui->lineEdit->setText(selectedText);
	ui->lineEdit->blockSignals(false);
}

void lsKeyAndFilterListEditor::on_toolButtonSort_clicked()
{
	QStringList list = ui->listWidget->strings();
	list.sort(Qt::CaseInsensitive);
	ui->listWidget->clear();
	ui->listWidget->addItems(list);
}

void lsKeyAndFilterListEditor::on_toolButtonAdd_clicked()
{
	if (ui->listWidget->selectedRow() == -1)
	{
		if (ui->listWidget->indexOf(ui->lineEdit->text()) > -1)
		{
			ui->listWidget->select(ui->lineEdit->text());
		}
		else
		{
			ui->listWidget->addItem(ui->lineEdit->text());
		}
	}
	else
	{
		ui->lineEdit->blockSignals(true);
		ui->lineEdit->setText(QStringLiteral(""));
		ui->lineEdit->blockSignals(false);
		int row = ui->listWidget->indexOf(QStringLiteral(""));
		if (row == -1)
		{
			ui->listWidget->addItem(QStringLiteral(""));
			ui->listWidget->select(QStringLiteral(""));
		}
		else
		{
			ui->listWidget->select(QStringLiteral(""));
		}
	}
}

void lsKeyAndFilterListEditor::on_lineEdit_textChanged(const QString &arg1)
{
	int row = ui->listWidget->selectedRow();
	if (row > -1)
	{
		ui->listWidget->replace(row, arg1);
	}
}

void lsKeyAndFilterListEditor::on_toolButtonRemove_clicked()
{
	int row = ui->listWidget->selectedRow();
	if (row > -1)
	{
		delete ui->listWidget->takeItem(row);
	}
}

void lsKeyAndFilterListEditor::on_toolButtonSave_clicked()
{
	QStringList list = ui->listWidget->strings();

	{
		QSqlQuery query(this->db);
		query.exec(QStringLiteral("DELETE FROM %1").arg(this->tableName));
	}
	foreach (const QString& item, list)
	{
		if ( ! item.simplified().isEmpty())
		{
			QSqlQuery query(this->db);
			QString insert = QStringLiteral("INSERT INTO %1(%2) VALUES ('%3')").arg(this->tableName).arg(this->columnName).arg(QString(item).replace("\\", "\\\\"));
			query.exec(insert);
		}
	}
}
