/****************************************************************************
**
** Copyright (C) 2020 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtAndroidExtras module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include "activityhandler.h"

#include <QAndroidIntent>
#include "global_define.h"

const jint RESULT_OK = QAndroidJniObject::getStaticField<jint>("android/app/Activity", "RESULT_OK");
ActivityHandler *ActivityHandler::m_instance = nullptr;

ActivityHandler::ActivityHandler(QObject *parent) : QObject(parent)
{
    m_instance = this;
#if defined(MODIFY_RECEIVER_LOCAL)

#else
    m_resultReceiver = new ResultReceiver();
#endif
}

void ActivityHandler::showSecondActivity()
{
    QAndroidIntent activityIntent(QtAndroid::androidActivity().object(),
                                  "org/qtproject/example/activityhandler/CustomActivity");
#if defined(MODIFY_RECEIVER_LOCAL)
    //this is
    QtAndroid::startActivity(
            activityIntent.handle(), REQUEST_CODE,this);
#elif defined(MODIFY_RECEIVER_EXTERNAL)
    //OK
    QtAndroid::startActivity(
            activityIntent.handle(), REQUEST_CODE,m_resultReceiver);
#else
    QtAndroid::startActivity(
            activityIntent.handle(), REQUEST_CODE,
            [this](int requestCode, int resultCode, const QAndroidJniObject &data) {
                activityReceiver(requestCode, resultCode, data);
            });
#endif
}

#if 1

void ActivityHandler::handleActivityResult(
        int receiverRequestCode,
        int resultCode,
        const QAndroidJniObject & data)
{
    qDebug() << "handleActivityResult, requestCode - " << receiverRequestCode
             << " resultCode - " << resultCode;

    const jint RESULT_OK = QAndroidJniObject::getStaticField<jint>("android/app/Activity", "RESULT_OK");

    if (receiverRequestCode == REQUEST_CODE) {
        if (resultCode == RESULT_OK) {
            const QAndroidJniObject key = QAndroidJniObject::fromString("message");
            const QAndroidJniObject key2 = QAndroidJniObject::fromString("message2");
            const QAndroidJniObject message = data.callObjectMethod(
                    "getStringExtra", "(Ljava/lang/String;)Ljava/lang/String;", key.object());
            const QAndroidJniObject message2 = data.callObjectMethod(
                    "getStringExtra", "(Ljava/lang/String;)Ljava/lang/String;", key2.object());
            if (message.isValid())
                qDebug() << "message:" << message.toString();
                qDebug() << "message 2:" << message2.toString();
        } else {
            qDebug() << "Rejected!";
        }
    }
}

#else

void ActivityHandler::activityReceiver(int requestCode, int resultCode, const QAndroidJniObject &data)
{
    if (requestCode == REQUEST_CODE) {
        if (resultCode == RESULT_OK) {
            const QAndroidJniObject key = QAndroidJniObject::fromString("message");
            const QAndroidJniObject message = data.callObjectMethod(
                    "getStringExtra", "(Ljava/lang/String;)Ljava/lang/String;", key.object());
            if (message.isValid())
                emit ActivityHandler::instance()->receiveFromActivityResult(message.toString());
        } else {
            emit ActivityHandler::instance()->receiveFromActivityResult("Rejected!");
        }
    }
}

#endif
