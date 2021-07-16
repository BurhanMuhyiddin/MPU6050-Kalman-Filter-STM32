#ifndef SERIALPLOT_H
#define SERIALPLOT_H

#include <QObject>

class SerialPlot : public QObject
{
    Q_OBJECT
public:
    explicit SerialPlot(QObject *parent = nullptr);

signals:

public slots:
};

#endif // SERIALPLOT_H