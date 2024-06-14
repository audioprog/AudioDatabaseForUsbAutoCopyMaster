#ifndef LSMP3CONVERTER_H
#define LSMP3CONVERTER_H

#include "qprocess.h"
#include <QObject>

#include <QPointer>
#include <QQueue>

class Einzelbeitrag;
class lsGlobalSettings;

class QProcess;

class lsMp3Converter : public QObject
{
    Q_OBJECT

public:
	explicit lsMp3Converter(lsGlobalSettings* globalSettings, QObject *parent = 0);
    ~lsMp3Converter();

signals:
	void signalFinished( lsMp3Converter* );

	void signalProgress( int percent );

public:
	bool convert( Einzelbeitrag* toConvert, const QDate& date );

	void setSubPath( const QString& newSubPath ) { this->subPath = newSubPath; }

private slots:
    void slotFinished(int code, QProcess::ExitStatus exitStatus);

	void slotProcOutput();

private:

	QPointer<lsGlobalSettings> globalSettings;

	QQueue<QPair<QPointer<Einzelbeitrag>,QDate> > queue;

	QProcess* proc;

	QPointer<Einzelbeitrag> currentProcessing;

	bool isProcessing;

	QString subPath;
};

#endif // LSMP3CONVERTER_H
