#include "lsMp3Converter.h"

#include "Einzelbeitrag.h"
#include "apGlobalHelper.h"
#include "lsGlobalSettings.h"

#include <QDir>
#include <QProcess>
#include <QRegExp>

lsMp3Converter::lsMp3Converter(lsGlobalSettings* globalSettings, QObject* parent) : QObject(parent), globalSettings(globalSettings)
{
	this->isProcessing = false;
	this->proc = new QProcess(this);
	this->proc->setProcessChannelMode(QProcess::MergedChannels);
	connect(this->proc, &QProcess::readyRead, this, &lsMp3Converter::slotProcOutput);
    connect(this->proc, qOverload<int,QProcess::ExitStatus>(&QProcess::finished), this, &lsMp3Converter::slotFinished);
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
        this->slotFinished(0, QProcess::NormalExit);
	}

	return true;
}

void lsMp3Converter::slotFinished(int code, QProcess::ExitStatus exitStatus)
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
        if (NULL == toConvert) { if ( ! this->queue.isEmpty()) { this->slotFinished(0, QProcess::NormalExit); } return; }

		const QDate& date = pair.second;
        if (date.isNull() || ! date.isValid()) { if ( ! this->queue.isEmpty()) { this->slotFinished(0, QProcess::NormalExit); } return; }

		connect(this, &lsMp3Converter::signalProgress, toConvert, &Einzelbeitrag::slotMp3Progress);
		connect(this, &lsMp3Converter::signalFinished, toConvert, &Einzelbeitrag::slotMp3ConvertFinished);

		QString cpath = this->globalSettings->capturePath();
		if (cpath.isEmpty())
		{
			if ( ! this->queue.isEmpty())
			{
                this->slotFinished(0, QProcess::NormalExit);
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
        if (mp3Path.isEmpty())
		{
			if ( ! this->queue.isEmpty())
            {
                this->slotFinished(0, QProcess::NormalExit);
			}
			return;
		}

        QString tarPath = mp3Path;

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
        if (fileName.isEmpty())
        {
            fileName = toConvert->newFileName() + ".mp3";
            toConvert->setFileName(QFileInfo(tarPath + fileName));
        }

		QStringList params;
		params << QStringLiteral("-b") << QStringLiteral("192") << QStringLiteral("--cbr");
		params << QStringLiteral("-h") << QStringLiteral("--tg") << QStringLiteral("12");
		params << "\"" + cpath + fileName.section('.', 0, -2) + QStringLiteral(".wav\"");
		params << "\"" + tarPath + fileName.section('.', 0, -2) + QStringLiteral(".mp3\"");

		this->currentProcessing = toConvert;

        QString command = "\"C:/Program Files (x86)/lame/lame.exe\" " + params.join(" ").replace("/", "\\");
        this->proc->start(command);
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
    qDebug(text.toLatin1().constData());
}

