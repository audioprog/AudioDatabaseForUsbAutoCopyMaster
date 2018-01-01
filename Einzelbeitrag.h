#ifndef EINZELBEITRAG_H
#define EINZELBEITRAG_H

#include <QWidget>

class LsFileList;
class lsGlobalSettings;
class lsKeyAndFilterListEditor;
class lsMp3Converter;
class MainWindow;

class QLabel;

#include <QDate>
#include <QFileInfo>
#include <QPointer>
#include <QtSql>

namespace Ui {
class Einzelbeitrag;
}

class Einzelbeitrag : public QWidget
{
    Q_OBJECT

	enum eState
	{
		eStateReady,
		eStateName,
		eStateNonExit
	};

public:
	explicit Einzelbeitrag(MainWindow* parentWindow, QWidget *parent = 0);
	explicit Einzelbeitrag( const QSqlRecord& record, const QString& subPath, lsGlobalSettings* settings, LsFileList* fileList, MainWindow* parentWindow, QWidget* parent = NULL);
	Einzelbeitrag( const QString& fileName, int title, lsKeyAndFilterListEditor* names, lsKeyAndFilterListEditor* types, lsKeyAndFilterListEditor* bible, const QString& subPath, lsGlobalSettings* settings, LsFileList* fileList, MainWindow* parentWindow, QWidget* parent );
    ~Einzelbeitrag();

	QString art() const;

	QString bibelstelle() const;

	const QDate& getDate() const { return this->date; }

	QDate detDate() const;

	QString detFullPath( const QString& basePath );

	QString fileName() const;

	void fillWithRecord(const QSqlRecord &record);

	QString findBestMatch( const QString& fileName, const QStringList& list, QLineEdit* lineEdit );

	QString getBeitragsart() const;

	const QString& getSubPath() const { return this->subPath; }

	QString name() const;

	QString newFileName() const;

	void reReadNames( bool isReadFromServer );

	void reReadWords( bool isReadFromServer );

	void setAutoCompleteDatabase( QSqlDatabase db );

    void setFileName( const QFileInfo& file );

    void setTitleNr( int newNumber );

	QString text() const;

    int titelNr() const;

	void reRead();

	void setId3Tag();

	void setFileList( LsFileList* newFileList ) { this->fileList = newFileList; }

	bool insertId3Tag( const QString& path, const QHash<int, QString>& names );

	static void correctForbiddenChars( QString& toCorrect );

	static QString correctForbiddenChars(const QString& rawFileName);

public slots:
	void slotMp3Progress( int percent );

	void slotMp3ConvertFinished( lsMp3Converter* converter );

signals:
	void signalConvertToMp3( Einzelbeitrag* );

    void signalRemoveThis( Einzelbeitrag* );

	void signalRename( Einzelbeitrag* );

    void signalAddNextTo( Einzelbeitrag* );

	void signalPlay( QString fileName );

	void signalSync( Einzelbeitrag* );

private slots:

	void slotUpdateStates();

    void on_toolButtonRemoveThis_clicked();

    void on_toolButtonAnnNext_clicked();

	void on_toolButtonStartPlay_clicked();

	void on_toolButtonMp3_clicked();

	void on_toolButtonRename_clicked();

	void on_toolButtonReread_clicked();

	void on_buttonSync_clicked();

	void on_toolButtonSetId3Tag_clicked();

private:

	QString intUpdateState(eState& curState, QLabel* label, QHash<int,QString> files, int titleNr, QString rightFileName , const QString& path);

	void intUpdateState(QLabel* label, eState& curState, const QString& path, eState nonExistState, const QString& extension);

	void log( const QString& logText );

private:
	Ui::Einzelbeitrag *ui;

	QPointer<lsKeyAndFilterListEditor> names;
	QPointer<lsKeyAndFilterListEditor> types;
	QPointer<lsKeyAndFilterListEditor> bible;

	eState stateCapture;
	eState stateLocal;
	eState stateServer;

	QDate date;

	QString subPath;

	lsGlobalSettings* globalSettings;

	LsFileList* fileList;

	MainWindow* parentWindow;

	QTimer* timerUpdate;
};

#endif // EINZELBEITRAG_H
