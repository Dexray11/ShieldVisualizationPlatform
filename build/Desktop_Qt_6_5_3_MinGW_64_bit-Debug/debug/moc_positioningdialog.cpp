/****************************************************************************
** Meta object code from reading C++ file 'positioningdialog.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/ui/positioningdialog.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'positioningdialog.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSPositioningDialogENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASSPositioningDialogENDCLASS = QtMocHelpers::stringData(
    "PositioningDialog",
    "onPositioningMethodChanged",
    "",
    "onGPSDeviceSelected",
    "index",
    "onConfirmClicked",
    "onCancelClicked",
    "onLocateByCoords",
    "onLocateByStake",
    "onResetPosition"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSPositioningDialogENDCLASS_t {
    uint offsetsAndSizes[20];
    char stringdata0[18];
    char stringdata1[27];
    char stringdata2[1];
    char stringdata3[20];
    char stringdata4[6];
    char stringdata5[17];
    char stringdata6[16];
    char stringdata7[17];
    char stringdata8[16];
    char stringdata9[16];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSPositioningDialogENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSPositioningDialogENDCLASS_t qt_meta_stringdata_CLASSPositioningDialogENDCLASS = {
    {
        QT_MOC_LITERAL(0, 17),  // "PositioningDialog"
        QT_MOC_LITERAL(18, 26),  // "onPositioningMethodChanged"
        QT_MOC_LITERAL(45, 0),  // ""
        QT_MOC_LITERAL(46, 19),  // "onGPSDeviceSelected"
        QT_MOC_LITERAL(66, 5),  // "index"
        QT_MOC_LITERAL(72, 16),  // "onConfirmClicked"
        QT_MOC_LITERAL(89, 15),  // "onCancelClicked"
        QT_MOC_LITERAL(105, 16),  // "onLocateByCoords"
        QT_MOC_LITERAL(122, 15),  // "onLocateByStake"
        QT_MOC_LITERAL(138, 15)   // "onResetPosition"
    },
    "PositioningDialog",
    "onPositioningMethodChanged",
    "",
    "onGPSDeviceSelected",
    "index",
    "onConfirmClicked",
    "onCancelClicked",
    "onLocateByCoords",
    "onLocateByStake",
    "onResetPosition"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSPositioningDialogENDCLASS[] = {

 // content:
      11,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   56,    2, 0x08,    1 /* Private */,
       3,    1,   57,    2, 0x08,    2 /* Private */,
       5,    0,   60,    2, 0x08,    4 /* Private */,
       6,    0,   61,    2, 0x08,    5 /* Private */,
       7,    0,   62,    2, 0x08,    6 /* Private */,
       8,    0,   63,    2, 0x08,    7 /* Private */,
       9,    0,   64,    2, 0x08,    8 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    4,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject PositioningDialog::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_CLASSPositioningDialogENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSPositioningDialogENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSPositioningDialogENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<PositioningDialog, std::true_type>,
        // method 'onPositioningMethodChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onGPSDeviceSelected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'onConfirmClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onCancelClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onLocateByCoords'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onLocateByStake'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onResetPosition'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void PositioningDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<PositioningDialog *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onPositioningMethodChanged(); break;
        case 1: _t->onGPSDeviceSelected((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 2: _t->onConfirmClicked(); break;
        case 3: _t->onCancelClicked(); break;
        case 4: _t->onLocateByCoords(); break;
        case 5: _t->onLocateByStake(); break;
        case 6: _t->onResetPosition(); break;
        default: ;
        }
    }
}

const QMetaObject *PositioningDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PositioningDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSPositioningDialogENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int PositioningDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 7;
    }
    return _id;
}
QT_WARNING_POP
