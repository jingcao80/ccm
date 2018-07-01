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

void TreeNode::MoveRootToFront(
    /* [in] */ Array<Node*>& tab,
    /* [in] */ TreeNode* root)
{
    Integer n;
    if (root != nullptr && !tab.IsNull() && (n = tab.GetLength(), n > 0)) {
        Integer index = (n - 1) & root->mHash;
        AutoPtr<TreeNode> first = (TreeNode*)tab[index];
        if (root != first) {
            AutoPtr<Node> rn;
            tab.Set(index, root);
            AutoPtr<TreeNode> rp = root->mPrev;
            if (rn = root->mNext, rn != nullptr) {
                ((TreeNode*)rn.Get())->mPrev = rp;
            }
            if (rp != nullptr) {
                rp->mNext = rn;
            }
            if (first != nullptr) {
                first->mPrev = root;
            }
            root->mNext = first;
            root->mPrev = nullptr;
        }
        CHECK(CheckInvariants(root));
    }
}

AutoPtr<TreeNode> TreeNode::Find(
    /* [in] */ Integer h,
    /* [in] */ IInterface* k,
    /* [in] */ Boolean compare)
{
    TreeNode* p = this;
    do {
        Integer ph, dir;
        IInterface* pk;
        TreeNode* pl = p->mLeft;
        TreeNode* pr = p->mRight;
        TreeNode* q = nullptr;
        if (ph = p->mHash, ph > h) {
            p = pl;
        }
        else if (ph < h) {
            p = pr;
        }
        else if ((pk = p->mKey, IInterface::Equals(pk, k)) ||
                (k != nullptr && Object::Equals(k, pk))) {
            return p;
        }
        else if (pl == nullptr) {
            p = pr;
        }
        else if (pr == nullptr) {
            p = pl;
        }
        else if (compare &&
                (dir = HashMap::CompareComparables(k, pk), dir != 0)) {
            p = dir < 0 ? pl : pr;
        }
        else if (q = pr->Find(h, k, compare), q != nullptr) {
            return q;
        }
        else {
            p = pl;
        }
    } while (p != nullptr);
    return nullptr;
}

AutoPtr<TreeNode> TreeNode::GetTreeNode(
    /* [in] */ Integer h,
    /* [in] */ IInterface* k)
{
    AutoPtr<TreeNode> node = mParent != nullptr ?
            Root() : AutoPtr<TreeNode>(this);
    return node->Find(h, k, false);
}

void TreeNode::Treeify(
    /* [in] */ Array<Node*>& tab)
{
    AutoPtr<TreeNode> root;
    for (AutoPtr<TreeNode> x = this, next; x != nullptr; x = next) {
        next = (TreeNode*)x->mNext.Get();
        x->mLeft = x->mRight = nullptr;
        if (root == nullptr) {
            x->mParent = nullptr;
            x->mRed = false;
            root = x;
        }
        else {
            IInterface* k = x->mKey;
            Integer h = x->mHash;
            IComparable* kc = nullptr;
            for (AutoPtr<TreeNode> p = root;;) {
                Integer dir, ph;
                IInterface* pk = p->mKey;
                if (ph = p->mHash, ph > h) {
                    dir = -1;
                }
                else if (ph < h) {
                    dir = 1;
                }
                else if ((kc == nullptr &&
                        (kc = IComparable::Probe(k), kc == nullptr)) ||
                        (dir = HashMap::CompareComparables(k, pk), dir == 0)) {
                    // dir = TieBreekOrder(k, pk);
                }

                // TreeNode* xp = p;

            }
        }
    }
    MoveRootToFront(tab, root);
}

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
