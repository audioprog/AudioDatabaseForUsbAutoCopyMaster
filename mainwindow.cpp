#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "apFileUtils.h"
#include "apGlobalHelper.h"
#include "Einzelbeitrag.h"
#include "PathScannerThread.h"
#include "LsFileList.h"
#include "lsGlobalSettings.h"
#include "lsKeyAndFilterListEditor.h"
#include "lsMp3Converter.h"
#include "ListSelectDialog.h"
#include "ListPrintDialog.h"

#include <QBoxLayout>
#include <QDesktopServices>
#include <QFileDialog>
#include <QFileSystemWatcher>
#include <QMediaPlayer>
#include <QMessageBox>
#include <QProgressBar>
#include <QRegExp>
#include <QSettings>
#include <QTimer>
#include <QUrl>

#include <QtSql>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
  , searchQuery("SELECT Datum,Zeit,Beschreibung,Name,Art,Text,Bibelstelle "
				"FROM `audio`.`Beitrag` "
				"INNER JOIN `audio`.`GottesdienstId` "
				"ON `audio`.`Beitrag`.`GottesdienstId` = `audio`.`GottesdienstId`.`Id` "
				"WHERE `FileName` LIKE '%%1%' OR `Name` LIKE '%%1%' OR `Art` LIKE '%%1%' OR `Text` LIKE '%%1%' OR `Bibelstelle` LIKE '%%1%' OR `Beschreibung` LIKE '%%1%'")
  , selectedQuery("SELECT TitelNr,Name,Art,Text,Bibelstelle FROM `audio`.`Beitrag` INNER JOIN `audio`.`GottesdienstId` ON `audio`.`Beitrag`.`GottesdienstId` = `audio`.`GottesdienstId`.`Id` WHERE Datum='%1' AND Zeit='%2'")
  , timerCheck(new QTimer(this))
{
	this->fileWatcher = NULL;
	this->fileList = NULL;
	this->isCreatingEinzelbeitragItems = false;
	this->mediaPlayer = NULL;
	this->sqlModel = NULL;
	this->mp3Converter = NULL;
    ui->setupUi(this);

	this->progressBar = new QProgressBar(this);
    this->ui->statusBar->addWidget(this->progressBar);

	this->mp3PathScanner = new PathScannerThread(this);
	connect(this->mp3PathScanner, &PathScannerThread::finished, this, &MainWindow::slotDateScannerFinished);

	this->capturePathScanner = new PathScannerThread(this);
	connect(this->capturePathScanner, &PathScannerThread::finished, this, &MainWindow::slotDateScannerFinished);

    this->db = QSqlDatabase::addDatabase("QMYSQL", "audio");
    this->db.setHostName("192.168.178.11");
    db.setDatabaseName("audio");
    db.setUserName("audio");
    db.setPassword("1udio");
	if ( ! db.open())
	{
		qDebug(db.lastError().text().toLatin1());
	}

	ui->dateEdit->blockSignals(true);
    ui->dateEdit->setDate(QDate::currentDate());
	ui->dateEdit->blockSignals(false);

    if (this->ui->scrollArea->widget() == NULL)
    {
        this->ui->scrollArea->setWidget(new QWidget(this));
    }
    if (this->ui->scrollArea->widget()->layout() == NULL)
    {
        new QVBoxLayout(this->ui->scrollArea->widget());
    }

	this->nameFilter = new lsKeyAndFilterListEditor(this);
	ui->tabWidget->addTab(this->nameFilter, QIcon(), QStringLiteral("NamenFilter"));
	this->nameFilter->openTable(db, QStringLiteral("KeywordsName"));

	this->typeFilter = new lsKeyAndFilterListEditor(this);
	ui->tabWidget->addTab(this->typeFilter, QIcon(), QStringLiteral("ArtFilter"));
	this->typeFilter->openTable(db, QStringLiteral("FilterType"));

	this->bibleFilter = new lsKeyAndFilterListEditor(this);
	ui->tabWidget->addTab(this->bibleFilter, QIcon(), QStringLiteral("BibelstellenFilter"));
	this->bibleFilter->openTable(db, QStringLiteral("FilterBibletext"));

	ui->tabWidget->setCurrentIndex(0);

	this->sqlModel = new QSqlTableModel(this, db);
	sqlModel->setTable("GottesdienstId");
	sqlModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
	sqlModel->setSort(1, Qt::AscendingOrder);
	sqlModel->select();
	ui->tableViewGottesdienste->setModel(sqlModel);
	ui->tableViewGottesdienste->hideColumn(0);
	ui->tableViewGottesdienste->horizontalHeader()->setStretchLastSection(true);

	this->createMediaPlayer();

	this->ui->globalSettings->load();

	QSettings settings;
	restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());

	this->mp3PathScanner->baseDir = lsGlobalSettings::mp3Path();
	if ( ! this->mp3PathScanner->baseDir.isEmpty() && QDir(this->mp3PathScanner->baseDir).exists())
	{
		this->mp3PathScanner->start();
	}

	this->capturePathScanner->baseDir = lsGlobalSettings::capturePath();
	if ( ! this->capturePathScanner->baseDir.isEmpty() && QDir(this->capturePathScanner->baseDir).exists())
	{
		this->capturePathScanner->start();
	}

	this->fileWatcher = new QFileSystemWatcher(this);
	connect(this->fileWatcher, &QFileSystemWatcher::directoryChanged, this, &MainWindow::slotDirChanged);

	this->mp3Converter = new lsMp3Converter(this->ui->globalSettings, this);

	this->fileList = new LsFileList(this);
	this->fileList->setGlobalSettings(ui->globalSettings);

	this->updateFileWatcher();

	QSqlQueryModel* model = new QSqlQueryModel(ui->tableViewSelect);
	ui->tableViewSelect->setModel(model);
	connect(ui->tableViewSelect->selectionModel(), &QItemSelectionModel::currentChanged, this, &MainWindow::slotTableViewSelectChanged);

	QSqlQueryModel* selModel = new QSqlQueryModel(ui->tableViewCD);
	ui->tableViewCD->setModel(selModel);

	connect(this->timerCheck, &QTimer::timeout, this, &MainWindow::slotCheck);

	this->timerCheck->start(20000);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::addConnections(Einzelbeitrag* einzel)
{
	einzel->setFileList(this->fileList);

	disconnect(einzel, &Einzelbeitrag::signalConvertToMp3, this, &MainWindow::slotConvertToMp3);
	disconnect(einzel, &Einzelbeitrag::signalRemoveThis, this,&MainWindow::slotRemoveMe);
	disconnect(einzel, &Einzelbeitrag::signalRename, this,&MainWindow::slotRename);
	disconnect(einzel, &Einzelbeitrag::signalAddNextTo, this, &MainWindow::slotAddAtMe);
    disconnect(einzel, &Einzelbeitrag::signalPlay, this, &MainWindow::slotPlayTitle);

	connect(einzel, &Einzelbeitrag::signalConvertToMp3, this, &MainWindow::slotConvertToMp3);
	connect(einzel, &Einzelbeitrag::signalRemoveThis, this,&MainWindow::slotRemoveMe);
	connect(einzel, &Einzelbeitrag::signalRename, this,&MainWindow::slotRename);
	connect(einzel, &Einzelbeitrag::signalAddNextTo, this, &MainWindow::slotAddAtMe);
    connect(einzel, &Einzelbeitrag::signalPlay, this, &MainWindow::slotPlayTitle);

	einzel->setAutoCompleteDatabase(this->db);
}

