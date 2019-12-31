//=========================================================================
// Copyright (C) 2018 The C++ Component Model(COMO) Open Source Project
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

#ifndef __CCM_UTIL_TREENODE_H__
#define __CCM_UTIL_TREENODE_H__

#include "ccm/util/LinkedHashMap.h"
#include "ccm.core.IComparable.h"

using ccm::core::IComparable;

namespace ccm {
namespace util {

extern const InterfaceID IID_ITreeNode;

INTERFACE_ID(2b2b51d9-c2c4-4312-8a06-67de1c1f754b)
interface ITreeNode : public IInterface
{
    inline static ITreeNode* Probe(
        /* [in] */ IInterface* object)
    {
        if (object == nullptr) return nullptr;
        return (ITreeNode*)object->Probe(IID_ITreeNode);
    }
};

class TreeNode
    : public LinkedHashMap::LinkedHashMapEntry
    , public ITreeNode
{
public:
    TreeNode(
        /* [in] */ Integer hash,
        /* [in] */ IInterface* key,
        /* [in] */ IInterface* val,
        /* [in] */ Node* next);

    CCM_INTERFACE_DECL();

    AutoPtr<TreeNode> Root();

    static void MoveRootToFront(
        /* [in] */ Array<Node*>& tab,
        /* [in] */ TreeNode* root);

    AutoPtr<TreeNode> Find(
        /* [in] */ Integer h,
        /* [in] */ IInterface* k,
        /* [in] */ Boolean compare);

    AutoPtr<TreeNode> GetTreeNode(
        /* [in] */ Integer h,
        /* [in] */ IInterface* k);

    static Integer TieBreakOrder(
        /* [in] */ IInterface* a,
        /* [in] */ IInterface* b);

    void Treeify(
        /* [in] */ Array<Node*>& tab);

    AutoPtr<Node> Untreeify(
        /* [in] */ HashMap* map);

    AutoPtr<TreeNode> PutTreeVal(
        /* [in] */ HashMap* map,
        /* [in] */ Array<Node*>& tab,
        /* [in] */ Integer h,
        /* [in] */ IInterface* k,
        /* [in] */ IInterface* v);

    void RemoveTreeNode(
        /* [in] */ HashMap* map,
        /* [in] */ Array<Node*>& tab,
        /* [in] */ Boolean movable);

    void Split(
        /* [in] */ HashMap* map,
        /* [in] */ Array<Node*>& tab,
        /* [in] */ Integer index,
        /* [in] */ Integer bit);

    static AutoPtr<TreeNode> RotateLeft(
        /* [in] */ TreeNode* root,
        /* [in] */ TreeNode* p);

    static AutoPtr<TreeNode> RotateRight(
        /* [in] */ TreeNode* root,
        /* [in] */ TreeNode* p);

    static AutoPtr<TreeNode> BalanceInsertion(
        /* [in] */ TreeNode* root,
        /* [in] */ TreeNode* x);

    static AutoPtr<TreeNode> BalanceDeletion(
        /* [in] */ TreeNode* root,
        /* [in] */ TreeNode* x);

    static Boolean CheckInvariants(
        /* [in] */ TreeNode* t);

    static Boolean IsTreeNode(
        /* [in] */ HashMap::Node* node);

public:
    TreeNode* mParent = nullptr;
    TreeNode* mLeft = nullptr;
    TreeNode* mRight = nullptr;
    TreeNode* mPrev = nullptr;
    Boolean mRed = false;
};

inline Boolean TreeNode::IsTreeNode(
    /* [in] */ HashMap::Node* node)
{
    return ITreeNode::Probe((IMapEntry*)node) != nullptr;
}

}
}


#endif // __CCM_UTIL_TREENODE_H__
