/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtMultimedia module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights.  These rights are described in the Nokia Qt LGPL
** Exception version 1.1, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/


#ifndef QAUDIODEVICEINFO_H
#define QAUDIODEVICEINFO_H

#include <QtCore/qobject.h>
#include <QtCore/qglobal.h>
#include <QtCore/qbytearray.h>
#include <QtCore/qstring.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qlist.h>

#include <QtMultimedia/qaudio.h>
#include <QtMultimedia/qaudioformat.h>
#include <QtMultimedia/qaudiodeviceid.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Multimedia)


class QAbstractAudioDeviceInfo;

class Q_MULTIMEDIA_EXPORT QAudioDeviceInfo : public QObject
{
    Q_OBJECT

public:
    explicit QAudioDeviceInfo(const QAudioDeviceId &id, QObject *parent = 0);
    ~QAudioDeviceInfo();

    QString deviceName() const;

    bool isFormatSupported(const QAudioFormat &format) const;
    QAudioFormat preferredFormat() const;
    QAudioFormat nearestFormat(const QAudioFormat &format) const;

    QStringList supportedCodecs() const;
    QList<int> supportedFrequencies() const;
    QList<int> supportedChannels() const;
    QList<int> supportedSampleSizes() const;
    QList<QAudioFormat::Endian> supportedByteOrders() const;
    QList<QAudioFormat::SampleType> supportedSampleTypes() const;

    static QAudioDeviceId defaultInputDevice();
    static QAudioDeviceId defaultOutputDevice();

    static QList<QAudioDeviceId> deviceList(QAudio::Mode mode);

private:
    Q_DISABLE_COPY(QAudioDeviceInfo)

    QAbstractAudioDeviceInfo* d;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QAUDIODEVICEINFO_H