void MainWindow::addRestEinzelbeitrag(QSqlDatabase* db, int einzelbeitragStartIndex, qlonglong gottesdienstId)
{
	qlonglong nextBeitragId = this->detMaxBeitragId(db);

	for (int i = einzelbeitragStartIndex; i < this->ui->scrollArea->widget()->layout()->count(); i++)
	{
		Einzelbeitrag* einzel = qobject_cast<Einzelbeitrag*>(this->ui->scrollArea->widget()->layout()->itemAt(i)->widget());
		if (einzel != NULL)
		{
			nextBeitragId++;
			QString insert = QStringLiteral("INSERT INTO Beitrag(Id, GottesdienstId, FileName, TitelNr, Name, Art, Text, Bibelstelle) VALUES (%1,%2,'%3',%4,'%5','%6','%7','%8')")
					.arg(nextBeitragId).arg(gottesdienstId).arg(einzel->fileName()).arg(einzel->titelNr()).arg(einzel->name()).arg(einzel->art()).arg(einzel->text()).arg(einzel->bibelstelle());
			QSqlQuery beitragInsertQuery(*db);
			if (beitragInsertQuery.exec(insert) == false)
			{
				ui->statusBar->showMessage(QStringLiteral("Fehler bei Beitrag %1").arg(nextBeitragId));
			}
		}
	}
}

void MainWindow::clearItems()
{
	ui->labelPath->clear();
	ui->lineEditBeschreibung->clear();

	for (int i = 0; i < this->ui->scrollArea->widget()->layout()->count(); i++)
	{
		Einzelbeitrag* einzel = qobject_cast<Einzelbeitrag*>(this->ui->scrollArea->widget()->layout()->itemAt(i)->widget());
		if (einzel != NULL)
		{
			einzel->deleteLater();
		}
	}
}

void MainWindow::closeEvent(QCloseEvent* event)
{
	ui->globalSettings->save();
	QSettings settings;
	settings.setValue("geometry", saveGeometry());
	settings.setValue("windowState", saveState());
	QMainWindow::closeEvent(event);
}

void MainWindow::createEinzelbeitragItems()
{
	if (this->isCreatingEinzelbeitragItems) { return; }
	disconnect (this->capturePathScanner, &QThread::finished, this, &MainWindow::on_buttonStartFileWatcher_clicked);

	this->isCreatingEinzelbeitragItems = true;

	QString path = this->findPathOfDate();

	if (path.isEmpty())
	{
		if (path.isEmpty())
		{
			QTimer::singleShot(1000, this->capturePathScanner, SLOT(start()));
			connect (this->capturePathScanner, &QThread::finished, this, &MainWindow::on_buttonStartFileWatcher_clicked);
		}
		this->clearItems();
	}
	else
	{
		ui->labelPath->setText(path);
	}

	QString subPath = this->detSubPath();

	QList<Einzelbeitrag*> toDelete;

	for (int i = 0; i < this->ui->scrollArea->widget()->layout()->count(); i++)
	{
		QWidget* widget = this->ui->scrollArea->widget()->layout()->itemAt(i)->widget();
		Einzelbeitrag* einzel = qobject_cast<Einzelbeitrag*>(widget);
		if (einzel != NULL && einzel->getSubPath() != subPath)
		{
			toDelete << einzel;
		}
	}

	foreach (Einzelbeitrag* einzel, toDelete)
	{
		delete einzel;
	}
	toDelete.clear();

	QString selecteTime = ui->comboBoxZeit->currentText();

	this->updateBeschreibungFromDb(ui->dateEdit->date(), selecteTime);

	{
		if ( ! path.isEmpty())
		{
			ui->labelPath->setText(path);

			QHash<int,QString> titles = this->fetchTitlesOfPath();

			QList<int> titleList = titles.keys();
            std::sort(titleList.begin(), titleList.end());

			foreach (int title, titleList)
			{
				Einzelbeitrag* einzel = this->detExistEinzelbeitrag(title);
				if (NULL == einzel)
				{
					einzel = new Einzelbeitrag(titles.value(title), title, this->nameFilter, this->typeFilter, this->bibleFilter, this->detSubPath(), ui->globalSettings, this->fileList, this, this);
					this->ui->scrollArea->widget()->layout()->addWidget(einzel);
					this->ui->scrollArea->widget()->resize(this->ui->scrollArea->maximumWidth(), einzel->sizeHint().height() * this->ui->scrollArea->widget()->layout()->count());
					this->addConnections(einzel);
				}
			}
		}
	}

	this->isCreatingEinzelbeitragItems = false;
}

void MainWindow::createMediaPlayer()
{
	if (this->mediaPlayer)
	{
		delete this->mediaPlayer;
	}
	this->mediaPlayer = new QMediaPlayer(this);
    //this->mediaPlayer->setVolume(100);
//	connect(this->mediaPlayer, &QMediaPlayer::mediaChanged, this, &MainWindow::slotNewTitle);
	connect(this->mediaPlayer, &QMediaPlayer::durationChanged, this->progressBar, &QProgressBar::setMaximum);
	connect(this->mediaPlayer, &QMediaPlayer::positionChanged, this->progressBar, &QProgressBar::setValue);
}

QString MainWindow::detFullPath(const QString& basePath)
{
	if (basePath.isEmpty())
	{
		return basePath;
	}

	QString preSubPath = this->ui->globalSettings->subPath(ui->dateEdit->date());
	QString path = basePath;
	if ( ! path.endsWith('/'))
	{
		path += "/";
	}
	if (QDir(path + preSubPath).exists())
	{
		path += preSubPath;
		if ( ! path.endsWith('/'))
		{
			path += "/";
		}
		path += this->detSubPath();
	}
	else
	{
		path += this->detSubPath();
	}

	return path;
}

