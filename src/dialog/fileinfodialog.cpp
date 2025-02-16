/*==============================================================================
** Copyright (C) 2024-2027 WingSummer
**
** This program is free software: you can redistribute it and/or modify it under
** the terms of the GNU Affero General Public License as published by the Free
** Software Foundation, version 3.
**
** This program is distributed in the hope that it will be useful, but WITHOUT
** ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
** FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more
** details.
**
** You should have received a copy of the GNU Affero General Public License
** along with this program. If not, see <https://www.gnu.org/licenses/>.
** =============================================================================
*/

#include "fileinfodialog.h"
#include "utilities.h"
#include <QDateTime>
#include <QFile>
#include <QFileInfo>
#include <QLabel>
#include <QMimeDatabase>
#include <QPicture>
#include <QTextBrowser>
#include <QVBoxLayout>

FileInfoDialog::FileInfoDialog(QString filename, QWidget *parent)
    : FramelessDialogBase(parent) {
    static const QString dfmt("yyyy/MM/dd hh:mm:ss ddd");

    auto widget = new QWidget(this);
    auto layout = new QVBoxLayout(widget);

    auto l = new QLabel(this);
    l->setFixedHeight(50);
    l->setAlignment(Qt::AlignCenter);

    QIcon icon;
    auto b = new QTextBrowser(this);

    if (filename.isEmpty()) {
        icon = this->style()->standardIcon(QStyle::SP_FileIcon);
        b->append(tr("FileNew"));
    } else {
        QMimeDatabase db;
        auto t = db.mimeTypeForFile(filename);
        auto ico = t.iconName();
        icon = QIcon::fromTheme(ico, QIcon(ico));
        if (icon.isNull()) {
            icon = this->style()->standardIcon(QStyle::SP_FileIcon);
        }
        QFileInfo finfo(filename);
        b->append(tr("FileName:") + finfo.fileName());
        b->append(tr("FilePath:") + finfo.filePath());
        b->append(tr("FileSize:") + Utilities::processBytesCount(finfo.size()));
        b->append(tr("Mime:") + t.name());
        b->append(tr("Md5:") + Utilities::getMd5(filename).toHex().toUpper());
        b->append(
            tr("FileBirthTime:") +
            finfo.fileTime(QFile::FileTime::FileBirthTime).toString(dfmt));
        b->append(
            tr("FileAccessTime:") +
            finfo.fileTime(QFile::FileTime::FileAccessTime).toString(dfmt));
        b->append(tr("FileModificationTime:") +
                  finfo.fileTime(QFile::FileTime::FileModificationTime)
                      .toString(dfmt));
        b->append(tr("LastRead:") + finfo.lastRead().toString(dfmt));
        b->append(tr("LastMod:") + finfo.lastModified().toString(dfmt));
    }

    auto availSizes = icon.availableSizes();
    QList<int> availLens;
    for (auto &size : availSizes) {
        if (size.width() != size.height()) {
            continue;
        }
        availLens << size.width();
    }
    std::sort(availLens.begin(), availLens.end(), std::greater<int>());
    auto ico = icon.pixmap(availLens.front(), availLens.front())
                   .toImage()
                   .scaled(50, 50, Qt::AspectRatioMode::KeepAspectRatio,
                           Qt::TransformationMode::SmoothTransformation);
    l->setPixmap(QPixmap::fromImage(ico));

    layout->addWidget(l, Qt::AlignHCenter);
    layout->addSpacing(10);
    layout->addWidget(b);

    buildUpContent(widget);

    setWindowTitle(tr("FileInfo"));
    this->resize(500, 450);
}

FileInfoDialog::~FileInfoDialog() {}
