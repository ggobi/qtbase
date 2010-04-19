/* Implement QAbstractTableModel against an R data.frame */

#include <QAbstractTableModel>
#include <Rinternals.h>

class DataFrameModel : public QAbstractTableModel {
public:
  
  ~DataFrameModel();

  int rowCount(const QModelIndex &/*parent*/) const {
    return length(VECTOR_ELT(_rowHeader, Qt::DisplayRole));
  }

  int columnCount (const QModelIndex &/*parent*/) const {
    return length(VECTOR_ELT(_colHeader, Qt::DisplayRole));
  }

  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
  QVariant headerData(int section, Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const;

  void setDataFrame(SEXP dataframe, SEXP roles, SEXP rowHeader, SEXP colHeader);

  SEXP dataFrame() { return _dataframe; }
  
private:
  SEXP _dataframe;
  SEXP _roles;
  SEXP _rowHeader;
  SEXP _colHeader;
};
