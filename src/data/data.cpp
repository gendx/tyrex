/*
    Tyrex - the versatile file decoder.
    Copyright (C) 2014  G. Endignoux

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see http://www.gnu.org/licenses/gpl-3.0.txt
*/

#include "data.hpp"

#include "graphic/treemodel.hpp"

namespace tyrex {
namespace data {

Data::~Data()
{
}


Shared<graphic::TreeNodeModel> Data::treeModel(QString title) const
{
    Shared<graphic::TreeNodeModel> result = makeShared<graphic::TreeNodeModel>(title);
    this->appendToTree(*result);
    return result;
}

Shared<graphic::View> Data::firstView() const
{
    Shared<graphic::View> result(mFirstView);
    mFirstView.reset();
    return result;
}

}
}
