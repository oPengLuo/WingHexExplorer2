#ifndef UTILITIES_H
#define UTILITIES_H

#include <QApplication>
#include <QCryptographicHash>
#include <QFile>
#include <QFileInfo>
#include <QList>
#include <QMimeDatabase>
#include <QScreen>
#include <QStyle>
#include <QWidget>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QStringDecoder>
#else
#include <QTextCodec>
#endif

#ifdef Q_OS_WIN
#include <windows.h>
#else
#include <unistd.h>
#endif

#define PROEXT ".wingpro"

Q_DECL_UNUSED static inline QString NAMEICONRES(const QString &name) {
    return ":/com.wingsummer.winghex/images/" + name + ".png";
}

Q_DECL_UNUSED static inline QIcon ICONRES(const QString &name) {
    return QIcon(NAMEICONRES(name));
}

/*
struct HexFile {
    QJsonDocument *doc;
    QHexRenderer *render;
    QString filename;
    QString workspace;
    int vBarValue;
    bool isdriver;
    QByteArray md5; // only for RegionFile
};*/

static QStringList encodingsBuffer;

class Utilities {
private:
public:
    static inline bool isRoot() {
#ifdef Q_OS_WIN
        BOOL isAdmin = FALSE;
        PSID adminGroup = NULL;

        SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;
        if (AllocateAndInitializeSid(
                &ntAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID,
                DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &adminGroup)) {
            // Check whether the token of the current process is a member of the
            // Admin group.
            if (!CheckTokenMembership(NULL, adminGroup, &isAdmin)) {
                isAdmin = FALSE;
            }
            FreeSid(adminGroup);
        }
        return isAdmin;
#else
        return getuid() == 0;
#endif
    }

    static QString processBytesCount(qint64 bytescount) {
        QStringList B{"B", "KB", "MB", "GB", "TB"};
        auto av = bytescount;
        auto r = av;

        for (int i = 0; i < 5; i++) {
            auto lld = lldiv(r, 1024);
            r = lld.quot;
            av = lld.rem;
            if (r == 0) {
                return QStringLiteral("%1 %2").arg(av).arg(B.at(i));
            }
        }

        return QStringLiteral("%1 TB").arg(av);
    }

    static QStringList getEncodings() {
        if (encodingsBuffer.length() > 0)
            return encodingsBuffer;
        QStringList encodings;

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        for (auto e = 0; e < int(QStringConverter::Encoding::LastEncoding);
             ++e) {
            auto ee = QStringConverter::nameForEncoding(
                QStringConverter::Encoding(e));
            if (ee == QStringLiteral("ISO-8859-1")) {
                encodings << QStringLiteral("ASCII");
            }
            encodings << ee;
        }
#else
        for (auto &e : QTextCodec::availableCodecs()) {
            if (e == QStringLiteral("ISO-8859-1")) {
                encodings << QStringLiteral("ASCII");
            } else {
                encodings << e;
            }
        }
#endif

        encodingsBuffer = encodings;
        return encodings;
    }

    static QByteArray getMd5(QString filename) {
        QFile sourceFile(filename);
        qint64 fileSize = sourceFile.size();
        const qint64 bufferSize = 10240;

        if (sourceFile.open(QIODevice::ReadOnly)) {
            char buffer[bufferSize];
            int bytesRead;
            auto readSize = qMin(fileSize, bufferSize);

#if QT_DEPRECATED_SINCE(6, 4)
            QByteArray hash;
#else
            QCryptographicHash hash(QCryptographicHash::Md5);
#endif

            while (readSize > 0 &&
                   (bytesRead = int(sourceFile.read(buffer, readSize))) > 0) {
                fileSize -= bytesRead;
#if QT_DEPRECATED_SINCE(6, 4)
                hash +=
                    QCryptographicHash::hash(buffer, QCryptographicHash::Md5);
#else
                hash.addData(buffer, bytesRead);
#endif
                readSize = qMin(fileSize, bufferSize);
            }

            sourceFile.close();
#if QT_DEPRECATED_SINCE(6, 4)
            return hash;
#else
            return hash.result();
#endif
        }

        return QByteArray();
    }

    static bool checkIsLittleEndian() {
        short s = 0x1122;
        auto l = *reinterpret_cast<char *>(&s);
        return l == 0x22;
    }

    /*
    static bool isRegionFile(QHexDocument *doc) {
        return doc ? doc->documentType() == DocumentType::RegionFile : false;
    }*/

    static QIcon getIconFromFile(QStyle *style, const QString &filename) {
        QMimeDatabase db;
        auto t = db.mimeTypeForFile(filename);
        auto ico = t.iconName();
        auto qicon = QIcon::fromTheme(ico, QIcon(ico));
        return qicon.availableSizes().count()
                   ? qicon
                   : style->standardIcon(QStyle::SP_FileIcon);
    }

    static bool fileCanWrite(QString path) {
        return QFileInfo(path).permission(QFile::WriteUser);
    }

    static void moveToCenter(QWidget *window) {
        if (window == nullptr)
            return;
        auto screen = qApp->primaryScreen()->availableSize();
        window->move((screen.width() - window->width()) / 2,
                     (screen.height() - window->height()) / 2);
    }

    static bool isTextFile(const QFileInfo &info) {
        QMimeDatabase db;
        auto t = db.mimeTypeForFile(info);
        return t.inherits(QStringLiteral("text/plain"));
    }
};

#endif // UTILITIES_H