//=========================================================================
// Copyright (C) 2018 The C++ Component Model(CCM) Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "ccm/util/TreeNode.h"

namespace ccm {
namespace util {

extern const InterfaceID IID_ITreeNode =
    {{0x2b2b51d9,0xc2c4,0x4312,0x8a06, {0x6,0x7,0xd,0xe,0x1,0xc,0x1,0xf,0x7,0x5,0x4,0xb}},
    &CID_libcore};

CCM_INTERFACE_IMPL_1(TreeNode, LinkedHashMap::LinkedHashMapEntry, ITreeNode);

TreeNode::TreeNode(
    /* [in] */ Integer hash,
    /* [in] */ IInterface* key,
    /* [in] */ IInterface* val,
    /* [in] */ Node* next)
    : LinkedHashMap::LinkedHashMapEntry(hash, key, val, next)
{}

AutoPtr<TreeNode> TreeNode::Root()
{
    for (TreeNode* r = this, *p;;) {
        if (p = r->mParent, p == nullptr) {
            return r;
        }
        r = p;
    }
}

AutoPtr<TreeNode> TreeNode::GetTreeNode(
    /* [in] */ Integer h,
    /* [in] */ IInterface* k)
{
    return nullptr;
}

void TreeNode::Treeify(
    /* [in] */ Array<Node*>& tab)
{}

AutoPtr<TreeNode> TreeNode::PutTreeVal(
    /* [in] */ HashMap* map,
    /* [in] */ Array<Node*>& tab,
    /* [in] */ Integer h,
    /* [in] */ IInterface* k,
    /* [in] */ IInterface* v)
{
    return nullptr;
}

void TreeNode::RemoveTreeNode(
    /* [in] */ HashMap* map,
    /* [in] */ Array<Node*>& tab,
    /* [in] */ Boolean movable)
{}

void TreeNode::Split(
    /* [in] */ HashMap* map,
    /* [in] */ Array<Node*>& tab,
    /* [in] */ Integer index,
    /* [in] */ Integer bit)
{}

}
}
