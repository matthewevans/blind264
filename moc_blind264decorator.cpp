/****************************************************************************
** Meta object code from reading C++ file 'blind264decorator.h'
**
** Created: Sat Jan 28 16:30:43 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "blind264decorator.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'blind264decorator.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Blind264Decorator[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      32,   18,   19,   18, 0x0a,
      54,   50,   18,   18, 0x0a,
      94,   50,   87,   18, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Blind264Decorator[] = {
    "Blind264Decorator\0\0InputDialog*\0"
    "new_InputDialog()\0obj\0"
    "delete_InputDialog(InputDialog*)\0"
    "double\0getNumber(InputDialog*)\0"
};

const QMetaObject Blind264Decorator::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Blind264Decorator,
      qt_meta_data_Blind264Decorator, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Blind264Decorator::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Blind264Decorator::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Blind264Decorator::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Blind264Decorator))
        return static_cast<void*>(const_cast< Blind264Decorator*>(this));
    return QObject::qt_metacast(_clname);
}

int Blind264Decorator::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: { InputDialog* _r = new_InputDialog();
            if (_a[0]) *reinterpret_cast< InputDialog**>(_a[0]) = _r; }  break;
        case 1: delete_InputDialog((*reinterpret_cast< InputDialog*(*)>(_a[1]))); break;
        case 2: { double _r = getNumber((*reinterpret_cast< InputDialog*(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< double*>(_a[0]) = _r; }  break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
