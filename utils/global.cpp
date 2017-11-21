#include "global.h"

GlobalShortcut* GlobalShortcut::m_globalSc = nullptr;

GlobalShortcut* GlobalShortcut::instance() {
    if (!m_globalSc)
    {
        m_globalSc = new GlobalShortcut();
    }

    return m_globalSc;
}

 GlobalShortcut::GlobalShortcut(QObject* parent)
  {
     Q_UNUSED(parent);
 }
