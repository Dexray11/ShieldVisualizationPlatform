/****************************************************************************
** Meta object code from reading C++ file 'mapwidget.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/ui/mapwidget.h"
#include <QtNetwork/QSslError>
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mapwidget.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSMapWidgetENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASSMapWidgetENDCLASS = QtMocHelpers::stringData(
    "MapWidget",
    "projectMarkerClicked",
    "",
    "projectName",
    "brief",
    "unit",
    "screenPos",
    "projectMarkerDoubleClicked",
    "mapAreaClicked",
    "onTileDownloaded",
    "onClickTimer"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSMapWidgetENDCLASS_t {
    uint offsetsAndSizes[22];
    char stringdata0[10];
    char stringdata1[21];
    char stringdata2[1];
    char stringdata3[12];
    char stringdata4[6];
    char stringdata5[5];
    char stringdata6[10];
    char stringdata7[27];
    char stringdata8[15];
    char stringdata9[17];
    char stringdata10[13];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSMapWidgetENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSMapWidgetENDCLASS_t qt_meta_stringdata_CLASSMapWidgetENDCLASS = {
    {
        QT_MOC_LITERAL(0, 9),  // "MapWidget"
        QT_MOC_LITERAL(10, 20),  // "projectMarkerClicked"
        QT_MOC_LITERAL(31, 0),  // ""
        QT_MOC_LITERAL(32, 11),  // "projectName"
        QT_MOC_LITERAL(44, 5),  // "brief"
        QT_MOC_LITERAL(50, 4),  // "unit"
        QT_MOC_LITERAL(55, 9),  // "screenPos"
        QT_MOC_LITERAL(65, 26),  // "projectMarkerDoubleClicked"
        QT_MOC_LITERAL(92, 14),  // "mapAreaClicked"
        QT_MOC_LITERAL(107, 16),  // "onTileDownloaded"
        QT_MOC_LITERAL(124, 12)   // "onClickTimer"
    },
    "MapWidget",
    "projectMarkerClicked",
    "",
    "projectName",
    "brief",
    "unit",
    "screenPos",
    "projectMarkerDoubleClicked",
    "mapAreaClicked",
    "onTileDownloaded",
    "onClickTimer"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSMapWidgetENDCLASS[] = {

 // content:
      11,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    4,   44,    2, 0x06,    1 /* Public */,
       7,    1,   53,    2, 0x06,    6 /* Public */,
       8,    0,   56,    2, 0x06,    8 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       9,    0,   57,    2, 0x08,    9 /* Private */,
      10,    0,   58,    2, 0x08,   10 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString, QMetaType::QPoint,    3,    4,    5,    6,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject MapWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_CLASSMapWidgetENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSMapWidgetENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSMapWidgetENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<MapWidget, std::true_type>,
        // method 'projectMarkerClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QPoint &, std::false_type>,
        // method 'projectMarkerDoubleClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'mapAreaClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onTileDownloaded'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onClickTimer'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void MapWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MapWidget *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->projectMarkerClicked((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<QPoint>>(_a[4]))); break;
        case 1: _t->projectMarkerDoubleClicked((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->mapAreaClicked(); break;
        case 3: _t->onTileDownloaded(); break;
        case 4: _t->onClickTimer(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (MapWidget::*)(const QString & , const QString & , const QString & , const QPoint & );
            if (_t _q_method = &MapWidget::projectMarkerClicked; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (MapWidget::*)(const QString & );
            if (_t _q_method = &MapWidget::projectMarkerDoubleClicked; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (MapWidget::*)();
            if (_t _q_method = &MapWidget::mapAreaClicked; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject *MapWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MapWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSMapWidgetENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int MapWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void MapWidget::projectMarkerClicked(const QString & _t1, const QString & _t2, const QString & _t3, const QPoint & _t4)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t4))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void MapWidget::projectMarkerDoubleClicked(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void MapWidget::mapAreaClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
QT_WARNING_POP