qlonglong MainWindow::detMaxBeitragId(QSqlDatabase* db)
{
	qlonglong nextBeitragId = 0;
	{
		QSqlQuery maxIdQuery(*db);
		if (maxIdQuery.exec("SELECT MAX( Id ) FROM Beitrag"))
		{
			if (maxIdQuery.next())
			{
				nextBeitragId = maxIdQuery.record().value(0).toLongLong();
			}
		}
	}

	return nextBeitragId;
}

QString MainWindow::detSubPath() const
{
	QString path = ui->labelPath->text();
    if (path.startsWith(lsGlobalSettings::mp3Path()))
	{
		path = path.mid(lsGlobalSettings::mp3Path().count());
		if (path.startsWith('/'))
		{
			path = path.mid(1);
		}
	}
	else if (path.startsWith(lsGlobalSettings::capturePath()))
	{
		path = path.mid(lsGlobalSettings::capturePath().count());
		if (path.startsWith('/'))
		{
			path = path.mid(1);
		}
	}

	QString preSubPath = lsGlobalSettings::subPath(ui->dateEdit->date());
	if ( preSubPath.startsWith('/'))
	{
		preSubPath = preSubPath.mid(1);
	}

	if (path.startsWith(preSubPath))
	{
		path = path.mid(preSubPath.count());

		if ( path.startsWith('/'))
		{
			path = path.mid(1);
		}
	}

	return path;
}

QString MainWindow::findPathOfDate() const
{
	QString path;

	if (path.isEmpty() && this->mp3PathScanner->hashDateDirInfo.contains(ui->dateEdit->date()))
	{
		path = this->findPathOfDateAtPathScanner(this->mp3PathScanner);
	}
	if (path.isEmpty() && this->capturePathScanner->hashDateDirInfo.contains(ui->dateEdit->date()))
	{
		path = this->findPathOfDateAtPathScanner(this->capturePathScanner);
	}

	return path;
}

QString MainWindow::findPathOfDateAtPathScanner(PathScannerThread* pathScanner) const
{
	QString path;
    QList<PathScannerThread::SDirInfo> list = pathScanner->hashDateDirInfo.values(ui->dateEdit->date());
	QString test = ui->comboBoxZeit->currentText().toLower();
	QString startTest = test.left(1);

	foreach (const PathScannerThread::SDirInfo& item, list)
	{
		if (startTest == "t" && item.dayTime.startsWith("t", Qt::CaseInsensitive))
		{
			if (item.dayTime.endsWith(test.right(1)))
			{
				path = item.path;
				break;
			}
		}
		else if (item.dayTime.left(1).toLower() == startTest)
		{
			path = item.path;
			break;
		}
	}

	return path;
}

QString MainWindow::getAlbum() const
{
	return this->ui->lineEditBeschreibung->text();
}

void MainWindow::log(const QString& modul, const QString& logText)
{
	ui->plainTextEditDebug->textCursor().movePosition(QTextCursor::End, QTextCursor::MoveAnchor);

	ui->plainTextEditDebug->insertPlainText(modul % "\n\t" % logText % "\n");
}

void MainWindow::setTitleNrsStartsAt(int index)
{
	for (int i = index; i < this->ui->scrollArea->widget()->layout()->count(); i++)
	{
		Einzelbeitrag* einzel = qobject_cast<Einzelbeitrag*>(this->ui->scrollArea->widget()->layout()->itemAt(i)->widget());
		if (einzel != NULL)
		{
			einzel->setTitleNr(i + 1);
		}
	}
}

void MainWindow::slotAddAtMe(Einzelbeitrag* bevor)
{
	int index = this->ui->scrollArea->widget()->layout()->indexOf(bevor);

	Einzelbeitrag* einzel = new Einzelbeitrag(this);

	((QBoxLayout*)this->ui->scrollArea->widget()->layout())->insertWidget(index, einzel);
	this->ui->scrollArea->widget()->resize(this->ui->scrollArea->maximumWidth(), einzel->sizeHint().height() * this->ui->scrollArea->widget()->layout()->count());

	einzel->setTitleNr(index + 1);

	this->addConnections(einzel);

	this->setTitleNrsStartsAt(index + 1);
}

void MainWindow::slotConvertToMp3(Einzelbeitrag* toConvert)
{
	slotRename(toConvert);
	on_actionSpeichern_triggered();
	if ( ! this->mp3Converter->convert(toConvert, ui->dateEdit->date()))
	{
		this->mp3Converter->setSubPath(this->detSubPath());
		this->mp3Converter->convert(toConvert, ui->dateEdit->date());
	}
}

void MainWindow::slotDateScannerFinished()
{
    if (this->mp3PathScanner == NULL) { return; }

    if (this->mp3PathScanner->hashDateDirInfo.contains(ui->dateEdit->date()))
	{
        QList<PathScannerThread::SDirInfo> list = this->mp3PathScanner->hashDateDirInfo.values(ui->dateEdit->date());
		QString test = ui->comboBoxZeit->currentText().toLower();
		QString startTest = test.left(1);

		foreach (const PathScannerThread::SDirInfo& item, list)
		{
			if (startTest == "t" && item.dayTime.startsWith("t", Qt::CaseInsensitive))
			{
				if (item.dayTime.endsWith(test.right(1)))
				{
					ui->labelPath->setText(item.path);
					break;
				}
			}
			else if (item.dayTime.left(1).toLower() == startTest)
			{
				ui->labelPath->setText(item.path);
				break;
			}
		}
	}

	if ( ! this->isCreatingEinzelbeitragItems)
	{
		this->createEinzelbeitragItems();
	}
}

Einzelbeitrag* MainWindow::detExistEinzelbeitrag(int titleNr)
{
	Einzelbeitrag* exist = NULL;

	for (int i = 0; i < this->ui->scrollArea->widget()->layout()->count(); i++)
	{
		QWidget* widget = this->ui->scrollArea->widget()->layout()->itemAt(i)->widget();
		Einzelbeitrag* einzel = qobject_cast<Einzelbeitrag*>(widget);
		if (einzel != NULL && einzel->titelNr() == titleNr)
		{
			exist = einzel;
		}
	}

	return exist;
}

