/****************************************************************************
** Meta object code from reading C++ file 'GeoDataImporter.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/utils/GeoDataImporter.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'GeoDataImporter.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSGeoDataImporterENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASSGeoDataImporterENDCLASS = QtMocHelpers::stringData(
    "GeoDataImporter",
    "importCompleted",
    "",
    "importCancelled",
    "onSelectBoreholeFile",
    "onSelectTunnelProfileFile",
    "onStartImport",
    "onCancel"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSGeoDataImporterENDCLASS_t {
    uint offsetsAndSizes[16];
    char stringdata0[16];
    char stringdata1[16];
    char stringdata2[1];
    char stringdata3[16];
    char stringdata4[21];
    char stringdata5[26];
    char stringdata6[14];
    char stringdata7[9];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSGeoDataImporterENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSGeoDataImporterENDCLASS_t qt_meta_stringdata_CLASSGeoDataImporterENDCLASS = {
    {
        QT_MOC_LITERAL(0, 15),  // "GeoDataImporter"
        QT_MOC_LITERAL(16, 15),  // "importCompleted"
        QT_MOC_LITERAL(32, 0),  // ""
        QT_MOC_LITERAL(33, 15),  // "importCancelled"
        QT_MOC_LITERAL(49, 20),  // "onSelectBoreholeFile"
        QT_MOC_LITERAL(70, 25),  // "onSelectTunnelProfileFile"
        QT_MOC_LITERAL(96, 13),  // "onStartImport"
        QT_MOC_LITERAL(110, 8)   // "onCancel"
    },
    "GeoDataImporter",
    "importCompleted",
    "",
    "importCancelled",
    "onSelectBoreholeFile",
    "onSelectTunnelProfileFile",
    "onStartImport",
    "onCancel"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSGeoDataImporterENDCLASS[] = {

 // content:
      11,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   50,    2, 0x06,    1 /* Public */,
       3,    0,   51,    2, 0x06,    2 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       4,    0,   52,    2, 0x08,    3 /* Private */,
       5,    0,   53,    2, 0x08,    4 /* Private */,
       6,    0,   54,    2, 0x08,    5 /* Private */,
       7,    0,   55,    2, 0x08,    6 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject GeoDataImporter::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_CLASSGeoDataImporterENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSGeoDataImporterENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSGeoDataImporterENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<GeoDataImporter, std::true_type>,
        // method 'importCompleted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'importCancelled'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onSelectBoreholeFile'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onSelectTunnelProfileFile'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onStartImport'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onCancel'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void GeoDataImporter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<GeoDataImporter *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->importCompleted(); break;
        case 1: _t->importCancelled(); break;
        case 2: _t->onSelectBoreholeFile(); break;
        case 3: _t->onSelectTunnelProfileFile(); break;
        case 4: _t->onStartImport(); break;
        case 5: _t->onCancel(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (GeoDataImporter::*)();
            if (_t _q_method = &GeoDataImporter::importCompleted; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (GeoDataImporter::*)();
            if (_t _q_method = &GeoDataImporter::importCancelled; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
    }
    (void)_a;
}

const QMetaObject *GeoDataImporter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *GeoDataImporter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSGeoDataImporterENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int GeoDataImporter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void GeoDataImporter::importCompleted()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void GeoDataImporter::importCancelled()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
