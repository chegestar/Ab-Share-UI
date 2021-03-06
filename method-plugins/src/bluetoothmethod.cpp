 
/*
 * This file is part of Handset UX Share user interface
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.
 * Contact: Jukka Tiihonen <jukka.tiihonen@nokia.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to 
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense,     
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER  
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS  
 * IN THE SOFTWARE. 
 */




#include "bluetoothmethod.h"
#include <QDebug>
#include <QDBusInterface>
#include <QDBusMessage>
#include <conbtdialogs-dbus.h>
#include <ShareUI/ItemContainer>
#include <ShareUI/FileItem>
#include <ShareUI/DataUriItem>
#include <QUrl>
#include "methodscommon.h"

using namespace ShareUiDefaultMethods;

BluetoothMethod::BluetoothMethod (QObject * parent) :
    ShareUI::MethodBase (parent) {
}

BluetoothMethod::~BluetoothMethod () {
}

QString BluetoothMethod::title() {
    //% "Bluetooth"
    QString name = qtTrId ("qtn_tui_share_via_bt");
    return name;
}

QString BluetoothMethod::icon () {
    return QString::fromLatin1 ("icon-m-content-bluetooth");
}

void BluetoothMethod::selected (const ShareUI::ItemContainer * items) {

    QDBusConnection systemBus = QDBusConnection::systemBus ();
    QDBusInterface dbusIf (CONBTDIALOGS_DBUS_SERVICE, CONBTDIALOGS_DBUS_PATH,
        CONBTDIALOGS_DBUS_INTERFACE, systemBus);
        
    QStringList uris;
    
    ShareUI::ItemIterator itemsIter = items->itemIterator();
    while (itemsIter.hasNext()) {
    
        ShareUI::SharedItem item = itemsIter.next();
        ShareUI::FileItem * fileItem = qobject_cast<ShareUI::FileItem*> (
            item.data());
        
        if (fileItem != 0) {
            uris << fileItem->fileUri ();
        } else {
            ShareUI::DataUriItem * uriItem =
                qobject_cast<ShareUI::DataUriItem*> (item.data());
                
            if (uriItem != 0 && uriItem->mimeType() == "text/x-vcard") {
                const MDataUri & dUri = uriItem->dataUri();
            
                QString path = makeFilePathForVCard (dUri);
                if (dUri.writeDataToFile (path) == true) {
                    //NOTE: do NOT use encoded, bluetooth api doesn't handle
                    //uris correctly and for example %20 isn't understood.
                    QString encoded = QUrl::fromLocalFile (path).toString();
                    qDebug() << "Serialized vcard to:" << encoded;
                    uris << encoded;
                } else {
                    qCritical() << "Failed to serialize vcard to:" << path;
                }        
            
            }
        }
    }
    
    QDBusMessage res = dbusIf.call (CONBTDIALOGS_SEND_FILE_REQ, uris);
    
    if (res.type() == QDBusMessage::ErrorMessage) {
        qCritical() << "Failed to call bluetooth service";
        emit (selectedFailed (
            QLatin1String("Failed to call bluetooth service")));
    } else {
        // Emit back to SUI that we are done.
        emit (done());        
    }
}

void BluetoothMethod::currentItems (const ShareUI::ItemContainer * items) {
    Q_EMIT (visible (acceptContent (items)));
}

QString BluetoothMethod::id () {
    return QString::fromLatin1 ("com.meego.bluetooth");
}

bool BluetoothMethod::acceptContent (const ShareUI::ItemContainer * items) {
    // Safety check
    if (items == 0) {
        return false;
    }
    
    if (items->count() == 0) {
        qDebug() << "Bluetooth not shown if empty container";
        return false;
    }

    ShareUI::ItemIterator itemsIter = items->itemIterator();

    // Limit to data uris and files
    while (itemsIter.hasNext()) {
    
        ShareUI::SharedItem item = itemsIter.next();
        ShareUI::FileItem * fileItem = qobject_cast<ShareUI::FileItem*> (
            item.data());
            
        if (fileItem == 0) {
            
            ShareUI::DataUriItem * uriItem =
                qobject_cast<ShareUI::DataUriItem*> (item.data());
                
            if ((uriItem != 0 &&
                uriItem->mimeType() == "text/x-vcard") == false) {
                
                qDebug() << "Bluetooth doesn't accept content";
                return false;
            }
        }
    }
    
    return true;
}