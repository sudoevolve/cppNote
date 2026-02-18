/*
 * Copyright (c) 2026 sudoevolve
 * Copyright (c) 2026 qtcraft
 * SPDX-License-Identifier: MIT
 */

import QtQuick 2.15
import QtQml 2.15

Item {
    id: root

    property string text: ""
    property color color: "#000000"
    property font font
    property real lineSpacing: 6
    property int horizontalAlignment: Text.AlignLeft

    property int duration: 420
    property int stagger: 40
    property real rise: 18
    property bool autoStart: true
    property bool loop: false
    property int loopDelay: 600

    readonly property bool running: _running

    signal finished()

    function start() {
        if (_lineCount === 0) {
            _finish()
            return
        }
        _running = true
        playId = playId + 1
    }

    function stop() {
        _running = false
        playId = playId + 1
        loopTimer.stop()
    }

    function restart() {
        stop()
        start()
    }

    clip: width > 0 && height > 0

    implicitWidth: col.implicitWidth
    implicitHeight: col.implicitHeight

    property var _lines: []
    property int _lineCount: _lines.length
    property var _charLines: []
    property var _lineStartIndex: []
    property int _totalChars: 0
    property int playId: 0
    property bool _running: false
    property bool _didFinishForPlay: false

    function _rebuild() {
        var raw = root.text === undefined || root.text === null ? "" : String(root.text)
        raw = raw.replace(/\r\n/g, "\n").replace(/\r/g, "\n")
        root._lines = raw.length === 0 ? [] : raw.split("\n")

        var cl = []
        var starts = []
        var total = 0
        for (var i = 0; i < root._lines.length; i = i + 1) {
            starts.push(total)
            var line = root._lines[i]
            var arr = line.length === 0 ? [] : line.split("")
            cl.push(arr)
            total = total + arr.length
        }
        root._charLines = cl
        root._lineStartIndex = starts
        root._totalChars = total
    }

    function _finish() {
        if (root._didFinishForPlay) {
            return
        }
        root._didFinishForPlay = true
        root._running = false
        root.finished()

        if (root.loop && root._totalChars > 0) {
            loopTimer.restart()
        }
    }

    onTextChanged: {
        _rebuild()
        _didFinishForPlay = false
        if (autoStart) {
            Qt.callLater(restart)
        }
    }

    Component.onCompleted: {
        _rebuild()
        if (autoStart) {
            start()
        }
    }

    onPlayIdChanged: {
        _didFinishForPlay = false
    }

    Timer {
        id: loopTimer
        interval: root.loopDelay
        repeat: false
        running: false
        onTriggered: {
            if (root.loop) {
                root.start()
            }
        }
    }

    Column {
        id: col
        anchors.fill: parent
        spacing: root.lineSpacing

        Repeater {
            id: rep
            model: root._lines

            Item {
                id: lineBox
                width: col.width > 0 ? col.width : implicitWidth
                implicitWidth: row.implicitWidth
                implicitHeight: row.implicitHeight
                height: implicitHeight

                property int lineIndex: index

                Row {
                    id: row
                    spacing: 0
                    anchors.left: parent.left
                    anchors.right: parent.right
                    layoutDirection: Qt.LeftToRight

                    property int rowPlayId: root.playId

                    Repeater {
                        id: charRep
                        model: (lineBox.lineIndex >= 0 && lineBox.lineIndex < root._charLines.length) ? root._charLines[lineBox.lineIndex] : []

                        Item {
                            id: chBox
                            implicitWidth: chText.implicitWidth
                            implicitHeight: chText.implicitHeight
                            width: implicitWidth
                            height: implicitHeight

                            property int _localPlayId: row.rowPlayId
                            on_LocalPlayIdChanged: {
                                appear.restart()
                            }

                            Component.onCompleted: {
                                if (root._running) {
                                    appear.restart()
                                }
                            }

                            Text {
                                id: chText
                                text: modelData
                                color: root.color
                                font: root.font
                                horizontalAlignment: root.horizontalAlignment
                                verticalAlignment: Text.AlignVCenter
                                opacity: 0
                                y: root.rise
                            }

                            SequentialAnimation {
                                id: appear
                                running: false

                                ScriptAction {
                                    script: {
                                        chText.opacity = 0
                                        chText.y = root.rise
                                    }
                                }

                                PauseAnimation {
                                    duration: root._running ? ((((root._lineStartIndex[lineBox.lineIndex] !== undefined) ? root._lineStartIndex[lineBox.lineIndex] : 0) + index) * root.stagger) : 0
                                }

                                ParallelAnimation {
                                    NumberAnimation {
                                        target: chText
                                        property: "y"
                                        to: root._running ? 0 : root.rise
                                        duration: root._running ? root.duration : 0
                                        easing.type: Easing.OutCubic
                                    }
                                    NumberAnimation {
                                        target: chText
                                        property: "opacity"
                                        to: root._running ? 1 : 0
                                        duration: root._running ? Math.max(1, Math.floor(root.duration * 0.9)) : 0
                                        easing.type: Easing.OutCubic
                                    }
                                }

                                ScriptAction {
                                    script: {
                                        if (!root._running) {
                                            return
                                        }
                                        var base = (root._lineStartIndex[lineBox.lineIndex] !== undefined) ? root._lineStartIndex[lineBox.lineIndex] : 0
                                        var g = base + index
                                        if (g === root._totalChars - 1) {
                                            root._finish()
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
