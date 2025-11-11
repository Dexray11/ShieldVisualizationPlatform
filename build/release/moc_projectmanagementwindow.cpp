/****************************************************************************
** Meta object code from reading C++ file 'projectmanagementwindow.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/ui/projectmanagementwindow.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'projectmanagementwindow.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSProjectManagementWindowENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASSProjectManagementWindowENDCLASS = QtMocHelpers::stringData(
    "ProjectManagementWindow",
    "onBackClicked",
    "",
    "onNewProjectClicked",
    "onEditProject",
    "row",
    "onDeleteProject",
    "onTabChanged",
    "index"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSProjectManagementWindowENDCLASS_t {
    uint offsetsAndSizes[18];
    char stringdata0[24];
    char stringdata1[14];
    char stringdata2[1];
    char stringdata3[20];
    char stringdata4[14];
    char stringdata5[4];
    char stringdata6[16];
    char stringdata7[13];
    char stringdata8[6];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSProjectManagementWindowENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSProjectManagementWindowENDCLASS_t qt_meta_stringdata_CLASSProjectManagementWindowENDCLASS = {
    {
        QT_MOC_LITERAL(0, 23),  // "ProjectManagementWindow"
        QT_MOC_LITERAL(24, 13),  // "onBackClicked"
        QT_MOC_LITERAL(38, 0),  // ""
        QT_MOC_LITERAL(39, 19),  // "onNewProjectClicked"
        QT_MOC_LITERAL(59, 13),  // "onEditProject"
        QT_MOC_LITERAL(73, 3),  // "row"
        QT_MOC_LITERAL(77, 15),  // "onDeleteProject"
        QT_MOC_LITERAL(93, 12),  // "onTabChanged"
        QT_MOC_LITERAL(106, 5)   // "index"
    },
    "ProjectManagementWindow",
    "onBackClicked",
    "",
    "onNewProjectClicked",
    "onEditProject",
    "row",
    "onDeleteProject",
    "onTabChanged",
    "index"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSProjectManagementWindowENDCLASS[] = {

 // content:
      11,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   44,    2, 0x08,    1 /* Private */,
       3,    0,   45,    2, 0x08,    2 /* Private */,
       4,    1,   46,    2, 0x08,    3 /* Private */,
       6,    1,   49,    2, 0x08,    5 /* Private */,
       7,    1,   52,    2, 0x08,    7 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void, QMetaType::Int,    8,

       0        // eod
};

Q_CONSTINIT const QMetaObject ProjectManagementWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_CLASSProjectManagementWindowENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSProjectManagementWindowENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSProjectManagementWindowENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<ProjectManagementWindow, std::true_type>,
        // method 'onBackClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onNewProjectClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onEditProject'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'onDeleteProject'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'onTabChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>
    >,
    nullptr
} };

void ProjectManagementWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ProjectManagementWindow *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onBackClicked(); break;
        case 1: _t->onNewProjectClicked(); break;
        case 2: _t->onEditProject((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 3: _t->onDeleteProject((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 4: _t->onTabChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject *ProjectManagementWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ProjectManagementWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSProjectManagementWindowENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int ProjectManagementWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
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
QT_WARNING_POP
