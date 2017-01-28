#include "apFileUtils.h"

#include <QDir>

// taken from utils/fileutils.cpp. We can not use utils here since that depends app_version.h.
bool apFileUtils::copyRecursively(const QString &srcFilePath,
							const QString &tgtFilePath)
{
	QFileInfo srcFileInfo(srcFilePath);
	if (srcFileInfo.isDir()) {
		if (!QDir().mkpath(tgtFilePath))
			return false;
		QDir sourceDir(srcFilePath);
		QStringList fileNames = sourceDir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System);
		foreach (const QString &fileName, fileNames) {
			const QString newSrcFilePath
					= srcFilePath + QLatin1Char('/') + fileName;
			const QString newTgtFilePath
					= tgtFilePath + QLatin1Char('/') + fileName;
			if (!copyRecursively(newSrcFilePath, newTgtFilePath))
				return false;
		}
	} else {
		if (!QFile::copy(srcFilePath, tgtFilePath))
			return false;
	}
	return true;
}
