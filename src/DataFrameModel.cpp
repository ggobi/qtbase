#include "DataFrameModel.hpp"

#include <QStringList>
#include <QMimeData>

#include "convert.hpp"
#include "NameOnlyClass.hpp"

QVariant DataFrameModel::data(const QModelIndex &index, int role) const
{
  return qvariant_from_sexp(dataFrameColumn(index, role), index.row());
}

/* Provides access to the SEXP vector backing the column + role. This
   allows RTextFormattingDelegate to inspect the actual R object when
   determining the appropriate editing widget. We thought about using
   a custom role for the type or something, but that is not much
   cleaner than this, and this has more potential. */
SEXP DataFrameModel::dataFrameColumn(const QModelIndex &index, int role) const
{
  int col = index.column();
  int row = index.row();
  SEXP value = R_NilValue;
  QModelIndex dummy;

  if (!index.isValid()) {
    qCritical("Model index is invalid");
    return value;
  }
  if (col >= columnCount(dummy)) {
    qCritical("Column index %d out of bounds", col);
    return value;
  }
  if (row >= rowCount(dummy)) {
    qCritical("Row index %d out of bounds", row);
    return value;
  }
  if (role >= length(_roles)) {
    //qCritical("Role index %d out of bounds", role);
    return value;
  }

  SEXP roleVector = VECTOR_ELT(_roles, role);
  int dfIndex;
  if (roleVector == R_NilValue || (dfIndex = INTEGER(roleVector)[col]) == -1) {
    if (role == Qt::ToolTipRole)
      value = dataFrameColumn(index, Qt::DisplayRole);
    if (role == Qt::DisplayRole)
      value = dataFrameColumn(index, Qt::EditRole);
  } else value = VECTOR_ELT(_dataframe, dfIndex);
  
  return value;
}

QVariant DataFrameModel::headerData(int section, Qt::Orientation orientation,
                                    int role) const
{
  QVariant value;
  QModelIndex dummy;
  if (section < 0) {
    qCritical("Invalid section number: %d", section);
    return value;
  }
  if (orientation == Qt::Horizontal) {
    if (section >= columnCount(dummy)) {
      qCritical("Column header index %d out of bounds", section);
      return value;
    }
    if (role >= length(_colHeader)) {
      //qCritical("Column header role %d out of bounds", role);
      return value;
    }
    SEXP roleVector = VECTOR_ELT(_colHeader, role);
    if (roleVector != R_NilValue)
      value = qvariant_from_sexp(roleVector, section);
  } else {
    if (section >= rowCount(dummy)) {
      qCritical("Row header index %d out of bounds", section);
      return value;
    }
    if (role >= length(_rowHeader)) {
      //qCritical("Row header role %d out of bounds", role);
      return value;
    }
    SEXP roleVector = VECTOR_ELT(_rowHeader, role);
    if (roleVector != R_NilValue)
      value = qvariant_from_sexp(roleVector, section);
  }
  return value;
}

bool DataFrameModel::setData(const QModelIndex &index, const QVariant &value,
                             int role)
{
  int col = index.column();
  int row = index.row();
  QModelIndex dummy;

  if (!index.isValid()) {
    qCritical("Model index is invalid");
    return false;
  }
  if (col >= columnCount(dummy)) {
    qCritical("Column index %d out of bounds", col);
    return false;
  }
  if (row >= rowCount(dummy)) {
    qCritical("Row index %d out of bounds", row);
    return false;
  }
  if (role >= length(_roles)) {
    qCritical("Role index %d out of bounds", role);
    return false;
  }

  SEXP roleVector = VECTOR_ELT(_roles, role);
  int dfIndex;
  
  if (roleVector == R_NilValue || (dfIndex = INTEGER(roleVector)[col]) == -1)
    return(false);

  SEXP tmpDataframe = duplicate(_dataframe);
  R_ReleaseObject(_dataframe);
  _dataframe = tmpDataframe;
  R_PreserveObject(_dataframe);

  SEXP v = VECTOR_ELT(_dataframe, dfIndex);
  bool success = qvariant_into_vector(value, v, row);
  if (success)
    dataChanged(index, index);
  return success;
}

Qt::ItemFlags DataFrameModel::flags(const QModelIndex &index) const {
  int col = index.column();
  int row = index.row();
  QModelIndex dummy;

  Qt::ItemFlags f = QAbstractItemModel::flags(index);
  if (index.isValid()) {
    SEXP roleVector = VECTOR_ELT(_roles, Qt::EditRole);
    if (roleVector != R_NilValue && INTEGER(roleVector)[col] != -1)
      f |= Qt::ItemIsEditable;
    f |= Qt::ItemIsDragEnabled;
  }
  return f;
}

static void OutCharQDS(R_outpstream_t stream, int c)
{
  QDataStream *qds = reinterpret_cast<QDataStream *>(stream->data);
  (*qds) << c;
}

static void OutBytesQDS(R_outpstream_t stream, void *buf, int length)
{
  QDataStream *qds = reinterpret_cast<QDataStream *>(stream->data);
  qds->writeRawData(reinterpret_cast<const char *>(buf), length);
}

static void InitQDSOutPStream(R_outpstream_t stream, QDataStream *data) {
  R_InitOutPStream(stream, data, R_pstream_xdr_format, 0,
                   OutCharQDS, OutBytesQDS, NULL, NULL);
}

