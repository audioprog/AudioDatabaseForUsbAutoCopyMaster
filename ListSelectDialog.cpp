#include "ListSelectDialog.h"
#include "ui_ListSelectDialog.h"

ListSelectDialog::ListSelectDialog(const QStringList& toShow, QWidget* parent) :
	QDialog(parent),
	ui(new Ui::ListSelectDialog)
{
	ui->setupUi(this);
	connect(this, &ListSelectDialog::accept, this, &ListSelectDialog::deleteLater);
	connect(this, &ListSelectDialog::reject, this, &ListSelectDialog::deleteLater);

	ui->listWidget->addItems(toShow);
}

ListSelectDialog::~ListSelectDialog()
{
	delete ui;
}

void ListSelectDialog::on_ListSelectDialog_accepted()
{
	QList<QListWidgetItem*> selected = ui->listWidget->selectedItems();

	QStringList toSend;

	foreach (QListWidgetItem* item, selected)
	{
		toSend << item->text();
	}

	emit signalAccepted(toSend);
}
