/* Implement QAbstractTableModel against an R data.frame */

#include <QAbstractTableModel>
#include <Rinternals.h>

class DataFrameModel : public QAbstractTableModel {
public:

  DataFrameModel() : _dataframe(R_NilValue), _rowHeader(R_NilValue),
                     _colHeader(R_NilValue), _roles(R_NilValue) { }
  
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

  void setDataFrame(SEXP dataframe, SEXP roles, SEXP rowHeader, SEXP colHeader);

  SEXP dataFrame() { return _dataframe; }
  
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
};