QStringList MainWindow::detFileNameOfTitle(const QString& fullPath, int titleNr)
{
	if (QDir(fullPath).exists())
	{
		QStringList filenames = QDir(fullPath).entryList(QStringList("*.wav") << "*.mp3", QDir::Files);
		if ( ! filenames.isEmpty())
		{
			QString regStr = QStringLiteral("^0*%1[^0-9].* ").arg(titleNr);
            QStringList test = filenames.filter(QRegularExpression(regStr));
            test << filenames.filter(QRegularExpression(QStringLiteral("^0*%1 .+").arg(titleNr)));
            test << filenames.filter(QRegularExpression(QStringLiteral("^Tit[le][el] *0*%1[^0-9]").arg(titleNr)));

			test.removeDuplicates();

			return test;
		}
	}

	return QStringList();
}

void MainWindow::slotDirChanged(QString dir)
{
	QHash<int,QString> titles = this->fetchTitlesOfPath(dir);

	QList<int> titleList = titles.keys();
    std::sort(titleList.begin(), titleList.end());

	foreach (int title, titleList)
	{
		Einzelbeitrag* exist = this->detExistEinzelbeitrag(title);
		if (exist == NULL)
		{
			Einzelbeitrag* einzel = new Einzelbeitrag(titles.value(title), title, this->nameFilter, this->typeFilter, this->bibleFilter, this->detSubPath(), ui->globalSettings, this->fileList, this, this);
			this->ui->scrollArea->widget()->layout()->addWidget(einzel);
			this->ui->scrollArea->widget()->resize(this->ui->scrollArea->maximumWidth(), einzel->sizeHint().height() * this->ui->scrollArea->widget()->layout()->count());
			this->addConnections(einzel);
		}
	}
}

void MainWindow::slotNewTitle()
{
	qint64 max = this->mediaPlayer->duration();
	this->progressBar->setMaximum(max);
	this->progressBar->setValue(0);
}

void MainWindow::slotPlayTitle(QString fileName)
{
	QString path = this->ui->labelPath->text();
	if (path.isEmpty()) { return; }

	if (path.endsWith('/') == false)
	{
		path += "/";
	}

	path += fileName;

	this->createMediaPlayer();
    this->mediaPlayer->setSource(QUrl::fromLocalFile(path));
	this->mediaPlayer->play();
}

void MainWindow::slotRemoveMe(Einzelbeitrag* toDelete)
{
    int index = this->ui->scrollArea->widget()->layout()->indexOf(toDelete);
    this->ui->scrollArea->widget()->layout()->removeWidget(toDelete);
	this->setTitleNrsStartsAt(index);
    toDelete->deleteLater();
}

void MainWindow::slotRename(Einzelbeitrag* toRename)
{
	QString subpath = this->detSubPath();
	if (subpath.isEmpty()) { return; }

	this->on_actionStop_triggered();

	if (subpath.endsWith('/') == false)
	{
		subpath += "/";
	}

	QString preSubPath = ui->globalSettings->subPath(ui->dateEdit->date());
	if (preSubPath.isEmpty() == false && ! preSubPath.endsWith('/'))
	{
		preSubPath += "/";
    }

	QString newMp3FileName = this->slotRenameInPath(this->ui->globalSettings->mp3Path(), toRename, preSubPath, subpath);

	QString newCapFileName = this->slotRenameInPath(this->ui->globalSettings->capturePath(), toRename, preSubPath, subpath);

    if ( ! newMp3FileName.isEmpty())
    {
        QFileInfo newName(newMp3FileName);
        toRename->setFileName(newName);
	}
	else if ( ! newCapFileName.isEmpty())
    {
        QFileInfo newName(newCapFileName);
        toRename->setFileName(newName);
	}
}

QString MainWindow::slotRenameInPath(QString path, Einzelbeitrag* toRename, QString preSubPath, QString subpath)
{
	if ( path.isEmpty())
	{
		return "";
	}

	if (path.endsWith('/') == false)
	{
		path += "/";
	}

	if (QDir(path + preSubPath).exists())
	{
		path += preSubPath;
	}

	if (QDir(path + subpath).exists())
	{
		path += subpath;
	}

	QString returnFileName;

	QStringList filenames = QDir(path).entryList(QStringList("*.wav") << "*.mp3", QDir::Files);
	if ( ! filenames.isEmpty())
	{
		QString regStr = QStringLiteral("^0*%1[^0-9].* ").arg(toRename->titelNr());
        QStringList test = filenames.filter(QRegularExpression(regStr));
        test << filenames.filter(QRegularExpression(QStringLiteral("^0*%1 .+").arg(toRename->titelNr())));
        test << filenames.filter(QRegularExpression(QStringLiteral("^Tit[le][el] *0*%1[^0-9]").arg(toRename->titelNr())));

        if (test.count() == 0)
        {
            test << filenames.filter(QRegularExpression(QStringLiteral("^0*%1\\.[WwMm][AaPp][Vv3]$").arg(toRename->titelNr())));
        }

		test.removeDuplicates();

        QStringList oldWave = test.filter(QRegularExpression(".[Ww][Aa][Vv]$"));
        QStringList oldMp3 = test.filter(QRegularExpression(".[Mm][Pp]3"));

		QString newName;

        if (oldWave.count() > 1)
        {
            test.clear();
            test << oldWave.filter(QRegularExpression(QStringLiteral("^0*%1 .+").arg(toRename->titelNr())));
            if (test.count() == 1)
            {
                oldWave = test;
            }
        }
		if (oldWave.count() == 1)
		{
			newName = toRename->newFileName();
			returnFileName = path + newName + ".wav";
			if (path + oldWave.first() != returnFileName && ! QFile(path + oldWave.first()).rename(returnFileName))
			{
				returnFileName = "";
			}
		}
		if (oldMp3.count() == 1)
		{
			if (newName.isEmpty())
			{
				newName = toRename->newFileName();
			}
			returnFileName = path + newName + ".mp3";
			QString oldFile = path + oldMp3.first();
			if ( ! QFile(oldFile).rename(returnFileName))
			{
				returnFileName = "";
			}
		}
	}

	return returnFileName;
}

void MainWindow::slotTableViewSelectChanged(const QModelIndex& index, const QModelIndex&)
{
	QSqlQueryModel* model = qobject_cast<QSqlQueryModel*>(ui->tableViewSelect->model());
	if (model)
	{
		QDate date = model->data(model->index(index.row(), 0)).toDate();
		if (date.isValid())
		{
			QString time = model->data(model->index(index.row(), 1)).toString();

			QSqlQueryModel* selModel = qobject_cast<QSqlQueryModel*>(ui->tableViewCD->model());
			if (selModel)
			{
				selModel->setQuery(this->selectedQuery.arg(date.toString(Qt::ISODate)).arg(time), this->db);
			}
		}
	}
}

