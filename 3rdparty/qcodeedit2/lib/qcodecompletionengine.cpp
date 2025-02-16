/****************************************************************************
**
** Copyright (C) 2006-2009 fullmetalcoder <fullmetalcoder@hotmail.fr>
**
** This file is part of the Edyuk project <http://edyuk.org>
**
** This file may be used under the terms of the GNU General Public License
** version 3 as published by the Free Software Foundation and appearing in the
** file GPL.txt included in the packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "qcodecompletionengine.h"

/*!
        \file qcompletionengine.cpp
        \brief Implementation of the QCodeCompletionEngine class.
*/

#include "qeditor.h"

#include <QAction>
#include <QKeyEvent>
#include <QTextCursor>

/*!

*/
QCodeCompletionEngine::QCodeCompletionEngine(QObject *p)
    : QObject(p), m_max(0), m_trigWordLen(-1) {
    pForcedTrigger = new QAction(tr("&Trigger completion"), this);

    connect(pForcedTrigger, SIGNAL(triggered()), this, SLOT(complete()));
}

/*!

*/
QCodeCompletionEngine::~QCodeCompletionEngine() {}

/*!
        \return
*/
QAction *QCodeCompletionEngine::triggerAction() const { return pForcedTrigger; }

/*!

*/
void QCodeCompletionEngine::retranslate() {
    pForcedTrigger->setText(tr("&Trigger completion"));
}

/*!

*/
QStringList QCodeCompletionEngine::triggers() const { return m_triggers; }

/*!

*/
void QCodeCompletionEngine::addTrigger(const QString &s) {
    if (m_triggers.contains(s))
        return;

    if (s.length() > m_max)
        m_max = s.length();

    m_triggers << s;
}

/*!

*/
void QCodeCompletionEngine::removeTrigger(const QString &s) {
    m_triggers.removeAll(s);
}

/*!

*/
QEditor *QCodeCompletionEngine::editor() const { return pEdit; }

/*!
        \brief Attach the completion engine instance to a new editor object
*/
void QCodeCompletionEngine::setEditor(QEditor *e) {
    if (pEdit) {
        pEdit->removeAction(pForcedTrigger, tr("&Edit"));
        // pEdit->removeEventFilter(this);

        disconnect(pEdit, SIGNAL(textEdited(QKeyEvent *)), this,
                   SLOT(textEdited(QKeyEvent *)));
    }

    pEdit = e;

    if (pEdit) {
        // pEdit->installEventFilter(this);
        pEdit->addAction(pForcedTrigger, tr("&Edit"));
        connect(pEdit, &QEditor::textEdited, this,
                &QCodeCompletionEngine::textEdited);
    }
}

/*!
        \internal
*/
void QCodeCompletionEngine::run() {
    if (m_cur.isNull())
        return;

    // qDebug("complete!");

    complete(m_cur, m_trig);

    m_cur = QDocumentCursor();
    m_trig.clear();
}

/*!
        \brief Forced completion trigger
*/
void QCodeCompletionEngine::complete(const QString &trigger) {
    complete(editor()->cursor(), trigger);
}

/*!
        \brief Standard completion entry point for QEditor
        \param e QKeyEvent that caused a modification of the text

        \note This slot is only called when editing happens without
        any cursor mirrors
*/
void QCodeCompletionEngine::textEdited(QKeyEvent *k) {
    QString s, txt = s = k->text();
    QDocumentCursor cur = editor()->cursor();

    auto count = txt.length();

    if (txt.isEmpty()) {
        return;
    }

    if (m_triggers.isEmpty()) {
        triggerWordLenComplete();
        return;
    }

    // qDebug("should trigger completion? (bis)");

    if (count > m_max) {
        txt = txt.right(m_max);

    } else if (count < m_max) {

        QDocumentCursor c(cur);
        c.movePosition(m_max, QDocumentCursor::Left,
                       QDocumentCursor::KeepAnchor);

        // qDebug("prev text : %s", qPrintable(c.selectedText()));

        txt = c.selectedText();
    }

    // qDebug("text : %s", qPrintable(txt));
    for (auto &trig : m_triggers) {
        if (txt.endsWith(trig)) {
            cur = editor()->cursor();
            cur.movePosition(trig.size(), QDocumentCursor::PreviousCharacter);

            // notify completion trigger
            emit completionTriggered(trig);

            // get rid of previous calltips/completions
            editor()->setFocus();

            // trigger completion
            complete(cur, trig);
            return;
        }
    }

    triggerWordLenComplete();
}

/*!
        \internal
*/
bool QCodeCompletionEngine::eventFilter(QObject *o, QEvent *e) {
    if (!e || !o || (e->type() != QEvent::KeyPress) || (o != pEdit))
        return false;

    // qDebug("should trigger completion?");

    QDocumentCursor cur = editor()->cursor();
    QKeyEvent *k = static_cast<QKeyEvent *>(e);

    QString s, txt = s = k->text();

    auto count = txt.length();

    if (txt.isEmpty() || m_triggers.isEmpty())
        return false; // QThread::eventFilter(o, e);

    // qDebug("should trigger completion? (bis)");

    if (count > m_max) {
        txt = txt.right(m_max);

    } else if (count < m_max) {

        QDocumentCursor c(cur);
        c.movePosition(m_max - count, QDocumentCursor::Left,
                       QDocumentCursor::KeepAnchor);

        // qDebug("prev text : %s", qPrintable(c.selectedText()));

        txt.prepend(c.selectedText());
    }

    // qDebug("text : %s", qPrintable(txt));

    for (auto &trig : m_triggers) {
        if (txt.endsWith(trig)) {
            editor()->write(s);

            cur = editor()->cursor();
            cur.movePosition(trig.length(), QDocumentCursor::PreviousCharacter);

            // notify completion trigger
            emit completionTriggered(trig);

            // get rid of previous calltips/completions
            editor()->setFocus();

            // trigger completion
            complete(cur, trig);

            return true;
        }
    }

    return false;
}

/*!
        \brief Completion callback
*/
void QCodeCompletionEngine::complete(const QDocumentCursor &c,
                                     const QString &trigger) {
    Q_UNUSED(c)
    Q_UNUSED(trigger)
    qWarning("From complete(QDocumentCursor, QString)");
    qWarning("QCodeCompletionEngine is not self-sufficient : subclasses should "
             "reimplement at least one of the complete() method...");
}

void QCodeCompletionEngine::triggerWordLenComplete() {
    if (m_trigWordLen > 0) {
        QDocumentCursor cur = editor()->cursor();
        emit completionTriggered({});
        complete(cur, {});
    }
}

qsizetype QCodeCompletionEngine::trigWordLen() const { return m_trigWordLen; }

void QCodeCompletionEngine::setTrigWordLen(qsizetype newTrigWordLen) {
    m_trigWordLen = newTrigWordLen;
}
