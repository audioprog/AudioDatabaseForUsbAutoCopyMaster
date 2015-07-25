#ifndef LSOUTPUTSOURCE_H
#define LSOUTPUTSOURCE_H

#include <QObject>

class lsOutputSource : public QObject
{
    Q_OBJECT
public:
    explicit lsOutputSource(QObject *parent = 0);
    ~lsOutputSource();

signals:

public slots:
};

#endif // LSOUTPUTSOURCE_H
