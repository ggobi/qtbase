#include <QEvent>
#include <QGraphicsItem>
#include <QGraphicsLayoutItem>
#include <QLayout>

#include <QListWidgetItem>
#include <QTableWidgetItem>
#include <QTreeWidgetItem>
#include <QWidget>

#include "RQtModule.hpp"
#include "SmokeObject.hpp"
#include "Class.hpp"

/*
 * Given an approximate classname and a qt instance, try to improve
 * the resolution of the name by using the various Qt rtti mechanisms
 * for QObjects, QEvents and so on
 */
Q_DECL_EXPORT int
resolve_classname_qt(const SmokeObject * o)
{
  int classId = o->classId();
  Smoke *smoke = o->smoke();
  const char *className = smoke->classes[classId].className;
  if (smoke->isDerivedFromByName(className, "QEvent")) {
    QEvent * qevent = (QEvent *)
      smoke->cast(o->ptr(), classId, smoke->idClass("QEvent").index);
    switch (qevent->type()) {
    case QEvent::Timer:
      classId = smoke->idClass("QTimerEvent").index;
      break;
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick:
    case QEvent::MouseMove:
      classId = smoke->idClass("QMouseEvent").index;
      break;
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
    case QEvent::ShortcutOverride:
      classId = smoke->idClass("QKeyEvent").index;
      break;
    case QEvent::FocusIn:
    case QEvent::FocusOut:
      classId = smoke->idClass("QFocusEvent").index;
      break;
    case QEvent::Enter:
    case QEvent::Leave:
      classId = smoke->idClass("QEvent").index;
      break;
    case QEvent::Paint:
      classId = smoke->idClass("QPaintEvent").index;
      break;
    case QEvent::Move:
      classId = smoke->idClass("QMoveEvent").index;
      break;
    case QEvent::Resize:
      classId = smoke->idClass("QResizeEvent").index;
      break;
    case QEvent::Create:
    case QEvent::Destroy:
      classId = smoke->idClass("QEvent").index;
      break;
    case QEvent::Show:
      classId = smoke->idClass("QShowEvent").index;
      break;
    case QEvent::Hide:
      classId = smoke->idClass("QHideEvent").index;
    case QEvent::Close:
      classId = smoke->idClass("QCloseEvent").index;
      break;
    case QEvent::Quit:
    case QEvent::ParentChange:
    case QEvent::ParentAboutToChange:
    case QEvent::ThreadChange:
    case QEvent::WindowActivate:
    case QEvent::WindowDeactivate:
    case QEvent::ShowToParent:
    case QEvent::HideToParent:
      classId = smoke->idClass("QEvent").index;
      break;
    case QEvent::Wheel:
      classId = smoke->idClass("QWheelEvent").index;
      break;
    case QEvent::WindowTitleChange:
    case QEvent::WindowIconChange:
    case QEvent::ApplicationWindowIconChange:
    case QEvent::ApplicationFontChange:
    case QEvent::ApplicationLayoutDirectionChange:
    case QEvent::ApplicationPaletteChange:
    case QEvent::PaletteChange:
      classId = smoke->idClass("QEvent").index;
      break;
    case QEvent::Clipboard:
      classId = smoke->idClass("QClipboardEvent").index;
      break;
    case QEvent::Speech:
    case QEvent::MetaCall:
    case QEvent::SockAct:
    case QEvent::WinEventAct:
    case QEvent::DeferredDelete:
      classId = smoke->idClass("QEvent").index;
      break;
    case QEvent::DragEnter:
      classId = smoke->idClass("QDragEnterEvent").index;
      break;
    case QEvent::DragLeave:
      classId = smoke->idClass("QDragLeaveEvent").index;
      break;
    case QEvent::DragMove:
      classId = smoke->idClass("QDragMoveEvent").index;
    case QEvent::Drop:
      classId = smoke->idClass("QDropEvent").index;
      break;
    case QEvent::DragResponse:
      classId = smoke->idClass("QDragResponseEvent").index;
      break;
    case QEvent::ChildAdded:
    case QEvent::ChildRemoved:
    case QEvent::ChildPolished:
      classId = smoke->idClass("QChildEvent").index;
      break;
    case QEvent::ShowWindowRequest:
    case QEvent::PolishRequest:
    case QEvent::Polish:
    case QEvent::LayoutRequest:
    case QEvent::UpdateRequest:
    case QEvent::EmbeddingControl:
    case QEvent::ActivateControl:
    case QEvent::DeactivateControl:
      classId = smoke->idClass("QEvent").index;
      break;
    case QEvent::ContextMenu:
      classId = smoke->idClass("QContextMenuEvent").index;
      break;
    case QEvent::InputMethod:
      classId = smoke->idClass("QInputMethodEvent").index;
      break;
    case QEvent::AccessibilityPrepare:
      classId = smoke->idClass("QEvent").index;
      break;
    case QEvent::TabletMove:
    case QEvent::TabletPress:
    case QEvent::TabletRelease:
      classId = smoke->idClass("QTabletEvent").index;
      break;
    case QEvent::LocaleChange:
    case QEvent::LanguageChange:
    case QEvent::LayoutDirectionChange:
    case QEvent::Style:
    case QEvent::OkRequest:
    case QEvent::HelpRequest:
      classId = smoke->idClass("QEvent").index;
      break;
    case QEvent::IconDrag:
      classId = smoke->idClass("QIconDragEvent").index;
      break;
    case QEvent::FontChange:
    case QEvent::EnabledChange:
    case QEvent::ActivationChange:
    case QEvent::StyleChange:
    case QEvent::IconTextChange:
    case QEvent::ModifiedChange:
    case QEvent::MouseTrackingChange:
      classId = smoke->idClass("QEvent").index;
      break;
    case QEvent::WindowBlocked:
    case QEvent::WindowUnblocked:
    case QEvent::WindowStateChange:
      classId = smoke->idClass("QWindowStateChangeEvent").index;
      break;
    case QEvent::ToolTip:
    case QEvent::WhatsThis:
      classId = smoke->idClass("QHelpEvent").index;
      break;
    case QEvent::StatusTip:
      classId = smoke->idClass("QEvent").index;
      break;
    case QEvent::ActionChanged:
    case QEvent::ActionAdded:
    case QEvent::ActionRemoved:
      classId = smoke->idClass("QActionEvent").index;
      break;
    case QEvent::FileOpen:
      classId = smoke->idClass("QFileOpenEvent").index;
      break;
    case QEvent::Shortcut:
      classId = smoke->idClass("QShortcutEvent").index;
      break;
    case QEvent::WhatsThisClicked:
      classId = smoke->idClass("QWhatsThisClickedEvent").index;
      break;
    case QEvent::ToolBarChange:
      classId = smoke->idClass("QToolBarChangeEvent").index;
      break;
    case QEvent::ApplicationActivated:
    case QEvent::ApplicationDeactivated:
    case QEvent::QueryWhatsThis:
    case QEvent::EnterWhatsThisMode:
    case QEvent::LeaveWhatsThisMode:
    case QEvent::ZOrderChange:
      classId = smoke->idClass("QEvent").index;
      break;
    case QEvent::HoverEnter:
    case QEvent::HoverLeave:
    case QEvent::HoverMove:
      classId = smoke->idClass("QHoverEvent").index;
      break;
    case QEvent::AccessibilityHelp:
    case QEvent::AccessibilityDescription:
      classId = smoke->idClass("QEvent").index;
#if QT_VERSION >= 0x40200
    case QEvent::GraphicsSceneMouseMove:
    case QEvent::GraphicsSceneMousePress:
    case QEvent::GraphicsSceneMouseRelease:
    case QEvent::GraphicsSceneMouseDoubleClick:
      classId = smoke->idClass("QGraphicsSceneMouseEvent").index;
      break;
    case QEvent::GraphicsSceneContextMenu:
      classId = smoke->idClass("QGraphicsSceneContextMenuEvent").index;
      break;
    case QEvent::GraphicsSceneHoverEnter:
    case QEvent::GraphicsSceneHoverMove:
    case QEvent::GraphicsSceneHoverLeave:
      classId = smoke->idClass("QGraphicsSceneHoverEvent").index;
      break;
    case QEvent::GraphicsSceneHelp:
      classId = smoke->idClass("QGraphicsSceneHelpEvent").index;
      break;
    case QEvent::GraphicsSceneDragEnter:
    case QEvent::GraphicsSceneDragMove:
    case QEvent::GraphicsSceneDragLeave:
    case QEvent::GraphicsSceneDrop:
      classId = smoke->idClass("QGraphicsSceneDragDropEvent").index;
      break;
    case QEvent::GraphicsSceneWheel:
      classId = smoke->idClass("QGraphicsSceneWheelEvent").index;
      break;
    case QEvent::KeyboardLayoutChange:
      classId = smoke->idClass("QEvent").index;
      break;
#endif
    default:
      break;
    }
  } else if (smoke->isDerivedFromByName(className, "QGraphicsItem")) {
    QGraphicsItem * item = (QGraphicsItem *)
      smoke->cast(o->ptr(), classId,
                     smoke->idClass("QGraphicsItem").index);
    switch (item->type()) {
    case 1:
      classId = smoke->idClass("QGraphicsItem").index;
      break;
    case 2:
      classId = smoke->idClass("QGraphicsPathItem").index;
      break;
    case 3:
      classId = smoke->idClass("QGraphicsRectItem").index;
    case 4:
      classId = smoke->idClass("QGraphicsEllipseItem").index;
      break;
    case 5:
      classId = smoke->idClass("QGraphicsPolygonItem").index;
      break;
    case 6:
      classId = smoke->idClass("QGraphicsLineItem").index;
      break;
    case 7:
      classId = smoke->idClass("QGraphicsItem").index;
      break;
    case 8:
      classId = smoke->idClass("QGraphicsTextItem").index;
      break;
    case 9:
      classId = smoke->idClass("QGraphicsSimpleTextItem").index;
      break;
    case 10:
      classId = smoke->idClass("QGraphicsItemGroup").index;
      break;
    }
  } else if (smoke->isDerivedFromByName(className, "QLayoutItem")) {
    QLayoutItem * item = (QLayoutItem *)
      smoke->cast(o->ptr(), classId,
                     smoke->idClass("QLayoutItem").index);
    if (item->widget() != 0) {
      classId = smoke->idClass("QWidgetItem").index;
    } else if (item->spacerItem() != 0) {
      classId = smoke->idClass("QSpacerItem").index;
    }
  }
  
  return classId;
}

