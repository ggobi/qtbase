#ifndef CLASS_FACTORY_H
#define CLASS_FACTORY_H

class Class;
class Smoke;
class QMetaObject;

/* Allows overriding the construction of Class instances. The Class
   provides the Methods, which sit at the core of the framework.
*/
class ClassFactory {
public:
  virtual Class *createClass(Smoke *smoke, int classId);
  virtual Class *createClass(const QMetaObject *meta);
};

#endif
