#include "lsGlobalSettings.h"
#include "ui_lsGlobalSettings.h"

#include <QFileDialog>
#include <QSettings>

lsGlobalSettings* lsGlobalSettings::self = NULL;

lsGlobalSettings::lsGlobalSettings(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::lsGlobalSettings)
{
	ui->setupUi(this);

	self = this;
}

lsGlobalSettings::~lsGlobalSettings()
{
	self = Q_NULLPTR;
	delete ui;
}

QString lsGlobalSettings::capturePath()
{
	if (self == NULL) { return QStringLiteral(""); }

	return self->ui->lineEditCapturePath->text();
}

QString lsGlobalSettings::copyCenterServicePath()
{
	if (self == Q_NULLPTR) { return QString(); }

	return self->ui->lineEditCopyCenter->text();
}

QString lsGlobalSettings::copyCenterSubPathFormat()
{
	if (self == Q_NULLPTR) { return QString(); }

	return self->ui->lineEditCopyCenterPathFormat->text();
}

void lsGlobalSettings::load()
{
	QSettings settings;
	settings.beginGroup(QStringLiteral("Global"));

	ui->lineEditMp3Path->setText(settings.value(QStringLiteral("Mp3Path"), QStringLiteral("D:/MP3")).toString());
	ui->lineEditCapturePath->setText(settings.value(QStringLiteral("CapturePath"), QStringLiteral("E:/Gottesdienste Aufnahme")).toString());
	ui->lineEditCopyCenter->setText(settings.value(QStringLiteral("CopyCenterServicePath")).toString());
	ui->lineEditCopyCenterPathFormat->setText(settings.value(QStringLiteral("CopyCenterPathFormat")).toString());

	settings.endGroup();
}

QString lsGlobalSettings::mp3Path()
{
	if (self == NULL) { return QStringLiteral(""); }

	return self->ui->lineEditMp3Path->text();
}

void lsGlobalSettings::save()
{
	QSettings settings;
	settings.beginGroup(QStringLiteral("Global"));

	settings.setValue(QStringLiteral("Mp3Path"), ui->lineEditMp3Path->text());
	settings.setValue(QStringLiteral("CapturePath"), ui->lineEditCapturePath->text());
	settings.setValue(QStringLiteral("CopyCenterServicePath"), ui->lineEditCopyCenter->text());
	settings.setValue(QStringLiteral("CopyCenterPathFormat"), ui->lineEditCopyCenterPathFormat->text());

	settings.endGroup();
}


QString lsGlobalSettings::subPath(const QDate& date)
{
	if (self == NULL) { return QStringLiteral(""); }

	QString toReturn = self->ui->lineEditSubPath->text();
	while (toReturn.contains('<'))
	{
		QString pre = toReturn.section('<', 0, 0);
		toReturn = toReturn.section('<', 1, -1);
		QString attr = toReturn.section('>', 0, 0);
		QString key = attr.section(':', 0, 0).toLower();
		if (key == "date")
		{
			attr = date.toString(attr.section(':', 1, -1));
		}
		else
		{
			attr = "";
		}
		toReturn = pre + attr + toReturn.section('>', 1, -1);
	}

	return toReturn;
}

void lsGlobalSettings::on_toolButtonCapturePath_clicked()
{
	QString path = QFileDialog::getExistingDirectory(this, QStringLiteral("Aufnahmepfad"), ui->lineEditCapturePath->text());
	if ( ! path.isEmpty())
	{
		ui->lineEditCapturePath->setText(path);
	}
}

void lsGlobalSettings::on_toolButtonMp3Path_clicked()
{
	QString path = QFileDialog::getExistingDirectory(this, QStringLiteral("MP3 Pfad"), ui->lineEditMp3Path->text());
	if ( ! path.isEmpty())
	{
		ui->lineEditMp3Path->setText(path);
	}
}

void lsGlobalSettings::on_toolButtonCopyCenter_clicked()
{
    QString path = QFileDialog::getExistingDirectory(this, QStringLiteral("CopyCenter"), ui->lineEditCopyCenter->text());
    if ( ! path.isEmpty())
    {
        ui->lineEditCopyCenter->setText(path);
    }
}