void MainWindow::slotCheck()
{
	QList<Einzelbeitrag*> list = this->detEinzelbeitragList();

	for (int i = 0; i < list.count(); i++)
	{
		if (list.at(i)->titelNr() != i + 1)
		{
			ui->toolButtonReRead->setStyleSheet("background: red;");
			return;
		}
	}

	ui->toolButtonReRead->setStyleSheet("");
}

void MainWindow::on_actionAddLine_triggered()
{
    Einzelbeitrag* einzel = new Einzelbeitrag(this);
    this->ui->scrollArea->widget()->layout()->addWidget(einzel);
    this->ui->scrollArea->widget()->resize(this->ui->scrollArea->maximumWidth(), einzel->sizeHint().height() * this->ui->scrollArea->widget()->layout()->count());
    einzel->setTitleNr(this->ui->scrollArea->widget()->layout()->count());
	this->addConnections(einzel);
}

void MainWindow::on_dateEdit_dateChanged(const QDate&)
{
	this->createEinzelbeitragItems();

	this->updateFileWatcher();
}

QHash<int, QString> MainWindow::fetchTitlesOfPath(const QString& dir)
{
	QHash<int,QString> fileHash;
	QString path = ui->labelPath->text();
	if ( ! dir.isEmpty())
	{
		path = dir;

		if (path.isEmpty() == false)
		{
			QStringList fileList = QDir(path).entryList(QStringList(QStringLiteral("*.mp3")) << QStringLiteral("*.wav"), QDir::Files);

            QRegExp regex(".*(\\d+).*");

			foreach (const QString& file, fileList)
			{
                int nrPos = regex.indexIn(file);
                if (nrPos > -1 && (file.startsWith(QStringLiteral("Tit"), Qt::CaseInsensitive) || nrPos == 0))
				{
					fileHash[regex.cap(1).toInt()] = file;
				}
			}
		}
	}
	else
	{
        QString basePath = ui->globalSettings->mp3Path();
		if ( ! basePath.isEmpty())
		{
			path = this->detFullPath(basePath);
			if (QDir(path).exists())
			{
				QHash<int,QString> addFileHash = this->fetchTitlesOfPath(path);
				foreach (int titleNr, addFileHash.keys())
				{
					if ( ! fileHash.contains(titleNr))
					{
						fileHash[titleNr] = addFileHash.value(titleNr);
					}
				}
			}
		}

		basePath = ui->globalSettings->capturePath();
		if ( ! basePath.isEmpty())
		{
			path = this->detFullPath(basePath);
			if (QDir(path).exists())
			{
				QHash<int,QString> addFileHash = this->fetchTitlesOfPath(path);
				foreach (int titleNr, addFileHash.keys())
				{
					if ( ! fileHash.contains(titleNr))
					{
						fileHash[titleNr] = addFileHash.value(titleNr);
					}
				}
			}
		}
	}

	return fileHash;
}

void MainWindow::setTitleNr(Einzelbeitrag* einzel, const QHash<int,QString>& fileHash)
{
	int titleNr = einzel->titelNr();
	if (fileHash.contains(titleNr))
	{
		QString path = ui->labelPath->text();
		if (path.endsWith('/') == false)
		{
			path += "/";
        }
        QFileInfo newName(path + fileHash.value(titleNr));
        einzel->setFileName(newName);
	}
}

bool MainWindow::updateBeschreibungFromDb(const QDate &date, const QString& selecteTime)
{
    QSqlDatabase db = QSqlDatabase::database(dbName);
    QSqlQuery query(db);

    qlonglong id = -1;

	ui->labelPath->clear();

    if (query.exec("SELECT * FROM GottesdienstId WHERE Datum='" + date.toString(Qt::ISODate) + "' AND Zeit='" + selecteTime + "'"))
    {
        while (query.next())
        {
            id = query.record().value("Id").toInt();
//            QString zeit = query.record().value("Zeit").toString();
			QString path = query.record().value("Path").toString();
            QString beschreibung = query.record().value("Beschreibung").toString();
			if (path.isEmpty() == false)
			{
                QString startpath = lsGlobalSettings::mp3Path();
				if (startpath.endsWith('/') == false)
				{
					startpath += "/";
				}
				ui->labelPath->setText(startpath + path);
			}
            ui->lineEditBeschreibung->setText(beschreibung);
        }
    }

    if (id > -1)
    {
		if (ui->labelPath->text().isEmpty())
		{
			this->slotDateScannerFinished();
		}

		QSqlQuery itemQuery(db);
		int nr = 1;
		if (itemQuery.exec("SELECT * FROM Beitrag WHERE GottesdienstId=" + QString::number(id) + " ORDER BY TitelNr,Id"))
        {
			QHash<int,QString> fileHash = this->fetchTitlesOfPath();

            while (itemQuery.next())
            {
				Einzelbeitrag* einzel = this->detExistEinzelbeitrag(itemQuery.record().value("TitelNr").toInt());
				if (einzel != NULL)
				{
					einzel->fillWithRecord(itemQuery.record());
				}
				else
				{
					einzel = new Einzelbeitrag(itemQuery.record(), this->detSubPath(), ui->globalSettings, this->fileList, this);
					this->ui->scrollArea->widget()->layout()->addWidget(einzel);
				}
                this->ui->scrollArea->widget()->resize(this->ui->scrollArea->maximumWidth(), einzel->sizeHint().height() * this->ui->scrollArea->widget()->layout()->count());
				if (einzel->titelNr() == 0)
				{
					einzel->setTitleNr(nr);
				}
				else
				{
					nr = einzel->titelNr();
				}
				this->setTitleNr(einzel, fileHash);
				this->addConnections(einzel);
				nr++;
			}
			return true;
		}
    }
	return false;
}

void MainWindow::on_actionVerwaltung_triggered()
{
	QDesktopServices::openUrl(QUrl("http://nas-bbh:7082/"));
}

void MainWindow::on_comboBoxZeit_currentIndexChanged(const QString&)
{
	this->createEinzelbeitragItems();

	this->updateFileWatcher();
}

void MainWindow::on_actionStop_triggered()
{
	if (this->mediaPlayer != NULL)
	{
		this->mediaPlayer->stop();
        this->mediaPlayer->setSource(QUrl());
		delete this->mediaPlayer;
		this->mediaPlayer = NULL;
	}
}

