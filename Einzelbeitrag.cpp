#include "Einzelbeitrag.h"
#include "ui_Einzelbeitrag.h"

#include "fileref.h"
#include "tag.h"

#include "LsFileList.h"
#include "lsGlobalSettings.h"
#include "lsKeyAndFilterListEditor.h"
#include "lsMp3Converter.h"
#include "mainwindow.h"

Einzelbeitrag::Einzelbeitrag(MainWindow* parentWindow, QWidget *parent) :
    QWidget(parent),
	ui(new Ui::Einzelbeitrag),
	timerUpdate(NULL)
  , fileList(NULL)
  , parentWindow(parentWindow)
{
    ui->setupUi(this);
}

Einzelbeitrag::Einzelbeitrag(const QSqlRecord &record, const QString& subPath, lsGlobalSettings* settings, LsFileList* fileList, MainWindow* parentWindow, QWidget *parent) :
	QWidget(parent),
	ui(new Ui::Einzelbeitrag),
	subPath(subPath),
	globalSettings(settings),
	timerUpdate(NULL)
  , fileList(fileList)
  , parentWindow(parentWindow)
{
    ui->setupUi(this);

//    qlonglong id = record.value("Id").toLongLong();
//    qlonglong GottesdienstId = record.value("GottesdienstId").toLongLong();
	this->fillWithRecord(record);

	this->timerUpdate = new QTimer(this);
	connect(this->timerUpdate, &QTimer::timeout, this, &Einzelbeitrag::slotUpdateStates);

	this->timerUpdate->start(4000);
}

Einzelbeitrag::Einzelbeitrag(const QString& fileName, int title, lsKeyAndFilterListEditor* names, lsKeyAndFilterListEditor* types, lsKeyAndFilterListEditor* bible, const QString& subPath, lsGlobalSettings* settings, LsFileList* fileList, MainWindow* parentWindow, QWidget* parent) :
	QWidget(parent),
	ui(new Ui::Einzelbeitrag),
	subPath(subPath),
	globalSettings(settings)
  , fileList(fileList)
  , parentWindow(parentWindow)
{
	ui->setupUi(this);

	this->names = names;
	this->types = types;
	this->bible = bible;

	ui->comboBoxFileName->addItem(fileName);
	ui->comboBoxFileName->setCurrentIndex(0);

	ui->titelnummerSpinBox->setValue(title);

	this->date = this->detDate();

	this->timerUpdate = new QTimer(this);
	connect(this->timerUpdate, &QTimer::timeout, this, &Einzelbeitrag::slotUpdateStates);

	this->reRead();

	this->timerUpdate->start(3000);
}

Einzelbeitrag::~Einzelbeitrag()
{
	delete ui;
}

QString Einzelbeitrag::art() const
{
	return ui->beitragsartLineEdit->text().replace('\'', '\"');
}

QString Einzelbeitrag::bibelstelle() const
{
	return ui->bibelstelleLineEdit->text().replace('\'', '\"');
}

QDate Einzelbeitrag::detDate() const
{
	QDate toReturn;

	QRegExp deDate("[0-9][0-9]?\\.[0-9][0-9]?\\.[0-9][0-9][0-9]?[0-9]?");
	if (deDate.indexIn(this->subPath) > -1)
	{
		QString dateStr = deDate.capturedTexts().last();
		if (dateStr.length() == 10)
		{
			toReturn = QDate::fromString(dateStr, "dd.MM.yyyy");
		}
		else
		{
			toReturn = QDate::fromString(dateStr, "d.M.yy");
		}
		if ( ! toReturn.isValid())
		{
			toReturn = QDate::fromString(dateStr, "d.M.yyyy");
		}
	}
	if ( ! toReturn.isValid())
	{
		QRegExp enDate("[0-9][0-9][0-9]?[0-9]?[-][0-9][0-9]?[-][0-9][0-9]?");
		if (enDate.indexIn(this->subPath) > -1)
		{
			QString dateStr = enDate.capturedTexts().last();
			toReturn = QDate::fromString(dateStr);
		}
	}

	return toReturn;
}

QString Einzelbeitrag::detFullPath(const QString& basePath)
{
	if (basePath.isEmpty())
	{
		return basePath;
	}

	QString preSubPath = this->globalSettings->subPath(this->date);
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
		path += this->subPath;
	}
	else
	{
		path += this->subPath;
	}

	return path;
}

QString Einzelbeitrag::fileName() const
{
	return ui->comboBoxFileName->currentText();
}

