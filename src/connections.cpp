#include <QIODevice>
#include "SmokeObject.hpp"

#include <Rinternals.h>

#if ! defined(R_CONNECTIONS_VERSION) || R_CONNECTIONS_VERSION != 1
#error "Connections API version must be 1"
#endif

#define R_EOF	-1

extern "C" SEXP qt_qtcon_QIODevice(SEXP x, SEXP s_binary) {
    const QIODevice *device = unwrapSmoke(x, QIODevice);
    Rboolean binary = asLogical(s_binary);
    const char *mode = "a+b";
    gboolean blocking = TRUE;
    Rconnection con;
    const char *desc = "<QIODevice>";
    
    QFileDevice *file = qobject_cast<QFileDevice *>(device);
    if (file != NULL) {
	desc = file->fileName();
    }
  
    SEXP rc = R_new_custom_connection(desc, mode, "QIODeviceConnection", &con);

    QObject parent = new QObject();
    device->setParent(parent);
    con->private = device;
    con->open = R_qtcon_open;
    con->canseek = !device->isSequential();
    if (con->canseek) {
	con->seek = R_qtcon_seek;
    }
    con->canwrite = device->isWritable();
    con->canread = device->isReadable();
    con->text = !binary;
    con->blocking = TRUE;
    con->destroy = R_qtcon_destroy;  
    con->isopen = device->isOpen();
    con->close = R_qtcon_close;
    if (con->canseek) {
	con->seek = R_qtcon_seek;
    }
    con->read = R_qtcon_read;
    con->fgetc_internal = R_qtcon_fgetc_internal;
    con->fflush = R_qtcon_fflush;
    con->write = R_qtcon_write;
  
    return rc;
}

static QIODevice::OpenMode modeToOpenMode(const char *mode) {
    QString qmode(mode);
    QIODevice::OpenMode openMode = QIODevice::NotOpen;

    if (qmode.startsWith("w+"))
	openMode |= QIODevice::WriteOnly;
    else if (qmode.startsWith("w") || qmode.startsWith("r+"))
	error("'%s' mode not supported by QIODevice, try 'w+'", mode);
    if (qmode.startsWith("a"))
	openMode |= QIODevice::Append;
    if (qmode.startsWith("r") || qmode.contains("+"))
	openMode |= QIODevice::ReadOnly;
    if (!qmode.endsWith("b"))
	openMode |= QIODevice::Text;

    return openMode;
}

static void R_qtcon_open(Rconnection con) {
    QIODevice *device = static_cast<QIODevice *>(con->private);
    QIODevice::OpenMode mode = modeToOpenMode(con->open);
    device->open(mode);
}

static void R_qtcon_destroy(Rconnection con) {
    QIODevice *device = static_cast<QIODevice *>(con->private);
    device->parent()->deleteLater();
}

static void R_qtcon_close(Rconnection con) {
    QIODevice *device = static_cast<QIODevice *>(con->private);
    device->close();
    if (device->errorString() != NULL) {
	error("Failed to close QIODevice: %s", device->errorString());
    }
    con->isopen = FALSE;
}

static size_t R_qtcon_read(void *ptr, size_t size,
			   size_t nitems, Rconnection con) {
    QIODevice *device = static_cast<QIODevice *>(con->private);
    
    int numread = device->read(ptr, size*nitems);
    
    if (device->errorString() != NULL) {
	error("Failed to read from QIODevice: %s", device->errorString());
    }
    
    return numread / size;
}

static int R_qtcon_fgetc_internal(Rconnection con) {
    int c;
    bool wasread;
    QIODevice *device = static_cast<QIODevice *>(con->private);

    wasread = device->getChar(&c);
    if (!wasread) {
	c = R_EOF;
    }
    
    if (device->errorString() != NULL) {
	error("Failed to read char from QIODevice: %s", device->errorString());
    }
    
    return c;
}

static double R_qtcon_seek(Rconnection con, double where, int origin, int rw) {
    QIODevice *device = static_cast<QIODevice *>(con->private);

    qint64 pos;

    if(ISNA(where)) return (double) device->pos();
    
    switch(origin) {
    case 2: where = device->pos() + where; break;
    case 3: where = device->size() + where - 1; break;
    default:
    }

    pos = device->seek(where);
 
    if (device->errorString() != NULL) {
	error("Failed to seek in QIODevice: %s", device->errorString());
    }
    return pos;
}

static int R_qtcon_fflush(Rconnection con) {
    QIODevice *device = static_cast<QIODevice *>(con->private);
    bool success = device->waitForBytesWritten(-1);
    if (device->errorString() != NULL) {
	error("Failed to flush QIODevice: %s", device->errorString());
    }
    return success;
}

static size_t R_qtcon_write(const void *ptr, size_t size, size_t nitems,
			    Rconnection con)
{
    qint64 numout;
    QIODevice *device = static_cast<QIODevice *>(con->private);
  
    numout = device->write(ptr, size * nitems);

    if (device->errorString() != NULL) {
	error("Failed to write to QIODevice: %s", device->errorString());
    }

    return numout / size;  
}