void MainWindow::on_actionSpeichern_triggered()
{
    QSqlDatabase db = QSqlDatabase::database(dbName);

	QSqlQuery gottesdiensteQuery(db);

	qlonglong gottesdienstId = -1;

	if (gottesdiensteQuery.exec("SELECT Id FROM GottesdienstId WHERE Datum='" + ui->dateEdit->date().toString(Qt::ISODate) + "' AND Zeit='" + ui->comboBoxZeit->currentText() + "'"))
	{
		while (gottesdiensteQuery.next())
		{
			gottesdienstId = gottesdiensteQuery.record().value("Id").toInt();
		}
	}

	QString path = this->detSubPath();
	QString prePath = ui->globalSettings->subPath(ui->dateEdit->date());
	if ( ! prePath.endsWith('/'))
	{
		prePath += "/";
	}
	path = prePath + path;

	if (gottesdienstId > -1)
	{
		QString update = QStringLiteral("UPDATE GottesdienstId SET Beschreibung='%2', Path='%3' WHERE Id=%1").arg(gottesdienstId).arg(ui->lineEditBeschreibung->text().replace('\'', '\"')).arg(path);
		QSqlQuery updateQuery(db);
		updateQuery.exec(update);

		QString qTest = QStringLiteral("SELECT Id FROM Beitrag WHERE GottesdienstId=%1").arg(gottesdienstId);

		int einzelbeitragIndex = 0;

		QList<qlonglong> toDeleteRowIds;

		QSqlQuery testQuery(db);
		if (testQuery.exec(qTest))
		{
			while (testQuery.next())
			{
				Einzelbeitrag* einzel = NULL;
				if (einzelbeitragIndex < ui->scrollArea->widget()->layout()->count())
				{
					einzel = qobject_cast<Einzelbeitrag*>(this->ui->scrollArea->widget()->layout()->itemAt(einzelbeitragIndex)->widget());
					while (einzel == NULL)
					{
						einzelbeitragIndex++;
						if (einzelbeitragIndex >= ui->scrollArea->widget()->layout()->count())
						{
							break;
						}
						einzel = qobject_cast<Einzelbeitrag*>(this->ui->scrollArea->widget()->layout()->itemAt(einzelbeitragIndex)->widget());
					}
				}
				if (einzel != NULL)
				{
					QString setStr = QStringLiteral("UPDATE Beitrag SET FileName='%1',TitelNr=%2,Name='%3',Art='%4',Text='%5',Bibelstelle='%6' WHERE Id=%7")
							.arg(einzel->fileName()).arg(einzel->titelNr()).arg(einzel->name()).arg(einzel->art()).arg(einzel->text()).arg(einzel->bibelstelle()).arg(testQuery.record().value("Id").toLongLong());
					QSqlQuery setQuery(db);
					setQuery.exec(setStr);

					einzelbeitragIndex++;
				}
				else
				{
					toDeleteRowIds << testQuery.record().value("Id").toLongLong();
				}
			}
		}

		foreach (qlonglong toDeleteId, toDeleteRowIds)
		{
			QString delStr = QStringLiteral("DELETE FROM `Beitrag` WHERE Id=%1").arg(toDeleteId);
			QSqlQuery delQuery(db);
			delQuery.exec(delStr);
		}

		if (einzelbeitragIndex < this->ui->scrollArea->widget()->layout()->count())
		{
			this->addRestEinzelbeitrag(&db, einzelbeitragIndex, gottesdienstId);
		}
	}
	else
	{
		qlonglong nextId = -1;
		{
			QSqlQuery maxId(db);
			if (maxId.exec("SELECT MAX( Id ) FROM GottesdienstId"))
			{
				if (maxId.next())
				{
					nextId = maxId.record().value(0).toLongLong();
				}
			}
		}

		nextId++;

		QString insert = QStringLiteral("INSERT INTO GottesdienstId(Id, Datum, Zeit, Path, Beschreibung) VALUES (%1,'%2','%3','%4','%5')").arg(nextId).arg(ui->dateEdit->date().toString("yyyy-MM-dd")).arg(ui->comboBoxZeit->currentText()).arg(path).arg(ui->lineEditBeschreibung->text().replace('\'', '\"'));
		QSqlQuery insertQuery(db);
		if (insertQuery.exec(insert) == false)
		{
            auto err = db.lastError();
			ui->statusBar->showMessage("Fehler beim Hinzufügen.");
		}
		else
		{
			this->addRestEinzelbeitrag(&db, 0, nextId);
		}
	}
}

void MainWindow::on_lineEditGottesdiensteFilter_textChanged(const QString &arg1)
{
	this->sqlModel->setFilter("Beschreibung LIKE '%" + arg1 + "%'");
}

void MainWindow::on_tableViewGottesdienste_doubleClicked(const QModelIndex &index)
{
	QString time = this->sqlModel->record(index.row()).value("Zeit").toString();
	QDate date = this->sqlModel->record(index.row()).value("Datum").toDate();

	ui->dateEdit->setDate(date);
	ui->comboBoxZeit->setCurrentText(time);
}

void MainWindow::updateFileWatcher()
{
	this->fileList->changeCurrent(this->detSubPath(), ui->dateEdit->date(), ui->comboBoxZeit->currentText());

    auto list = this->fileWatcher->directories();
    if (!list.empty())
    {
        this->fileWatcher->removePaths(list);
    }

	if (this->capturePathScanner->hashDateDirInfo.contains(ui->dateEdit->date()))
	{
		QString path = this->findPathOfDateAtPathScanner(this->capturePathScanner);
		if ( ! path.isEmpty())
		{
			this->fileWatcher->addPath(path);
		}
	}

	if (this->mp3PathScanner->hashDateDirInfo.contains(ui->dateEdit->date()))
	{
		QString path = this->findPathOfDateAtPathScanner(this->mp3PathScanner);
		if ( ! path.isEmpty())
		{
			this->fileWatcher->addPath(path);
		}
	}

	this->mp3Converter->setSubPath(this->detSubPath());
}

void MainWindow::on_buttonStartFileWatcher_clicked()
{
	this->on_comboBoxZeit_currentIndexChanged("");
}

QStringList MainWindow::searchNewWords(const QString& tableName, const QStringList& wordList, int minCount)
{
	QStringList newWordList;

    QSqlDatabase db = QSqlDatabase::database(dbName);

	QSqlQuery query(db);

	if (query.exec(QStringLiteral("SELECT * FROM %1").arg(tableName)))
	{
		QString regExpStr = QString("[A-Za-zÄÖÜäöüß]{%1,}").arg(minCount);
		QRegExp regexp(regExpStr);

		QStringList existingWords;

		while (query.next())
		{
			QString fullName = query.value(0).toString();
			if (regexp.indexIn(fullName) > -1)
			{
				QStringList matched = regexp.capturedTexts();

				if (matched.count() > 1)
				{
					matched = matched.mid(1);
				}

				existingWords << matched;
			}
		}
		foreach (const QString& singleText, wordList)
		{
			if ( ( ! existingWords.contains(singleText)) && ! newWordList.contains(singleText) )
			{
				newWordList << singleText;
			}
		}
	}

	return newWordList;
}

