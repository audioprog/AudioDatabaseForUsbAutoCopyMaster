#ifndef APFIEUTILS_H
#define APFIEUTILS_H

#include <QString>

class apFileUtils
{
public:
	static bool copyRecursively(const QString& srcFilePath, const QString& tgtFilePath);
};

#endif // APFIEUTILS_H
