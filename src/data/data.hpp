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

#ifndef TYREX_DATA_HPP
#define TYREX_DATA_HPP

#include "graphic/view/view.hpp"
#include "graphic/treemodel.hpp"
#include <QString>
#include <memory>

namespace tyrex {
namespace data {

class Data
{
public:
    virtual ~Data();

    void appendToTree(graphic::TreeNodeModel& tree) const;
    std::shared_ptr<graphic::TreeNodeModel> treeModel(QString title) const;

    std::shared_ptr<graphic::View> firstView() const;

    inline void addError(const std::shared_ptr<Data>& data);

protected:
    virtual void doAppendToTree(graphic::TreeNodeModel& tree) const = 0;

    mutable std::shared_ptr<graphic::View> mFirstView;

private:
    std::vector<std::shared_ptr<Data> > mErrors;
};

inline void Data::addError(const std::shared_ptr<Data>& data)
    {mErrors.push_back(data);}

}
}

#endif // TYREX_DATA_HPP