void MainWindow::on_actionNamen_triggered()
{
	QStringList newWordList = searchNewWords(QStringLiteral("Names"), this->nameFilter->readSqlTable(), 3);

	if ( ! newWordList.isEmpty())
	{
		ListSelectDialog* selectDialog = new ListSelectDialog(newWordList, this);
		connect(selectDialog, &ListSelectDialog::signalAccepted, this, &MainWindow::slotAddNewNames);
	}
}

QList<Einzelbeitrag*> MainWindow::detEinzelbeitragList()
{
	QList<Einzelbeitrag*> einzelbeitragList;

	for (int i = 0; i < this->ui->scrollArea->widget()->layout()->count(); i++)
	{
		QWidget* widget = this->ui->scrollArea->widget()->layout()->itemAt(i)->widget();
		Einzelbeitrag* einzel = qobject_cast<Einzelbeitrag*>(widget);
		if (einzel != NULL)
		{
			einzelbeitragList << einzel;
		}
	}

	return einzelbeitragList;
}

void MainWindow::slotAddNewNames(QStringList newWordList)
{
	this->nameFilter->addNames(newWordList);

	QList<Einzelbeitrag*> einzelbeitragList = detEinzelbeitragList();

	bool isFirst = true;
	foreach (Einzelbeitrag* beitrag, einzelbeitragList)
	{
		beitrag->reReadNames(isFirst);
		if (isFirst)
		{
			isFirst = false;
		}
	}
}

void MainWindow::slotAddNewWords(QStringList newWordList)
{
	QString exporTableName = "Words";
	const QString& tableName = exporTableName;
	QString exportColumnName = "Word";
	const QString& columnName = exportColumnName;

    QSqlDatabase db = QSqlDatabase::database(dbName);

	QSqlQuery query(db);

	foreach (const QString& newWord, newWordList)
	{
		if ( ! query.exec(QStringLiteral("INSERT INTO %1 (%2) VALUES ('%3')").arg(tableName).arg(columnName).arg(newWord)))
		{
			qDebug(query.lastError().text().toLatin1().constData());
		}
	}


	QList<Einzelbeitrag*> einzelbeitragList = detEinzelbeitragList();

	bool isFirst = true;
	foreach (Einzelbeitrag* beitrag, einzelbeitragList)
	{
		beitrag->reReadWords(isFirst);
		if (isFirst)
		{
			isFirst = false;
		}
	}
}

void MainWindow::on_actionWorte_triggered()
{
	int minCount = 4;

	QStringList words;

    QSqlDatabase db = QSqlDatabase::database(dbName);

	QSqlQuery query(db);

	QString queryStr = "SELECT Text FROM Beitrag";

	if (query.exec(queryStr))
	{
		QString regExpStr = QString("[A-Za-zÄÖÜäöüß]{%1,}").arg(minCount);
        QRegularExpression regexp(regExpStr);

        QRegularExpression regexpSplit("[^A-Za-zÄÖÜäöüß]");

		while (query.next())
		{
			QStringList values = query.value(0).toString().split(regexpSplit);

			if ( ! values.isEmpty())
			{
				QStringList matched = values.filter(regexp);

				words << matched;
			}
		}

		if ( ! words.isEmpty())
		{
			QStringList newWordList = searchNewWords(QStringLiteral("Words"), words, 4);

			if ( ! newWordList.isEmpty())
			{
				ListSelectDialog* selectDialog = new ListSelectDialog(newWordList, this);
				connect(selectDialog, &ListSelectDialog::signalAccepted, this, &MainWindow::slotAddNewWords);
				selectDialog->show();
				selectDialog->exec();
			}
		}
	}
}

void MainWindow::on_actionPrintpreview_triggered()
{
	static ListPrintDialog* printDialog = NULL;

	if (printDialog == NULL)
	{
		printDialog = new ListPrintDialog(this);
	}

	printDialog->setTitleItems(this->detEinzelbeitragList());

	printDialog->date = ui->dateEdit->date().toString("dd.MM.yyyy ") + ui->comboBoxZeit->currentText();
	printDialog->headText = ui->lineEditBeschreibung->text();

	printDialog->show();
	printDialog->exec();
}

void MainWindow::on_lineEditSearch_textChanged(const QString &arg1)
{
	QSqlQueryModel* model = qobject_cast<QSqlQueryModel*>(ui->tableViewSelect->model());
	if (model)
	{
		QString argument = QString(arg1).replace("'", "");
		if ( ( ! argument.isEmpty() ) && argument.count() > 2 )
		{
			model->setQuery(this->searchQuery.arg(argument), this->db);
		}
	}
}

void MainWindow::on_toolButtonGotoFromSearch_clicked()
{
	QModelIndex index = ui->tableViewSelect->selectionModel()->currentIndex();
	if ( ! index.isValid())
	{
		return;
	}

	QSqlQueryModel* model = qobject_cast<QSqlQueryModel*>(ui->tableViewSelect->model());
	if (model)
	{
		QDate date = model->data(model->index(index.row(), 0)).toDate();
		if (date.isValid())
		{
			QString time = model->data(model->index(index.row(), 1)).toString();
			ui->dateEdit->setDate(date);
			ui->comboBoxZeit->setCurrentText(time);

			ui->tabWidget->setCurrentIndex(0);
		}
	}
}

void MainWindow::on_toolButtonOpenPath_clicked()
{
	if ( ! ui->labelPath->text().isEmpty() )
	{
		QProcess::execute(QStringLiteral("explorer \"%1\"").arg(ui->labelPath->text().replace('/', '\\')));
	}
}

void MainWindow::on_toolButtonReRead_clicked()
{
	QString currentTime = ui->comboBoxZeit->currentText();

	if (currentTime == ui->comboBoxZeit->itemText(ui->comboBoxZeit->count() - 1))
	{
		ui->comboBoxZeit->setCurrentIndex(0);
		ui->comboBoxZeit->setCurrentIndex(ui->comboBoxZeit->count() - 1);
	}
	else
	{
		ui->comboBoxZeit->setCurrentIndex(ui->comboBoxZeit->count() - 1);
		ui->comboBoxZeit->setCurrentText(currentTime);
	}
}

