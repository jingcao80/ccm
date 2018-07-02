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

Integer TreeNode::TieBreakOrder(
    /* [in] */ IInterface* a,
    /* [in] */ IInterface* b)
{
    Integer d = 0;
    String aKlsName, bKlsName;
    if (a == nullptr || b == nullptr ||
            (aKlsName = Object::GetCoclassName(a),
             bKlsName = Object::GetCoclassName(b),
             d = aKlsName.Compare(bKlsName), a == 0)) {
        d = Object::GetHashCode(a) <= Object::GetHashCode(b) ?
                -1 : 1;
    }
    return d;
}

void TreeNode::Treeify(
    /* [in] */ Array<Node*>& tab)
{
    AutoPtr<TreeNode> root;
    for (TreeNode* x = this, *next; x != nullptr; x = next) {
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
            for (TreeNode* p = root;;) {
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
                    dir = TieBreakOrder(k, pk);
                }

                TreeNode* xp = p;
                if (p = (dir <= 0) ? p->mLeft : p->mRight, p == nullptr) {
                    x->mParent = xp;
                    if (dir <= 0) {
                        xp->mLeft = x;
                    }
                    else {
                        xp->mRight = x;
                    }
                    root = BalanceInsertion(root, x);
                    break;
                }
            }
        }
    }
    MoveRootToFront(tab, root);
}

AutoPtr<HashMap::Node> TreeNode::Untreeify(
    /* [in] */ HashMap* map)
{
    AutoPtr<Node> hd;
    Node* tl = nullptr;
    for (Node* q = this; q != nullptr; q = q->mNext) {
        AutoPtr<Node> p = map->ReplacementNode(q, nullptr);
        if (tl == nullptr) {
            hd = p;
        }
        else {
            tl->mNext = p;
        }
        tl = p;
    }
    return hd;
}

AutoPtr<TreeNode> TreeNode::PutTreeVal(
    /* [in] */ HashMap* map,
    /* [in] */ Array<Node*>& tab,
    /* [in] */ Integer h,
    /* [in] */ IInterface* k,
    /* [in] */ IInterface* v)
{
    IComparable* kc = nullptr;
    Boolean searched = false;
    AutoPtr<TreeNode> root = (mParent != nullptr) ?
            Root() : AutoPtr<TreeNode>(this);
    for (TreeNode* p = root;;) {
        Integer dir, ph;
        IInterface* pk;
        if (ph = p->mHash, ph > h) {
            dir = -1;
        }
        else if (ph < h) {
            dir = 1;
        }
        else if ((pk = p->mKey, IInterface::Equals(pk, k)) ||
                (k != nullptr && Object::Equals(k, pk))) {
            return p;
        }
        else if ((kc == nullptr &&
                (kc = IComparable::Probe(k), kc == nullptr)) ||
                (dir = HashMap::CompareComparables(k, pk), dir == 0)) {
            if (!searched) {
                TreeNode* q, *ch;
                searched = true;
                if (((ch = p->mLeft, ch != nullptr) &&
                        (q = ch->Find(h, k, kc), q != nullptr)) ||
                        ((ch = p->mRight, ch != nullptr) &&
                        (q = ch->Find(h, k, kc), q != nullptr))) {
                    return q;
                }
            }
            dir = TieBreakOrder(k, pk);
        }

        TreeNode* xp = p;
        if (p = (dir <= 0) ? p->mLeft : p->mRight, p == nullptr) {
            Node* xpn = xp->mNext;
            AutoPtr<TreeNode> x = map->NewTreeNode(h, k, v, xpn);
            if (dir <= 0) {
                xp->mLeft = x;
            }
            else {
                xp->mRight = x;
            }
            xp->mNext = x;
            x->mParent = x->mPrev = xp;
            if (xpn != nullptr) {
                ((TreeNode*)xpn)->mPrev = x;
            }
            MoveRootToFront(tab, BalanceInsertion(root, x));
            return nullptr;
        }
    }
}

