import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 2.4

Window {
    visible: true
    objectName: "windowObject"
    width: 1080
    height: 720
    title: qsTr("Hello World")

    Button {
        id: dialButton
        objectName: "dialButtonObject"
        signal dial(string address)
        onClicked: dialButton.dial(dialNumberTextEdit.text);
        x: 23
        y: 26
        text: qsTr("Dial")
    }

    TextEdit {
        id: dialNumberTextEdit
        x: 141
        y: 36
        width: 80
        height: 20
        font.pixelSize: 12
        text: qsTr("")
    }

    Button {
        id: answerButton
        objectName: "answerButtonObject"
        signal answer()
        onClicked: answerButton.answer();
        x: 23
        y: 128
        text: qsTr("Answer")
    }

    Button {
        id: rejectButton
        objectName: "rejectButtonObject"
        signal reject()
        onClicked: rejectButton.reject();
        x: 131
        y: 128
        text: qsTr("Reject")
    }

    Button {
        id: endCallButton
        objectName: "endCallButtonObject"
        signal endCall(int callId)
        onClicked: endCallButton.endCall(parseInt(callIdTextEdit.text));
        x: 23
        y: 75
        text: qsTr("End")
    }

    TextEdit {
        id: callIdTextEdit
        x: 141
        y: 85
        width: 80
        height: 20
        font.pixelSize: 12
    }

    Button {
        id: startPresentButton
        x: 248
        y: 26
        text: qsTr("Present")
        onClicked: presentationTimer.running = true
    }

    Image {
        id: snapshotImage
        objectName: "imageObject"
        cache: false;
        x: 23
        y: 191
        width: 325
        height: 513
        source: ""
    }
    Timer {
        id:presentationTimer
        interval: 500;
        running: false;
        repeat: true;
        onTriggered: {
            snapshotImage.source = ""
            snapshotImage.source = "image://snapshotimageprovider/presentation"
        }
    }

    Button {
        id: stopPresentButton
        x: 248
        y: 75
        text: qsTr("Stop Present")
        onClicked: {
            presentationTimer.running = false
            snapshotImage.source = ""
        }
    }

    Button {
        id: fetchSnapshotButton
        objectName: "fetchSnapshotButtonObject"
        signal sendSnapshotId(string id)
        x: 248
        y: 128
        text: qsTr("Fetch")
        onClicked: {
            sendSnapshotId("wangshenxing")
            snapshotImage.source = "image://snapshotimageprovider/fetch"
        }
    }
}
