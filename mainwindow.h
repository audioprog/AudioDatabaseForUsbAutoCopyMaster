#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QMediaContent>
#include <QtSql>

class QMediaPlayer;

namespace Ui {
class MainWindow;
}

class Einzelbeitrag;
class PathScannerThread;
class LsFileList;
class lsKeyAndFilterListEditor;
class lsMp3Converter;

class QCloseEvent;
class QProgressBar;
class QSqlTableModel;
class QTimer;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
    void clearItems();

	QString detFullPath( const QString& basePath );

	QHash<int,QString> fetchTitlesOfPath( const QString& dir = QStringLiteral(""));

	QString findPathOfDate() const;

	void createEinzelbeitragItems();

	Einzelbeitrag* detExistEinzelbeitrag( int titleNr );

	QString detSubPath() const;

	QString getAlbum() const;

	void updateFileWatcher();

	QStringList searchNewWords(const QString& tableName, const QStringList& wordList, int minCount);
	QList<Einzelbeitrag*> detEinzelbeitragList();
        
	QString detCopyCenterSubPath(const QDate& date, const QString& dayTime);
        
    void copyToCopyCenter(const QDate& date, const QString& dayTime, bool forceUpdate = false);

protected:
	void closeEvent( QCloseEvent* event );

private slots:
	void slotAddAtMe( Einzelbeitrag* bevor );

	void slotAddNewNames( QStringList newWordList );

	void slotAddNewWords(QStringList newWordList);

	void slotConvertToMp3( Einzelbeitrag* toConvert );

	void slotDateScannerFinished();

	void slotDirChanged( QString dir );

	void slotNewTitle();

	void slotPlayTitle( QString fileName );

    void slotRemoveMe( Einzelbeitrag* toDelete );

	void slotRename(Einzelbeitrag* toRename);

	void slotSyncMp3(Einzelbeitrag* toSync);

	void slotTableViewSelectChanged(const QModelIndex &current, const QModelIndex &previous);

	void slotCheck();

private slots:
    void on_actionAddLine_triggered();

	void on_dateEdit_dateChanged( const QDate& );

    void on_actionVerwaltung_triggered();

    void on_comboBoxZeit_currentIndexChanged(const QString &arg1);

    void on_actionStop_triggered();

    void on_actionSpeichern_triggered();

	void on_lineEditGottesdiensteFilter_textChanged(const QString &arg1);

	void on_tableViewGottesdienste_doubleClicked(const QModelIndex &index);

	void on_buttonStartFileWatcher_clicked();

	void on_actionNamen_triggered();

	void on_actionWorte_triggered();

	void on_actionPrintpreview_triggered();

	void on_lineEditSearch_textChanged(const QString &arg1);

	void on_toolButtonGotoFromSearch_clicked();

	void on_toolButtonOpenPath_clicked();

	void on_toolButtonReRead_clicked();

    void on_toolButtonPush_clicked();

	void on_actionAlles_kopieren_triggered();

private:

	void addConnections(Einzelbeitrag* einzel);

	void addRestEinzelbeitrag( QSqlDatabase* db, int einzelbeitragIndex, qlonglong gottesdienstId );

	void createMediaPlayer();

	QStringList detFileNameOfTitle( const QString& fullPath, int titleNr );

	QString findPathOfDateAtPathScanner( PathScannerThread* pathScanner ) const;

	qlonglong detMaxBeitragId( QSqlDatabase* db );

	void setTitleNr(Einzelbeitrag* einzel, const QHash<int, QString>& fileHash);

	void setTitleNrsStartsAt(int index);

	QString slotRenameInPath( QString path, Einzelbeitrag* toRename, QString preSubPath, QString subpath );

	bool updateBeschreibungFromDb(const QDate &date, const QString &selecteTime);

private:
    Ui::MainWindow *ui;

	bool isCreatingEinzelbeitragItems;

	QFileSystemWatcher* fileWatcher;

    QMediaPlayer*   mediaPlayer;

	PathScannerThread* serverPathScanner;

	PathScannerThread* mp3PathScanner;

	PathScannerThread* capturePathScanner;

	QProgressBar*	progressBar;

	QSqlTableModel* sqlModel;

	QSqlDatabase	db;

	LsFileList*	fileList;

	lsKeyAndFilterListEditor* nameFilter;

	lsKeyAndFilterListEditor* bibleFilter;

	lsKeyAndFilterListEditor* typeFilter;

	lsMp3Converter* mp3Converter;

	QString searchQuery;

	QString selectedQuery;

	QTimer* timerCheck;
};

#endif // MAINWINDOW_H