void Einzelbeitrag::fillWithRecord(const QSqlRecord &record)
{
	int     TitelNr = record.value("TitelNr").toInt();
	QString fileName = record.value("FileName").toString();
	QString Name = record.value("Name").toString();
	QString Art = record.value("Art").toString();
	QString Text = record.value("Text").toString();
	QString Bibelstelle = record.value("Bibelstelle").toString();

	this->date = this->detDate();

	ui->beitragsartLineEdit->setText(Art);
	ui->bibelstelleLineEdit->setText(Bibelstelle);
	ui->comboBoxFileName->setCurrentText(fileName);
	ui->nameLineEdit->setText(Name);
	ui->themaLineEdit->setText(Text);
	ui->titelnummerSpinBox->setValue(TitelNr);
}

QString Einzelbeitrag::findBestMatch(const QString& fileName, const QStringList& list, QLineEdit* lineEdit)
{
	QString match;

	foreach (const QString& item, list)
	{
		QRegExp rx(item);
		if (rx.indexIn(fileName) > -1)
		{
			QString str = rx.capturedTexts().first();
			if (match.count() < str.count())
			{
				match = str;
			}
		}
	}

	match = match.simplified();

	if (match.startsWith(QStringLiteral("(")) && match.endsWith(QStringLiteral(")")))
	{
		match = match.mid(1, match.count() - 2).simplified();
	}

	if (match.left(1) != match.left(1).toUpper())
	{
		lineEdit->setText(match.left(1).toUpper() + match.mid(1));
	}
	else
	{
		lineEdit->setText(match);
	}

	if (match.isEmpty() == false)
	{
		return QString(fileName).remove(match);
	}
	return fileName;
}

void Einzelbeitrag::intUpdateState(QLabel* label, eState& curState, const QString& path, eState nonExistState, const QString& extension)
{
	eState newState = curState;
	if ( path.isEmpty() || ! QDir(path).exists() )
	{
		newState = nonExistState;
	}
	else if (QFile(path + "/" + this->newFileName() + extension).exists())
	{
		newState = eStateReady;
	}
	else
	{
		QStringList filenames = QDir(path).entryList(QStringList("*.wav") << "*.mp3", QDir::Files);
		if (filenames.isEmpty())
		{
			newState = nonExistState;
		}
		else
		{
			QString regStr = QStringLiteral("^0*%1[^0-9].* ").arg(this->titelNr());
			QStringList test = filenames.filter(QRegExp(regStr));
			test << filenames.filter(QRegExp(QStringLiteral("^0*%1 .+").arg(this->titelNr())));
			test << filenames.filter(QRegExp(QStringLiteral("^Tit[le][el]0*%1[^0-9]").arg(this->titelNr())));

			test.removeDuplicates();

			if (test.count() == 1)
			{
				newState = eStateName;
			}
			else if (test.count() > 1)
			{
				newState = eStateNonExit;
			}
		}
	}

	if (newState != curState)
	{
		curState = newState;

		switch (newState)
		{
		case eStateReady:
			label->setStyleSheet("background: green");
			break;
		case eStateName:
			label->setStyleSheet("background: yellow");
			break;
		default:
			label->setStyleSheet("background: red");
			break;
		}
	}
}

QString Einzelbeitrag::name() const
{
	return ui->nameLineEdit->text().replace('\'', '\"');
}

QString Einzelbeitrag::newFileName() const
{
	QString name = QStringLiteral("%1 %2 %3 %4 %5").arg(ui->titelnummerSpinBox->value(), 2, 10, QLatin1Char('0')).arg(ui->nameLineEdit->text()).arg(ui->beitragsartLineEdit->text())
			.arg(ui->themaLineEdit->text()).arg(ui->bibelstelleLineEdit->text()).simplified();
	if (name.count() > 256)
	{
		name = QStringLiteral("%1 %2 %3 %4").arg(ui->titelnummerSpinBox->value(), 2, 10, QLatin1Char('0')).arg(ui->nameLineEdit->text()).arg(ui->beitragsartLineEdit->text())
				.arg(ui->themaLineEdit->text()).simplified();
		if (name.count() > 256)
		{
			name = QStringLiteral("%1 %2 %3 %4").arg(ui->titelnummerSpinBox->value(), 2, 10, QLatin1Char('0')).arg(ui->nameLineEdit->text()).arg(ui->beitragsartLineEdit->text())
						.arg(ui->bibelstelleLineEdit->text()).simplified();
			if (name.count() > 256)
			{
				name = QStringLiteral("%1 %2 %3").arg(ui->titelnummerSpinBox->value(), 2, 10, QLatin1Char('0')).arg(ui->nameLineEdit->text()).arg(ui->beitragsartLineEdit->text())
							.simplified();

				if (name.count() > 256)
				{
					QRegExp regExp("[\\D\\W]");
					while (name.count() > 256)
					{
						int oldLen = name.count();
						name = name.section(regExp, 0, -2);
						if (oldLen == name.count())
						{
							name = name.left(256);
						}
					}
				}
			}
		}
	}

	return name;
}