QStringList DataFrameModel::mimeTypes() const
{
  QStringList types = QAbstractItemModel::mimeTypes();
  types << "application/x-rlang-transport";
  return types;
}

QMimeData *DataFrameModel::mimeData(const QModelIndexList &indexes) const
{
  SEXP r_list = allocVector(VECSXP, indexes.size());
  for (int i = 0; i < indexes.size(); i++) {
    QModelIndex index = indexes[i];
    if (index.isValid()) {
      SEXP obj = to_sexp(data(index, Qt::DisplayRole));
      SET_VECTOR_ELT(r_list, i, obj);
    }
  }

  QMimeData *mimeData = QAbstractItemModel::mimeData(indexes);
  QByteArray encodedData;
  QDataStream stream(&encodedData, QIODevice::WriteOnly);
  struct R_outpstream_st r_stream;
  InitQDSOutPStream(&r_stream, &stream);
  R_Serialize(r_list, &r_stream);
  
  mimeData->setData("application/x-rlang-transport", encodedData);
  return mimeData;
}

void DataFrameModel::beginChanges(int nr, int nc) {
  if (_dataframe != R_NilValue) {
    int oldnr = rowCount(QModelIndex());
    int oldnc = columnCount(QModelIndex());
    if (oldnr > nr)
      beginRemoveRows(QModelIndex(), nr, oldnr - 1L);
    else if (oldnr < nr)
      beginInsertRows(QModelIndex(), oldnr, nr - 1L);
    if (oldnc > nc)
      beginRemoveColumns(QModelIndex(), nc, oldnc - 1L);
    else if (oldnc < nc)
      beginInsertColumns(QModelIndex(), oldnc, nc - 1L);
  }
}

void DataFrameModel::endChanges(int oldnr, int oldnc) {
  if (oldnr != -1) {
    int nr = rowCount(QModelIndex());
    int nc = columnCount(QModelIndex());
    if (oldnc > nc)
      endRemoveColumns();
    else if (oldnc < nc)
      endInsertColumns(); // just in case column names/roles changed
    else headerDataChanged(Qt::Horizontal, 0, nc);
    if (oldnr > nr) // insert rows
      endRemoveRows();
    else if (oldnr < nr)
      endInsertRows();
    else headerDataChanged(Qt::Vertical, 0, nr);
    // be lazy and just say everything changed
    // will not matter unless many rows/cols are in view (rare)
    dataChanged(index(0, 0), index(nr, nc));
  }
}

void DataFrameModel::setDataFrame(SEXP dataframe, SEXP roles, SEXP rowHeader,
                                  SEXP colHeader)
{
  R_PreserveObject(dataframe);
  R_PreserveObject(roles);
  R_PreserveObject(rowHeader);
  R_PreserveObject(colHeader);

  // need dimension changes up-front
  beginChanges(headerLength(rowHeader), headerLength(colHeader));

  int oldnr = rowCount(QModelIndex()); // returns -1 if no dataframe
  int oldnc = columnCount(QModelIndex());
  
  _dataframe = dataframe;
  _roles = roles;
  _rowHeader = rowHeader;
  _colHeader = colHeader;

  // finish change notifications
  endChanges(oldnr, oldnc);
}

DataFrameModel::~DataFrameModel() {
  R_ReleaseObject(_dataframe);
  R_ReleaseObject(_roles);
  R_ReleaseObject(_rowHeader);
  R_ReleaseObject(_colHeader);
  R_ReleaseObject(_useRoles);
  R_ReleaseObject(_editable);
}

extern "C"
SEXP qt_qdataFrameModel(SEXP rparent, SEXP useRoles, SEXP editable) {
  static Class *dataFrameModelClass =
    new NameOnlyClass("DataFrameModel", Class::fromName("QAbstractTableModel"));
  SmokeObject *so =
    SmokeObject::fromPtr(new DataFrameModel(unwrapSmoke(rparent, QObject),
                                            useRoles, editable),
                         Class::fromName("QAbstractTableModel"), true);
  so->cast(dataFrameModelClass);
  return so->sexp();
}

extern "C"
SEXP qt_qsetDataFrame(SEXP rmodel, SEXP df, SEXP roles, SEXP rowHeader,
                      SEXP colHeader)
{
  DataFrameModel *model =
    static_cast<DataFrameModel *>(unwrapSmoke(rmodel, QAbstractTableModel));
  model->setDataFrame(df, roles, rowHeader, colHeader);
  return R_NilValue;
}

extern "C"
SEXP qt_qdataFrame(SEXP rmodel) {
  DataFrameModel *model =
    static_cast<DataFrameModel *>(unwrapSmoke(rmodel, QAbstractTableModel));
  return model->dataFrame();
}

extern "C"
SEXP qt_quseRoles(SEXP rmodel) {
  DataFrameModel *model =
    static_cast<DataFrameModel *>(unwrapSmoke(rmodel, QAbstractTableModel));
  return model->useRoles();
}

extern "C"
SEXP qt_qeditable(SEXP rmodel) {
  DataFrameModel *model =
    static_cast<DataFrameModel *>(unwrapSmoke(rmodel, QAbstractTableModel));
  return model->editable();
}
