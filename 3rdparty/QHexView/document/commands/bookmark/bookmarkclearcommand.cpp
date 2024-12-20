#include "bookmarkclearcommand.h"

BookMarkClearCommand::BookMarkClearCommand(
    QHexDocument *doc, const QMap<qsizetype, QString> &bookmarks,
    QUndoCommand *parent)
    : QUndoCommand(parent), m_doc(doc), m_bookmarks(bookmarks) {}

void BookMarkClearCommand::redo() { m_doc->clearBookMark(); }

void BookMarkClearCommand::undo() { m_doc->applyBookMarks(m_bookmarks); }