void Einzelbeitrag::reReadNames(bool isReadFromServer)
{
	ui->beitragsartLineEdit->readWordsAndCreateCompleter(isReadFromServer);
}

void Einzelbeitrag::reReadWords(bool isReadFromServer)
{
	ui->themaLineEdit->readWordsAndCreateCompleter(isReadFromServer);
}

void Einzelbeitrag::setAutoCompleteDatabase(QSqlDatabase db)
{
	ui->beitragsartLineEdit->createFromSimpleDatabaseTable(db, QStringLiteral("Beitragsarten"));
	ui->bibelstelleLineEdit->createFromSimpleDatabaseTable(db, QStringLiteral("Bibelstellen"));
	ui->nameLineEdit->createFromSimpleDatabaseTable(db, QStringLiteral("Names"));
	ui->themaLineEdit->createFromSimpleDatabaseTable(db, QStringLiteral("Words"));
}

void Einzelbeitrag::reRead()
{
	if (this->types.isNull() || this->names.isNull() || this->bible.isNull())
	{
		return;
	}
	const QStringList& types = this->types->filterList();
	const QStringList& names = this->names->filterList();
	const QStringList& bible = this->bible->filterList();
	const QString& fileName = ui->comboBoxFileName->currentText();
	QString rest = fileName;

	if (rest.endsWith(".mp3", Qt::CaseInsensitive) || rest.endsWith(".wav", Qt::CaseInsensitive))
	{
		rest.chop(4);
	}

	QRegExp ex("^[Tt]it[el][le] *([0-9]+)\\.?");
	if (ex.indexIn(fileName) > -1)
	{
		rest = rest.remove(0, ex.capturedTexts().first().length());
	}
	else
	{
		QRegExp ex2("^[0-9]+\\.?");
		if (ex2.indexIn(fileName) > -1)
		{
			rest = rest.remove(0, ex2.capturedTexts().first().length());
		}
	}

	rest = this->findBestMatch(rest, bible, ui->bibelstelleLineEdit);

	rest = this->findBestMatch(rest, names, ui->nameLineEdit);

	rest = this->findBestMatch(rest, types, ui->beitragsartLineEdit);

	rest = rest.remove(QRegExp("\\( *\\)")).simplified();

	if (rest.startsWith(QStringLiteral("(")) && rest.endsWith(QStringLiteral(")")))
	{
		rest = rest.mid(1, rest.count() - 2).simplified();
	}

	ui->themaLineEdit->setText(rest);
}

void Einzelbeitrag::setFileName(const QFileInfo &file)
{
    if (file.isFile())
    {
		this->ui->comboBoxFileName->clear();
		this->ui->comboBoxFileName->addItem(file.fileName());
		this->ui->comboBoxFileName->setCurrentIndex(0);
        QString fileName = file.fileName();
        if (fileName.startsWith(QStringLiteral("Titel"), Qt::CaseInsensitive))
        {
            fileName = fileName.mid(5);
        }
    }
}

void Einzelbeitrag::insertId3Tag(const QString& path, const QHash<int,QString>& names)
{
	int nr = this->ui->titelnummerSpinBox->value();
	QString fileName = names.value(nr);
	if (QFile(path + fileName).exists())
	{
		QString filePath = path + fileName;
#ifdef _WIN32
        TagLib::FileRef tagFile(filePath);
#else
        TagLib::FileRef tagFile(filePath.toUtf8().data());
#endif
		if ( ! tagFile.isNull() )
		{
			TagLib::Tag* tags = tagFile.tag();
			if (tags != NULL)
			{
				QString album = this->parentWindow->getAlbum();
                tags->setAlbum((this->fileList->getDate().toString("ddd dd.MM.yyyy ") + this->fileList->getDateTime() + " " + album).simplified().toStdWString());
                tags->setArtist(this->ui->nameLineEdit->text().toStdWString());
                tags->setTitle((this->art() + " " + this->text() + " " + this->bibelstelle()).simplified().toStdWString());
                tags->setGenre(this->art().toStdWString());
				tags->setTrack(this->ui->titelnummerSpinBox->value());
				tags->setYear(this->fileList->getDate().year());

				if ( ! tagFile.save() )
				{
					qDebug("Fehler");
				}
			}
		}
	}
}

void Einzelbeitrag::setId3Tag()
{
	QString path = this->fileList->getPathServer();
	if ( ! path.isEmpty() )
	{
		this->insertId3Tag(path, this->fileList->getDirServer());
	}
	path = this->fileList->getPathMp3();
	if ( ! path.isEmpty() )
	{
		this->insertId3Tag(path, this->fileList->getDirLocal());
	}
}

