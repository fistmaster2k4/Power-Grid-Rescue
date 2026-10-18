#ifndef ROCCATPOWERGRID_RECONSTRUCTED_H
#define ROCCATPOWERGRID_RECONSTRUCTED_H

/*
    Clean-room reconstruction of the minimum ROCCAT Power-Grid 0461
    native function-provider interface required by Sample2-style plugins.

    Evidence basis (18 Oct 2026):
      - ROCCATPowerGrid 0461 host executable
      - packaged 2014 Sample2.dll from ExamplePlugin3.rpgp
      - early Sample2 source project

    Verified MSVC x86 vtable layout after QObject's virtuals:
      +0x2c  QString GetName()
      +0x30  void <state setter>(QVariantMap)       [original name unknown]
      +0x34  QVariantMap <state getter>()           [original name unknown]
      +0x38  QVariant GetValue(QString,const QVariantList&)
      +0x3c  const QStringList& GetList()            [IFuncProvider adds this]

    The reconstructed names SetPluginData/GetPluginData are placeholders only.
    C++ virtual dispatch uses slot/signature/layout, not those source names.
*/

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtCore/QMap>

class IPluginBase : public QObject
{
public:
    explicit IPluginBase(QObject *parent = 0)
        : QObject(parent)
    {
    }

    virtual ~IPluginBase()
    {
    }

    // vtable +0x2c
    virtual QString GetName() = 0;

    // vtable +0x30
    // Original method name has not yet been recovered. Host passes a
    // QMap<QString,QVariant> / QVariantMap by value when restoring plugin state.
    virtual void SetPluginData(QVariantMap data)
    {
        Q_UNUSED(data);
    }

    // vtable +0x34
    // Original method name has not yet been recovered. Host serializes the
    // returned map as a QVariant.
    virtual QVariantMap GetPluginData()
    {
        return QVariantMap();
    }

    // vtable +0x38
    virtual QVariant GetValue(QString name, const QVariantList &params)
    {
        Q_UNUSED(name);
        Q_UNUSED(params);
        return QVariant();
    }
};

class IFuncProvider : public IPluginBase
{
public:
    explicit IFuncProvider(QObject *parent = 0)
        : IPluginBase(parent)
    {
    }

    virtual ~IFuncProvider()
    {
    }

    // vtable +0x3c
    virtual const QStringList &GetList() = 0;
};

#define IFuncProvider_iid "org.roccat.powergrid.IFuncProvider"
Q_DECLARE_INTERFACE(IFuncProvider, IFuncProvider_iid)

#endif // ROCCATPOWERGRID_RECONSTRUCTED_H
