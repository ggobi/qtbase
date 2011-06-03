// Basic defines

// So the headers compile as C++ or as C
#ifdef  __cplusplus
# define QT_BEGIN_DECLS  extern "C" {
# define QT_END_DECLS    }
#else
# define QT_BEGIN_DECLS
# define QT_END_DECLS
#endif