void Einzelbeitrag::setTitleNr(int newNumber)
{
	this->ui->titelnummerSpinBox->setValue(newNumber);
}

void Einzelbeitrag::slotMp3Progress(int percent)
{
	ui->progressBar->setValue(percent);
}

void Einzelbeitrag::slotMp3ConvertFinished(lsMp3Converter* converter)
{
	disconnect(converter, &lsMp3Converter::signalFinished, this, &Einzelbeitrag::slotMp3ConvertFinished);
	disconnect(converter, &lsMp3Converter::signalProgress, this, &Einzelbeitrag::slotMp3Progress);

	QString fileName = ui->comboBoxFileName->currentText();
	fileName = fileName.section('.', 0, -2) + ".mp3";
	ui->comboBoxFileName->clear();
	ui->comboBoxFileName->addItem(fileName);
	ui->comboBoxFileName->setCurrentIndex(0);
	ui->progressBar->setValue(0);

	emit signalSync(this);
}

QString Einzelbeitrag::intUpdateState(eState& curState, QLabel* label, QHash<int,QString> files, int titleNr, QString rightFileName, const QString& path)
{
	QString filePath;

	eState newState = eStateNonExit;
	if (files.contains(titleNr))
	{
		QString fileName = files.value(titleNr);

		filePath = path + "/" + fileName;

		if (fileName.endsWith(".wav", Qt::CaseInsensitive) || fileName.endsWith(".mp3", Qt::CaseInsensitive))
		{
			fileName.chop(4);
		}
		if (fileName == rightFileName)
		{
			newState = eStateReady;
			label->setStyleSheet("background: green");
		}
		else
		{
			newState = eStateName;
			label->setStyleSheet("background: yellow");
		}
	}

	if (newState != curState)
	{
		curState = newState;
	}

	return filePath;
}

void Einzelbeitrag::slotUpdateStates()
{
	if (NULL == this->fileList) { return; }

	QString rightFileName = this->newFileName();
	int titleNr = ui->titelnummerSpinBox->value();

	QString filePath = this->intUpdateState(this->stateCapture, ui->labelLocal, this->fileList->getDirCapture(), titleNr, rightFileName, this->fileList->getPathCapture());
	QString mp3FilePath = this->intUpdateState(this->stateLocal, ui->labelMp3, this->fileList->getDirLocal(), titleNr, rightFileName, this->fileList->getPathMp3());
	QString serverFilePath = this->intUpdateState(this->stateServer, ui->labelServer, this->fileList->getDirServer(), titleNr, rightFileName, this->fileList->getPathServer());

	if (filePath.isEmpty())
	{
		if (mp3FilePath.isEmpty())
		{
			filePath = serverFilePath;
		}
		else
		{
			filePath = mp3FilePath;
		}
	}

	if ( ! filePath.isEmpty())
	{
#ifdef _WIN32
        TagLib::FileRef tagFile(filePath);
#else
        TagLib::FileRef tagFile(filePath.toUtf8().data());
#endif
        if ( ! tagFile.isNull() )
		{
			TagLib::AudioProperties* propertys = tagFile.audioProperties();
			if (propertys != NULL)
			{
				int seconds = propertys->length();
				QTime time(0,0,0);
				time = time.addSecs(seconds);
				ui->labelTime->setText(QStringLiteral("Zeit: %1").arg(time.toString("h:mm:ss")));
			}
		}
	}
}

QString Einzelbeitrag::text() const
{
	return ui->themaLineEdit->text().replace('\'', '\"');
}

int Einzelbeitrag::titelNr() const
{
    return this->ui->titelnummerSpinBox->value();
}

void Einzelbeitrag::on_toolButtonRemoveThis_clicked()
{
    emit signalRemoveThis(this);
}

void Einzelbeitrag::on_toolButtonAnnNext_clicked()
{
    emit signalAddNextTo(this);
}

void Einzelbeitrag::on_toolButtonStartPlay_clicked()
{
	QString fileName = this->fileName();
	if (fileName.isEmpty()) { return; }

	emit signalPlay(this->fileName());
}

void Einzelbeitrag::on_toolButtonMp3_clicked()
{
	emit signalConvertToMp3(this);
}

void Einzelbeitrag::on_toolButtonRename_clicked()
{
	emit signalRename(this);
}

void Einzelbeitrag::on_toolButtonReread_clicked()
{
	this->reRead();
}

void Einzelbeitrag::on_buttonSync_clicked()
{
	emit signalSync(this);
}

void Einzelbeitrag::on_toolButtonSetId3Tag_clicked()
{
	this->setId3Tag();
}
