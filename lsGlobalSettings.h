#ifndef LSGLOBALSETTINGS_H
#define LSGLOBALSETTINGS_H

#include <QWidget>

#include <QDate>

namespace Ui {
class lsGlobalSettings;
}

class lsGlobalSettings : public QWidget
{
	Q_OBJECT

public:
	explicit lsGlobalSettings(QWidget *parent = 0);
	~lsGlobalSettings();

	static QString capturePath();

	void load();

	static QString mp3Path();

	void save();

	static QString serverPath();

	static QString subPath(const QDate& date );

private slots:
	void on_toolButtonCapturePath_clicked();

	void on_toolButtonMp3Path_clicked();

	void on_toolButtonServerPath_clicked();

private:
	Ui::lsGlobalSettings *ui;

	static lsGlobalSettings* self;
};

#endif // LSGLOBALSETTINGS_H
