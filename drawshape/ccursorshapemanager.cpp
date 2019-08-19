#include "ccursorshapemanager.h"

CCursorShapeManager::CCursorShapeManager()
{

}

QCursor CCursorShapeManager::getRotateCursor()
{
    return QCursor((QPixmap(":/theme/light/images/mouse_style/shape/rotate_mouse.png")));
}
