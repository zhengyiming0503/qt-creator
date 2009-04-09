/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact:  Qt Software Information (qt-info@nokia.com)
**
** Commercial Usage
**
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
**
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
**
**************************************************************************/

#include "fakevimhandler.h"

#include <QtCore/QSet>

#include <QtGui/QTextEdit>
#include <QtGui/QPlainTextEdit>

#include <QtTest/QtTest>

using namespace FakeVim;
using namespace FakeVim::Internal;


class tst_FakeVim : public QObject
{
    Q_OBJECT

public:
    tst_FakeVim();

public slots:
    void changeStatusData(const QString &info) { m_statusData = info; }
    void changeStatusMessage(const QString &info) { m_statusMessage = info; }
    void changeExtraInformation(const QString &info) { m_infoMessage = info; }
    
private slots:
    void commandI();
    void commandDollar();
    void commandDown();
    void commandUp();

private:
    void setup();    
    void send(const QString &command); // send a normal command
    void sendEx(const QString &command); // send an ex command

    bool checkContentsHelper(QString expected, const char* file, int line);
    bool checkHelper(bool isExCommand, QString cmd, QString expected,
        const char* file, int line);
    QString insertCursor(const QString &needle0);

    //QTextEdit m_editor;
    QPlainTextEdit m_editor;
    FakeVimHandler m_handler;
    QList<QTextEdit::ExtraSelection> m_selection;

    QString m_statusMessage;
    QString m_statusData;
    QString m_infoMessage;

    static const QString lines;
    static const QString escape;
};

const QString tst_FakeVim::lines = 
    "\n"
    "#include <QtCore>\n"
    "#include <QtGui>\n"
    "\n"
    "int main(int argc, char *argv[])\n"
    "{\n"
    "    QApplication app(argc, argv);\n"
    "\n"
    "    return app.exec();\n"
    "}\n";

const QString tst_FakeVim::escape = QChar(27);

tst_FakeVim::tst_FakeVim()
    : m_handler(&m_editor, this)
{
    QObject::connect(&m_handler, SIGNAL(commandBufferChanged(QString)),
        this, SLOT(changeStatusMessage(QString)));
    QObject::connect(&m_handler, SIGNAL(extraInformationChanged(QString)),
        this, SLOT(changeExtraInformation(QString)));
    QObject::connect(&m_handler, SIGNAL(statusDataChanged(QString)),
        this, SLOT(changeStatusData(QString)));
}

void tst_FakeVim::setup()
{
    m_statusMessage.clear();
    m_statusData.clear();
    m_infoMessage.clear();
    m_editor.setPlainText(lines);
    QTextCursor tc = m_editor.textCursor();
    tc.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
    m_editor.setTextCursor(tc);
    m_editor.setPlainText(lines);
    //m_editor.show();
    //qApp->exec();
    QCOMPARE(m_editor.toPlainText(), lines);
}

void tst_FakeVim::send(const QString &command)
{
    m_handler.handleCommand("normal " + command);
}

void tst_FakeVim::sendEx(const QString &command)
{
    m_handler.handleCommand(command);
}

bool tst_FakeVim::checkContentsHelper(QString want, const char* file, int line)
{
    QString got = m_editor.toPlainText();
    int pos = m_editor.textCursor().position();
    got = got.left(pos) + "@" + got.mid(pos);
    QStringList wantlist = want.split('\n');
    QStringList gotlist = got.split('\n');
    if (!QTest::qCompare(gotlist.size(), wantlist.size(), "", "", file, line)) {
        qDebug() << "WANT: " << want;
        qDebug() << "GOT: " << got;
        return false;
    }
    for (int i = 0; i < wantlist.size() && i < gotlist.size(); ++i) {
        QString g = QString("line %1: %2").arg(i + 1).arg(gotlist.at(i));
        QString w = QString("line %1: %2").arg(i + 1).arg(wantlist.at(i));
        if (!QTest::qCompare(g, w, "", "", file, line)) {
            qDebug() << "WANT: " << want;
            qDebug() << "GOT: " << got;
            return false;
        }
    }
    return true;
}

bool tst_FakeVim::checkHelper(bool ex, QString cmd, QString expected,
    const char *file, int line)
{
    if (ex)
        sendEx(cmd);
    else
        send(cmd);
    return checkContentsHelper(expected, file, line);
}


#define checkContents(expected) \
    do { if (!checkContentsHelper(expected, __FILE__, __LINE__)) return; } while (0)

// Runs a "normal" command and checks the result.
// Cursor position is marked by a '@' in the expected contents.
#define check(cmd, expected) \
    do { if (!checkHelper(false, cmd, expected, __FILE__, __LINE__)) \
            return; } while (0)

// Runs an ex command and checks the result.
// Cursor position is marked by a '@' in the expected contents.
#define checkEx(cmd, expected) \
    do { if (!checkHelper(true, cmd, expected, __FILE__, __LINE__)) \
            return; } while (0)

QString tst_FakeVim::insertCursor(const QString &needle0)
{
    QString needle = needle0;
    needle.remove('@');
    QString lines0 = lines;
    lines0.replace(needle, needle0);
    //qDebug() << "LINES: " << lines0;
    return lines0;
}

void tst_FakeVim::commandI()
{
    return;
    setup();

    // empty insertion at start of document
    check("i" + escape, "@" + lines);
    check("u", "@" + lines);

    // small insertion at start of document
    check("ix" + escape, "@x" + lines);
    check("u", "@" + lines);

    // small insertion at start of document
    check("ixxx" + escape, "xx@x" + lines);
    check("u", "@" + lines);

    // combine insertions
    check("ia" + escape, "@a" + lines);
    check("ibx" + escape, "b@xa" + lines);
    check("icyy" + escape, "bcy@yxa" + lines);
    check("u", "b@xa" + lines);
    check("u", "@a" + lines);   // undo broken
    checkEx("redo", "b@xa" + lines);
    check("u", "@a" + lines);
    check("u", "@" + lines);
}

void tst_FakeVim::commandDollar()
{
    setup();
    check("j$", insertCursor("<QtCore>@"));
    //check("j", insertCursor("<QtGui>@"));
}

void tst_FakeVim::commandDown()
{
    setup();
    check("j", insertCursor("@#include <QtCore"));
    check("3j", insertCursor("@int main"));
    check("4j", insertCursor("@    return app.exec()"));
}

void tst_FakeVim::commandUp()
{
    setup();
    check("j", insertCursor("@#include <QtCore"));
    check("3j", insertCursor("@int main"));
    check("4j", insertCursor("@    return app.exec()"));
}



QTEST_MAIN(tst_FakeVim)

#include "main.moc"
