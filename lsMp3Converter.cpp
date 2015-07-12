#include "lsMp3Converter.h"

#include "Einzelbeitrag.h"
#include "lsGlobalSettings.h"

#include <QDir>
#include <QProcess>

lsMp3Converter::lsMp3Converter(lsGlobalSettings* globalSettings, QObject* parent) : QObject(parent), globalSettings(globalSettings)
{
	this->isProcessing = false;
	this->proc = new QProcess(this);
	this->proc->setProcessChannelMode(QProcess::MergedChannels);
	connect(this->proc, &QProcess::readyRead, this, &lsMp3Converter::slotProcOutput);
	connect(this->proc, SIGNAL(finished(int)), this, SLOT(slotFinished()));
}

lsMp3Converter::~lsMp3Converter()
{
}

bool lsMp3Converter::convert(Einzelbeitrag* toConvert, const QDate& date)
{
	if (NULL == toConvert) { return false; }
	if (date.isNull() || ! date.isValid()) { return false; }

	if ( ! this->queue.contains(QPair<QPointer<Einzelbeitrag>,QDate>(toConvert, date)))
	{
		this->queue.enqueue(QPair<QPointer<Einzelbeitrag>,QDate>(toConvert, date));
	}

	if (this->subPath.isEmpty())
	{
		return false;
	}
	if (this->globalSettings.isNull())
	{
		return false;
	}
	if (this->globalSettings->capturePath().isEmpty())
	{
		return false;
	}

	if ( ! this->isProcessing)
	{
		this->slotFinished();
	}

	return true;
}

void lsMp3Converter::slotFinished()
{
	if (this->isProcessing && ! this->currentProcessing.isNull())
	{
		this->currentProcessing->setId3Tag();
	}
	emit signalFinished(this);

	if (this->queue.isEmpty())
	{
		this->isProcessing = false;
	}
	else
	{
		this->isProcessing = true;

		QPair<QPointer<Einzelbeitrag>,QDate> pair = this->queue.dequeue();

		Einzelbeitrag* toConvert = pair.first.data();
		if (NULL == toConvert) { if ( ! this->queue.isEmpty()) { this->slotFinished(); } return; }

		const QDate& date = pair.second;
		if (date.isNull() || ! date.isValid()) { if ( ! this->queue.isEmpty()) { this->slotFinished(); } return; }

		connect(this, &lsMp3Converter::signalProgress, toConvert, &Einzelbeitrag::slotMp3Progress);
		connect(this, &lsMp3Converter::signalFinished, toConvert, &Einzelbeitrag::slotMp3ConvertFinished);

		QString cpath = this->globalSettings->capturePath();
		if (cpath.isEmpty())
		{
			if ( ! this->queue.isEmpty())
			{
				this->slotFinished();
			}
			return;
		}
		if ( ! cpath.endsWith('/'))
		{
			cpath += QStringLiteral("/");
		}
		QString preSubPath = this->globalSettings->subPath(date);

		if (QDir(cpath + preSubPath).exists())
		{
			cpath += preSubPath;
			if ( ! cpath.endsWith('/'))
			{
				cpath += QStringLiteral("/");
			}
		}
		cpath += this->subPath;
		if ( ! cpath.endsWith('/'))
		{
			cpath += QStringLiteral("/");
		}

		QString mp3Path = this->globalSettings->mp3Path();
		QString serverPath = this->globalSettings->serverPath();
		if (mp3Path.isEmpty() && serverPath.isEmpty())
		{
			if ( ! this->queue.isEmpty())
			{
				this->slotFinished();
			}
			return;
		}

		QString tarPath = mp3Path;
		if (tarPath.isEmpty())
		{
			tarPath = serverPath;
		}

		if ( ! tarPath.endsWith('/'))
		{
			tarPath += QStringLiteral("/");
		}

		if (QDir(tarPath + preSubPath).exists())
		{
			tarPath += preSubPath;
			if ( ! tarPath.endsWith('/'))
			{
				tarPath += QStringLiteral("/");
			}
		}

		QString tarBasePath = tarPath;

		tarPath += this->subPath;
		if ( ! tarPath.endsWith('/'))
		{
			tarPath += QStringLiteral("/");
		}

		if ( ! QDir(tarPath).exists())
		{
			QDir(tarBasePath).mkpath(tarPath);
		}

		QString fileName = toConvert->fileName();

		QStringList params;
		params << QStringLiteral("-b") << QStringLiteral("192") << QStringLiteral("--cbr");
		params << QStringLiteral("-h") << QStringLiteral("--tg") << QStringLiteral("12");
		params << "\"" + cpath + fileName.section('.', 0, -2) + QStringLiteral(".wav\"");
		params << "\"" + tarPath + fileName.section('.', 0, -2) + QStringLiteral(".mp3\"");

		this->currentProcessing = toConvert;

		this->proc->start("\"C:/Program Files/lame/lame.exe\" " + params.join(" ").replace("/", "\\"));
	}
}

void lsMp3Converter::slotProcOutput()
{
	QString text = this->proc->readAll();
	QRegExp regex("\\( ?([0-9]+)\\%\\)");
	if (regex.indexIn(text) > -1)
	{
		int percent = regex.capturedTexts().last().toInt();
		if (percent > 0)
		{
			emit signalProgress(percent);
		}
	}
//	qDebug(text.toLatin1().constData());
}