void TreeNode::RemoveTreeNode(
    /* [in] */ HashMap* map,
    /* [in] */ Array<Node*>& tab,
    /* [in] */ Boolean movable)
{
    Integer n;
    if (tab.IsNull() || (n = tab.GetLength(), n == 0)) {
        return;
    }
    Integer index = (n - 1) & mHash;
    TreeNode* first = (TreeNode*)tab[index];
    AutoPtr<TreeNode> root = first;
    TreeNode* rl;
    TreeNode* succ = (TreeNode*)mNext.Get();
    TreeNode* pred = mPrev;
    if (pred == nullptr) {
        first = succ;
        tab.Set(index, first);
    }
    else {
        pred->mNext = succ;
    }
    if (succ != nullptr) {
        succ->mPrev = pred;
    }
    if (first == nullptr) {
        return;
    }
    if (root->mParent != nullptr) {
        root = root->Root();
    }
    if (root == nullptr || root->mRight == nullptr ||
            (rl = root->mLeft, rl == nullptr) || rl->mLeft == nullptr) {
        tab.Set(index, first->Untreeify(map)); // too small
        return;
    }
    TreeNode* p = this;
    TreeNode* pl = mLeft;
    TreeNode* pr = mRight;
    TreeNode* replacement;
    if (pl != nullptr && pr != nullptr) {
        TreeNode* s = pr, *sl;
        while (sl = s->mLeft, sl != nullptr) { // find successor
            s = sl;
        }
        Boolean c = s->mRed;
        s->mRed = p->mRed;
        p->mRed = c; // swap colors
        TreeNode* sr = s->mRight;
        TreeNode* pp = p->mParent;
        if (s == pr) { // p was s's direct parent
            p->mParent = s;
            s->mRight = p;
        }
        else {
            TreeNode* sp = s->mParent;
            if (p->mParent = sp, p->mParent != nullptr) {
                if (s == sp->mLeft) {
                    sp->mLeft = p;
                }
                else {
                    sp->mRight = p;
                }
            }
            if (s->mRight = pr, s->mRight != nullptr) {
                pr->mParent = s;
            }
        }
        p->mLeft = nullptr;
        if (p->mRight = sr, p->mRight != nullptr) {
            sr->mParent = p;
        }
        if (s->mLeft = pl, s->mLeft != nullptr) {
            pl->mParent = s;
        }
        if (s->mParent = pp, s->mParent == nullptr) {
            root = s;
        }
        else if (p == pp->mLeft) {
            pp->mLeft = s;
        }
        else {
            pp->mRight = s;
        }
        if (sr != nullptr) {
            replacement = sr;
        }
        else {
            replacement = p;
        }
    }
    else if (pl != nullptr) {
        replacement = pl;
    }
    else if (pr != nullptr) {
        replacement = pr;
    }
    else {
        replacement = p;
    }
    if (replacement != p) {
        TreeNode* pp = replacement->mParent = p->mParent;
        if (pp == nullptr) {
            root = replacement;
        }
        else if (p == pp->mLeft) {
            pp->mLeft = replacement;
        }
        else {
            pp->mRight = replacement;
        }
        p->mLeft = p->mRight = p->mParent = nullptr;
    }

    AutoPtr<TreeNode> r = p->mRed ? root : BalanceDeletion(root, replacement);

    if (replacement == p) {  // detach
        TreeNode* pp = p->mParent;
        p->mParent = nullptr;
        if (pp != nullptr) {
            if (p == pp->mLeft) {
                pp->mLeft = nullptr;
            }
            else if (p == pp->mRight) {
                pp->mRight = nullptr;
            }
        }
    }
    if (movable) {
        MoveRootToFront(tab, r);
    }
}

