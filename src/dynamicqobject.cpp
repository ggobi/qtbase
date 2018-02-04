/****************************************************************************
**
** Copyright (C) 2006 Trolltech AS. All rights reserved.
**
** This file is part of the documentation of Qt. It was originally
** published as part of Qt Quarterly.
**
** This file may be used under the terms of the GNU General Public License
** version 2.0 as published by the Free Software Foundation or under the
** terms of the Qt Commercial License Agreement. The respective license
** texts for these are provided with the open source and commercial
** editions of Qt.
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "dynamicqobject.h"

bool DynamicQObject::connectDynamicSlot(QObject *obj, const char *signal,
                                        const char *slot)
{
    QByteArray theSignal = QMetaObject::normalizedSignature(signal);
    QByteArray theSlot = QMetaObject::normalizedSignature(slot);
    if (!QMetaObject::checkConnectArgs(theSignal, theSlot))
        return false;

    int signalId = obj->metaObject()->indexOfSignal(theSignal);
    if (signalId < 0) 
        return false;

    int slotId = slotIndices.value(theSlot, -1);
    if (slotId < 0) {
        slotId = slotList.size();
        slotIndices[theSlot] = slotId;
        slotList.append(createSlot(theSlot.data()));
    }

    return QMetaObject::connect(obj, signalId, this, slotId + metaObject()->methodCount());
}

bool DynamicQObject::connectDynamicSignal(const char *signal, QObject *obj,
                                          const char *slot)
{
    QByteArray theSignal = QMetaObject::normalizedSignature(signal);
    QByteArray theSlot = QMetaObject::normalizedSignature(slot);
    if (!QMetaObject::checkConnectArgs(theSignal, theSlot))
        return false;

    int slotId = obj->metaObject()->indexOfSlot(theSlot);
    if (slotId < 0) 
        return false;    

    int signalId = signalIndices.value(theSignal, -1);
    if (signalId < 0) {
        signalId = signalIndices.size();
        signalIndices[theSignal] = signalId;
    }

    return QMetaObject::connect(this, signalId + metaObject()->methodCount(), obj, slotId);
}


int DynamicQObject::qt_metacall(QMetaObject::Call c, int id, void **arguments)
{
    id = QObject::qt_metacall(c, id, arguments);
    if (id < 0 || c != QMetaObject::InvokeMetaMethod) 
        return id;
    Q_ASSERT(id < slotList.size());
    
    slotList[id]->call(sender(), arguments);
    return -1;
}

bool DynamicQObject::emitDynamicSignal(const char *signal, void **arguments)
{
    QByteArray theSignal = QMetaObject::normalizedSignature(signal);
    int signalId = signalIndices.value(theSignal, -1);
    if (signalId >= 0) {
        QMetaObject::activate(this, metaObject(), signalId + metaObject()->methodCount(), 
            arguments);
        return true;
    } else {
        return false;
    }
}

DynamicQObject::~DynamicQObject() {
    foreach (DynamicSlot *slot, slotList) {
	delete slot;
    }
}
