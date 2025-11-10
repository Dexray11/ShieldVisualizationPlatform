/****************************************************************************
** Meta object code from reading C++ file 'dashboardwindow.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/ui/dashboardwindow.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'dashboardwindow.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSDashboardWindowENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASSDashboardWindowENDCLASS = QtMocHelpers::stringData(
    "DashboardWindow",
    "onProjectSelected",
    "",
    "row",
    "column",
    "onProjectMarkerClicked",
    "projectName",
    "onWorkbenchClicked",
    "openProjectView",
    "onProjectListItemClicked",
    "QListWidgetItem*",
    "item",
    "onProjectListItemDoubleClicked"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSDashboardWindowENDCLASS_t {
    uint offsetsAndSizes[26];
    char stringdata0[16];
    char stringdata1[18];
    char stringdata2[1];
    char stringdata3[4];
    char stringdata4[7];
    char stringdata5[23];
    char stringdata6[12];
    char stringdata7[19];
    char stringdata8[16];
    char stringdata9[25];
    char stringdata10[17];
    char stringdata11[5];
    char stringdata12[31];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSDashboardWindowENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSDashboardWindowENDCLASS_t qt_meta_stringdata_CLASSDashboardWindowENDCLASS = {
    {
        QT_MOC_LITERAL(0, 15),  // "DashboardWindow"
        QT_MOC_LITERAL(16, 17),  // "onProjectSelected"
        QT_MOC_LITERAL(34, 0),  // ""
        QT_MOC_LITERAL(35, 3),  // "row"
        QT_MOC_LITERAL(39, 6),  // "column"
        QT_MOC_LITERAL(46, 22),  // "onProjectMarkerClicked"
        QT_MOC_LITERAL(69, 11),  // "projectName"
        QT_MOC_LITERAL(81, 18),  // "onWorkbenchClicked"
        QT_MOC_LITERAL(100, 15),  // "openProjectView"
        QT_MOC_LITERAL(116, 24),  // "onProjectListItemClicked"
        QT_MOC_LITERAL(141, 16),  // "QListWidgetItem*"
        QT_MOC_LITERAL(158, 4),  // "item"
        QT_MOC_LITERAL(163, 30)   // "onProjectListItemDoubleClicked"
    },
    "DashboardWindow",
    "onProjectSelected",
    "",
    "row",
    "column",
    "onProjectMarkerClicked",
    "projectName",
    "onWorkbenchClicked",
    "openProjectView",
    "onProjectListItemClicked",
    "QListWidgetItem*",
    "item",
    "onProjectListItemDoubleClicked"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSDashboardWindowENDCLASS[] = {

 // content:
      11,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    2,   50,    2, 0x08,    1 /* Private */,
       5,    1,   55,    2, 0x08,    4 /* Private */,
       7,    0,   58,    2, 0x08,    6 /* Private */,
       8,    0,   59,    2, 0x08,    7 /* Private */,
       9,    1,   60,    2, 0x08,    8 /* Private */,
      12,    1,   63,    2, 0x08,   10 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    3,    4,
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 10,   11,
    QMetaType::Void, 0x80000000 | 10,   11,

       0        // eod
};

Q_CONSTINIT const QMetaObject DashboardWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_CLASSDashboardWindowENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSDashboardWindowENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSDashboardWindowENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<DashboardWindow, std::true_type>,
        // method 'onProjectSelected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'onProjectMarkerClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onWorkbenchClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'openProjectView'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onProjectListItemClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QListWidgetItem *, std::false_type>,
        // method 'onProjectListItemDoubleClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QListWidgetItem *, std::false_type>
    >,
    nullptr
} };

void DashboardWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<DashboardWindow *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onProjectSelected((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 1: _t->onProjectMarkerClicked((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->onWorkbenchClicked(); break;
        case 3: _t->openProjectView(); break;
        case 4: _t->onProjectListItemClicked((*reinterpret_cast< std::add_pointer_t<QListWidgetItem*>>(_a[1]))); break;
        case 5: _t->onProjectListItemDoubleClicked((*reinterpret_cast< std::add_pointer_t<QListWidgetItem*>>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject *DashboardWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DashboardWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSDashboardWindowENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int DashboardWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
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
QT_WARNING_POP