void TreeNode::Split(
    /* [in] */ HashMap* map,
    /* [in] */ Array<Node*>& tab,
    /* [in] */ Integer index,
    /* [in] */ Integer bit)
{
    TreeNode* b = this;
    // Relink into lo and hi lists, preserving order
    AutoPtr<TreeNode> loHead;
    TreeNode *loTail = nullptr;
    AutoPtr<TreeNode> hiHead;
    TreeNode *hiTail = nullptr;
    Integer lc = 0, hc = 0;
    for (AutoPtr<TreeNode> e = b, next; e != nullptr; e = next) {
        next = (TreeNode*)e->mNext.Get();
        e->mNext = nullptr;
        if ((e->mHash & bit) == 0) {
            if (e->mPrev = loTail, e->mPrev == nullptr) {
                loHead = e;
            }
            else {
                loTail->mNext = e;
            }
            loTail = e;
            ++lc;
        }
        else {
            if (e->mPrev = hiTail, e->mPrev == nullptr) {
                hiHead = e;
            }
            else {
                hiTail->mNext = e;
            }
            hiTail = e;
            ++hc;
        }
    }

    if (loHead != nullptr) {
        if (lc <= HashMap::UNTREEIFY_THRESHOLD) {
            tab.Set(index, loHead->Untreeify(map));
        }
        else {
            tab.Set(index, loHead);
            if (hiHead != nullptr) { // (else is already treeified)
                loHead->Treeify(tab);
            }
        }
    }
    if (hiHead != nullptr) {
        if (hc <= HashMap::UNTREEIFY_THRESHOLD) {
            tab.Set(index + bit, hiHead->Untreeify(map));
        }
        else {
            tab.Set(index + bit, hiHead);
            if (loHead != nullptr) {
                hiHead->Treeify(tab);
            }
        }
    }
}

AutoPtr<TreeNode> TreeNode::RotateLeft(
    /* [in] */ TreeNode* root,
    /* [in] */ TreeNode* p)
{
    TreeNode* r, *pp, *rl;
    if (p != nullptr && (r = p->mRight, r != nullptr)) {
        if (rl = p->mRight = r->mLeft, rl != nullptr) {
            rl->mParent = p;
        }
        if (pp = r->mParent = p->mParent, pp == nullptr) {
            root = r;
            root->mRed = false;
        }
        else if (pp->mLeft == p) {
            pp->mLeft = r;
        }
        else {
            pp->mRight = r;
        }
        r->mLeft = p;
        p->mParent = r;
    }
    return root;
}

AutoPtr<TreeNode> TreeNode::RotateRight(
    /* [in] */ TreeNode* root,
    /* [in] */ TreeNode* p)
{
    TreeNode* l, *pp, *lr;
    if (p != nullptr && (l = p->mLeft, l != nullptr)) {
        if (lr = p->mLeft = l->mRight, lr != nullptr) {
            lr->mParent = p;
        }
        if (pp = l->mParent = p->mParent, pp == nullptr) {
            root = l;
            root->mRed = false;
        }
        else if (pp->mRight == p) {
            pp->mRight = l;
        }
        else {
            pp->mLeft = l;
        }
        l->mRight = p;
        p->mParent = l;
    }
    return root;
}

AutoPtr<TreeNode> TreeNode::BalanceInsertion(
    /* [in] */ TreeNode* root,
    /* [in] */ TreeNode* x)
{
    x->mRed = true;
    for (TreeNode* xp, *xpp, *xppl, *xppr;;) {
        if (xp = x->mParent, xp == nullptr) {
            x->mRed = false;
            return x;
        }
        else if (!xp->mRed || (xpp = xp->mParent, xpp == nullptr)) {
            return root;
        }
        if (xppl = xpp->mLeft, xp == xppl) {
            if ((xppr = xpp->mRight, xppr != nullptr) && xppr->mRed) {
                xppr->mRed = false;
                xp->mRed = false;
                xpp->mRed = true;
                x = xpp;
            }
            else {
                if (x == xp->mRight) {
                    root = RotateLeft(root, x = xp);
                    xpp = (xp = x->mParent) == nullptr ? nullptr : xp->mParent;
                }
                if (xp != nullptr) {
                    xp->mRed = false;
                    if (xpp != nullptr) {
                        xpp->mRed = true;
                        root = RotateRight(root, xpp);
                    }
                }
            }
        }
        else {
            if (xppl != nullptr && xppl->mRed) {
                xppl->mRed = false;
                xp->mRed = false;
                xpp->mRed = true;
                x = xpp;
            }
            else {
                if (x == xp->mLeft) {
                    root = RotateRight(root, x = xp);
                    xpp = (xp = x->mParent) == nullptr ? nullptr : xp->mParent;
                }
                if (xp != nullptr) {
                    xp->mRed = false;
                    if (xpp != nullptr) {
                        xpp->mRed = true;
                        root = RotateLeft(root, xpp);
                    }
                }
            }
        }
    }
}

