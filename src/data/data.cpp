/*
    Tyrex - the versatile file decoder.
    Copyright (C) 2014 - 2015  G. Endignoux

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


std::shared_ptr<graphic::TreeNodeModel> Data::treeModel(QString title) const
{
    std::shared_ptr<graphic::TreeNodeModel> result = std::make_shared<graphic::TreeNodeModel>(title);
    this->appendToTree(*result);
    return result;
}

void Data::appendToTree(graphic::TreeNodeModel& tree) const
{
    this->doAppendToTree(tree);

    if (!mErrors.empty())
    {
        std::shared_ptr<graphic::TreeNodeModel> node = std::make_shared<graphic::TreeNodeModel>("Errors");
        for (unsigned int i = 0 ; i < mErrors.size() ; ++i)
        {
            std::shared_ptr<graphic::TreeNodeModel> tmp = std::make_shared<graphic::TreeNodeModel>("Error " + QString::number(i));
            mErrors[i]->appendToTree(*tmp);
            node->appendTree(tmp);
        }
        tree.appendTree(node);
    }
}

std::shared_ptr<graphic::View> Data::firstView() const
{
    std::shared_ptr<graphic::View> result(mFirstView);
    mFirstView.reset();
    return result;
}

}
}
