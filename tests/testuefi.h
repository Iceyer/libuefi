#ifndef TESTUEFI_H
#define TESTUEFI_H

#include <QObject>

class TestUEFI : public QObject
{
    Q_OBJECT
public:
    explicit TestUEFI(QObject *parent = 0);

signals:

private slots:
    void testAll();
};

#endif // TESTUEFI_H