AutoPtr<TreeNode> TreeNode::BalanceDeletion(
    /* [in] */ TreeNode* root,
    /* [in] */ TreeNode* x)
{
    for (TreeNode* xp, *xpl, *xpr;;)  {
        if (x == nullptr || x == root) {
            return root;
        }
        else if (xp = x->mParent, xp == nullptr) {
            x->mRed = false;
            return x;
        }
        else if (x->mRed) {
            x->mRed = false;
            return root;
        }
        else if (xpl = xp->mLeft, xpl == x) {
            if ((xpr = xp->mRight, xpr != nullptr) && xpr->mRed) {
                xpr->mRed = false;
                xp->mRed = true;
                root = RotateLeft(root, xp);
                xpr = (xp = x->mParent) == nullptr ? nullptr : xp->mRight;
            }
            if (xpr == nullptr) {
                x = xp;
            }
            else {
                TreeNode* sl = xpr->mLeft, *sr = xpr->mRight;
                if ((sr == nullptr || !sr->mRed) &&
                    (sl == nullptr || !sl->mRed)) {
                    xpr->mRed = true;
                    x = xp;
                }
                else {
                    if (sr == nullptr || !sr->mRed) {
                        if (sl != nullptr) {
                            sl->mRed = false;
                        }
                        xpr->mRed = true;
                        root = RotateRight(root, xpr);
                        xpr = (xp = x->mParent) == nullptr ?
                            nullptr : xp->mRight;
                    }
                    if (xpr != nullptr) {
                        xpr->mRed = (xp == nullptr) ? false : xp->mRed;
                        if ((sr = xpr->mRight) != nullptr)
                            sr->mRed = false;
                    }
                    if (xp != nullptr) {
                        xp->mRed = false;
                        root = RotateLeft(root, xp);
                    }
                    x = root;
                }
            }
        }
        else { // symmetric
            if (xpl != nullptr && xpl->mRed) {
                xpl->mRed = false;
                xp->mRed = true;
                root = RotateRight(root, xp);
                xpl = (xp = x->mParent) == nullptr ? nullptr : xp->mLeft;
            }
            if (xpl == nullptr) {
                x = xp;
            }
            else {
                TreeNode* sl = xpl->mLeft, *sr = xpl->mRight;
                if ((sl == nullptr || !sl->mRed) &&
                    (sr == nullptr || !sr->mRed)) {
                    xpl->mRed = true;
                    x = xp;
                }
                else {
                    if (sl == nullptr || !sl->mRed) {
                        if (sr != nullptr) {
                            sr->mRed = false;
                        }
                        xpl->mRed = true;
                        root = RotateLeft(root, xpl);
                        xpl = (xp = x->mParent) == nullptr ?
                            nullptr : xp->mLeft;
                    }
                    if (xpl != nullptr) {
                        xpl->mRed = (xp == nullptr) ? false : xp->mRed;
                        if (sl = xpl->mLeft, sl != nullptr) {
                            sl->mRed = false;
                        }
                    }
                    if (xp != nullptr) {
                        xp->mRed = false;
                        root = RotateRight(root, xp);
                    }
                    x = root;
                }
            }
        }
    }
}

Boolean TreeNode::CheckInvariants(
    /* [in] */ TreeNode* t)
{
    TreeNode* tp = t->mParent, *tl = t->mLeft, *tr = t->mRight,
            *tb = t->mPrev, *tn = (TreeNode*)t->mNext.Get();
    if (tb != nullptr && tb->mNext != t) {
        return false;
    }
    if (tn != nullptr && tn->mPrev != t) {
        return false;
    }
    if (tp != nullptr && t != tp->mLeft && t != tp->mRight) {
        return false;
    }
    if (tl != nullptr && (tl->mParent != t || tl->mHash > t->mHash)) {
        return false;
    }
    if (tr != nullptr && (tr->mParent != t || tr->mHash < t->mHash)) {
        return false;
    }
    if (t->mRed && tl != nullptr && tl->mRed && tr != nullptr && tr->mRed) {
        return false;
    }
    if (tl != nullptr && !CheckInvariants(tl)) {
        return false;
    }
    if (tr != nullptr && !CheckInvariants(tr)) {
        return false;
    }
    return true;
}

}
}