QString MainWindow::detCopyCenterSubPath(const QDate& date, const QString& dayTime)
{
	QString copyCenterSubPath = this->ui->globalSettings->copyCenterSubPathFormat();

	int indexLt = copyCenterSubPath.indexOf('<');
	int indexGt = copyCenterSubPath.indexOf('>');
	while (indexLt > -1 || indexGt > -1)
	{
		if (indexLt == -1 || indexGt == -1)
		{
			if (indexLt > -1)
			{
				copyCenterSubPath.remove('<');
			}
			else if (indexGt > -1)
			{
				copyCenterSubPath.remove('>');
			}
		}
		else if (indexLt > indexGt)
		{
			copyCenterSubPath.remove(indexGt, 1);
		}
		else
		{
			QString midString = copyCenterSubPath.mid(indexLt + 1, indexGt - indexLt - 1).toLower().replace('m', 'M');

			if (midString == "c")
			{
				if (dayTime.startsWith('T', Qt::CaseInsensitive))
				{
					int nr = dayTime.right(1).toInt();
					if (nr > 0)
					{
						midString = QString(1, QChar('a' - 1 + nr));
					}
				}
				else if (dayTime.startsWith('M', Qt::CaseInsensitive))
				{
					midString = "a";
				}
				else if (dayTime.startsWith('Z', Qt::CaseInsensitive))
				{
					midString = "b";
				}
				else if (dayTime.startsWith('N', Qt::CaseInsensitive))
				{
					midString = "c";
				}
				else
				{
                    midString = "c";
				}
			}
            else if (midString == "d")
            {
                switch (date.dayOfWeek())
                {
                case 1:
                    midString = "Mo";
                    break;
                case 2:
                    midString = "Di";
                    break;
                case 3:
                    midString = "Mi";
                    break;
                case 4:
                    midString = "Do";
                    break;
                case 5:
                    midString = "Fr";
                    break;
                case 6:
                    midString = "Sa";
                    break;
                default:
                    midString = "So";
                    break;
                }
            }
            else if (midString == "t")
            {
                midString = ui->comboBoxZeit->currentText();
            }
			else
			{
				midString = date.toString(midString);
			}

			copyCenterSubPath = copyCenterSubPath.left(indexLt) + midString + copyCenterSubPath.mid(indexGt + 1);
		}

		indexLt = copyCenterSubPath.indexOf('<');
		indexGt = copyCenterSubPath.indexOf('>');
	}

	return copyCenterSubPath;
}

bool MainWindow::copyToCopyCenter(const QDate& date, const QString& dayTime, bool forceUpdate)
{
    QString srcPath;

    QList<PathScannerThread::SDirInfo> list = this->mp3PathScanner->hashDateDirInfo.values(date);
    for (const PathScannerThread::SDirInfo& item : list)
    {
        if (item.dayTime == dayTime)
        {
            srcPath = item.path;
            break;
        }
    }

    if (srcPath.isEmpty() && list.count() == 1
            && list.first().dayTime.left(1).toUpper() == dayTime.left(1).toUpper())
    {
        srcPath = list.first().path;
    }

    if (srcPath.isEmpty() && this->ui->dateEdit->date() == date
            && dayTime.startsWith(this->ui->comboBoxZeit->currentText().left(1), Qt::CaseInsensitive)
            && ( ! dayTime.startsWith("t", Qt::CaseInsensitive) || dayTime.right(1) == this->ui->comboBoxZeit->currentText().right(1) ))
    {
        QString preSubPath = this->ui->globalSettings->subPath(date);

        QString mp3Path = this->ui->globalSettings->mp3Path();

        if ( ! mp3Path.isEmpty() )
        {
            if ( ! mp3Path.endsWith('/'))
            {
                mp3Path += QStringLiteral("/");
            }

            if (QDir(mp3Path + preSubPath).exists())
            {
                mp3Path += preSubPath;
                if ( ! mp3Path.endsWith('/'))
                {
                    mp3Path += QStringLiteral("/");
                }
            }

            mp3Path += this->detSubPath();
            if ( ! mp3Path.endsWith('/'))
            {
                mp3Path += QStringLiteral("/");
            }

            if (QDir(mp3Path).exists())
            {
                srcPath = mp3Path;
            }
        }
    }

	if (srcPath.isNull())
	{
		QMessageBox::information(this, "Quelle", "Quelle nicht gefunden");
	}
	else
    {
        QString tarPath = this->ui->globalSettings->copyCenterServicePath();

        QString copyCenterSubPath = this->detCopyCenterSubPath(date, dayTime);

        if ( ! tarPath.endsWith('/') && ! copyCenterSubPath.startsWith('/'))
        {
            tarPath += "/";
        }
        tarPath += copyCenterSubPath;

        if (forceUpdate || ! QDir(tarPath).exists())
        {
			if (apFileUtils::copyRecursively(srcPath, tarPath))
			{
				if ( ! tarPath.endsWith('/'))
				{
					tarPath += '/';
				}
				QFile file(tarPath + dayTime + ".txt");
				file.open(QFile::WriteOnly);
                file.write("");
				file.close();

				return true;
			}
        }
    }

	return false;
}

void MainWindow::on_toolButtonPush_clicked()
{
	if (this->copyToCopyCenter(ui->dateEdit->date(), ui->comboBoxZeit->currentText(), true))
	{
		QMessageBox::information(this, "Fertig", "Kopieren auf UsbCopyCenter ist fertig.");
	}
	else
	{
		QMessageBox::warning(this, "Fehler", "Nicht kopiert");
	}
}

void MainWindow::on_actionAlles_kopieren_triggered()
{
    QMessageBox::information(this, "Abgeschaltet", "Diese Funktion ist abgeschaltet.");// Überbrückt, ist bei uns nicht mehr notwendig.
    return;
	QString select = "SELECT Datum,Zeit FROM `audio`.`GottesdienstId` ORDER BY Datum,Zeit";

	QSqlQuery query = this->db.exec(select);

	int indexDate = query.record().indexOf("Datum");
	int indexDayTime = query.record().indexOf("Zeit");

	while (query.next())
	{
		QDate date = query.record().value(indexDate).toDate();
		QString dayTime = query.record().value(indexDayTime).toString();

		this->copyToCopyCenter(date, dayTime);
	}
}

void MainWindow::on_comboBoxZeit_currentIndexChanged(int index)
{
   on_comboBoxZeit_currentIndexChanged("");
}

