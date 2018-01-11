/* Implement QAbstractTableModel against an R data.frame */

#include <QAbstractTableModel>
#include <Rinternals.h>

class DataFrameModel : public QAbstractTableModel {
  Q_OBJECT

public:

  DataFrameModel(QObject *parent, SEXP useRoles, SEXP editable)
    : _dataframe(R_NilValue), _rowHeader(R_NilValue),
      _roles(R_NilValue), _colHeader(R_NilValue), _useRoles(useRoles),
      QAbstractTableModel(parent), _editable(editable)
  {
    R_PreserveObject(_useRoles);
    R_PreserveObject(_editable);
  }
  
  ~DataFrameModel();

  int rowCount(const QModelIndex &/*parent*/) const {
    return headerLength(_rowHeader);
  }

  int columnCount (const QModelIndex &/*parent*/) const {
    return headerLength(_colHeader);
  }

  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
  QVariant headerData(int section, Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const;

  bool setData(const QModelIndex &index, const QVariant &value,
               int role = Qt::EditRole);

  Qt::ItemFlags flags(const QModelIndex &index) const;
  
  QStringList mimeTypes() const;
  QMimeData *mimeData(const QModelIndexList &indexes) const;
  
  void setDataFrame(SEXP dataframe, SEXP roles, SEXP rowHeader, SEXP colHeader);

  SEXP dataFrame() { return _dataframe; }

  SEXP useRoles() { return _useRoles; }
  SEXP editable() { return _editable; }

  SEXP dataFrameColumn(const QModelIndex &index, int role) const;
  
private:

  int headerLength(SEXP header) const {
    return header == R_NilValue ? -1 :
      length(VECTOR_ELT(header, Qt::DisplayRole));
  }
  
  void beginChanges(int nr, int nc);
  void endChanges(int nr, int nc);
  
  SEXP _dataframe;
  SEXP _roles;
  SEXP _rowHeader;
  SEXP _colHeader;

  /* These are saved here for use on the R side */
  SEXP _useRoles;
  SEXP _editable;
};