Q_DECL_EXPORT bool
memory_is_owned_qt(const SmokeObject *o)
{
  Smoke *smoke = o->smoke();
  const char *className = o->klass()->name();
  
  if (	qstrcmp(className, "QListBoxItem") == 0
        || qstrcmp(className, "QStyleSheetItem") == 0
        || qstrcmp(className, "QSqlCursor") == 0
        || qstrcmp(className, "QModelIndex") == 0 )
  {
    return true;
  } else if (smoke->isDerivedFromByName(className, "QLayoutItem")) {
    QLayoutItem * item = (QLayoutItem *) o->castPtr("QLayoutItem");
    QLayout * layout = item->layout();
    if (layout) { // we really have a QLayout
      if (layout->parent() || layout->parentWidget())
        return true;
    } else if (item->widget() != 0 || item->spacerItem() != 0)
      return true; // NOTE: this will leak spacer items not in a layout
  } else if (qstrcmp(className, "QListWidgetItem") == 0) {
    QListWidgetItem * item = (QListWidgetItem *) o->ptr();
    if (item->listWidget() != 0) {
      return true;
    }
  } else if (smoke->isDerivedFromByName(className, "QTableWidgetItem")) {
    QTableWidgetItem * item = (QTableWidgetItem *) o->castPtr("QTableWidgetItem");
    if (item->tableWidget() != 0) {
      return true;
    }
  } else if (smoke->isDerivedFromByName(className, "QTreeWidgetItem")) {
    QTreeWidgetItem * item = (QTreeWidgetItem *) o->castPtr("QTreeWidgetItem");
    if (item->treeWidget() != 0) {
      return true;
    }
  } else if (smoke->isDerivedFromByName(className, "QWidget")) {
    QWidget * qwidget = (QWidget *) o->castPtr("QWidget");
    if (qwidget->parentWidget() != 0) {
      return true;
    }
    // Qt avoided garbage collecting custom QWidget subclasses here -- why?
    /*
    const QMetaObject * meta = qwidget->metaObject();
    Smoke::ModuleIndex classId = smoke->idClass(meta->className());
    return (classId.index == 0);
    */
  } else if (smoke->isDerivedFromByName(className, "QGraphicsItem")) {
    QGraphicsItem * item = (QGraphicsItem *) o->castPtr("QGraphicsItem");
    if (item->scene() != 0 || item->parentItem() != 0) {
      return true;
    }
  } else if (smoke->isDerivedFromByName(className, "QGraphicsLayoutItem")) {
    QGraphicsLayoutItem * item =
      (QGraphicsLayoutItem *) o->castPtr("QGraphicsLayoutItem");
    if (item->parentLayoutItem() != 0) {
      return true;
    }
  } else if (smoke->isDerivedFromByName(className, "QObject")) {
    QObject * qobject = (QObject *) o->castPtr("QObject");
    if (qobject->parent() != 0) {
      return true;
    }
  } else if (smoke->isDerivedFromByName(className, "QTextBlockUserData")) {
    return true;
  } 	
  return false;
}

void init_smoke(void) {
  if (qt_Smoke == 0) init_qt_Smoke();
  RSmokeBinding *binding = new RSmokeBinding(qt_Smoke);
  RQtModule *module = new RQtModule(binding, resolve_classname_qt,
                                    memory_is_owned_qt);
  RQtModule::registerModule(module);
}
