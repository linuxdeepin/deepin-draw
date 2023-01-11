/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     Tan Lang <tanlang@uniontech.com>
 *
 * Maintainer: Tan Lang <tanlang@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef ATTRIBUTIONREGISTERMANAGER_H
#define ATTRIBUTIONREGISTERMANAGER_H
#include "iattributionregister.h"

class AttributionRegisterManager : public IAttributionRegister
{
public:
    using IAttributionRegister::IAttributionRegister;
    virtual void registe() override;
    void addRegister(IAttributionRegister *pRegister);
    virtual ~AttributionRegisterManager() override;
private:
    void addDefaultAttri();

private:
    QList<IAttributionRegister *> m_registers;
};

#endif // ATTRIBUTIONREGISTERMANAGER_H
