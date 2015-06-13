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

#include "huffmantree.hpp"

#include "misc/tree.tpl"

namespace tyrex {
namespace parse {

HuffmanTree::HuffmanTree(const std::vector<unsigned int>& lengths, bool& check)
{
    unsigned int maxLength = 0;
    for (unsigned int len : lengths)
        if (len > maxLength)
            maxLength = len;

    mBinaryTree = std::vector<unsigned int>((1 << (maxLength + 1)) - 1, 0xFFFFFFFE);

    std::vector<unsigned int> count(maxLength + 1, 0);
    for (unsigned int len : lengths)
        ++count[len];
    count[0] = 0;

    std::vector<unsigned int> nextCode;
    unsigned int code = 0;
    nextCode.push_back(code);
    for (unsigned int bits = 1 ; bits <= maxLength ; ++bits)
    {
        code = (code + count[bits-1]) << 1;
        nextCode.push_back(code);
    }

    // Check tree.
    code += count[maxLength];
    check = (code == 1u << maxLength);

    std::vector<unsigned int> values(lengths.size(), 0);
    for (unsigned int n = 0 ; n < lengths.size() ; ++n)
    {
        unsigned int len = lengths[n];
        if (len != 0)
        {
            values[n] = nextCode[len];
            ++nextCode[len];

            unsigned int value = values[n];
            unsigned int index = 1;
            while (len)
            {
                mBinaryTree[index - 1] = 0xFFFFFFFF;
                --len;
                unsigned int bit = (value >> len) & 1;
                index = (index << 1) + bit;
            }

            --index;
            mBinaryTree[index] = n;
        }
    }
}


unsigned int HuffmanTree::parse(BitStream& stream) const
{
    unsigned int index = 1;
    while (mBinaryTree[index - 1] == 0xFFFFFFFF)
        index = (index << 1) + stream.get();
    return mBinaryTree[index - 1];
}


Tree<void> HuffmanTree::toTree() const
{
    Tree<void> result = Tree<void>(QString());

    std::shared_ptr<Tree<void> > root = std::make_shared<Tree<void> >(QString());
    this->toTree(*root, 0, QString());
    result.appendTree(root);

    return result;
}

bool HuffmanTree::toTree(Tree<void>& tree, unsigned int pos, QString sequence) const
{
    if (mBinaryTree[pos] == 0xFFFFFFFF)
    {
        bool result = false;

        std::shared_ptr<Tree<void> > left = std::make_shared<Tree<void> >(QString());
        if (this->toTree(*left, 2 * pos + 1, sequence + "0"))
        {
            tree.appendTree(left);
            result = true;
        }

        std::shared_ptr<Tree<void> > right = std::make_shared<Tree<void> >(QString());
        if (this->toTree(*right, 2 * pos + 2, sequence + "1"))
        {
            tree.appendTree(right);
            result = true;
        }

        return result;
    }
    else if (mBinaryTree[pos] == 0xFFFFFFFE)
        return false;
    else
        tree.mTitle = sequence + " : " + QString::number(mBinaryTree[pos]);
    return true;
}

}
}
