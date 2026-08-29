Point Updates:
const int N = 1e5;
int n;
int t[2*N];
void build(){
  for(int i=n-1; i>0; i--) t[i] = t[i<<1]+t[i<<1|1];
}
void modify(int p, int value){
  for(t[p+=n]=value; p>1; p>>=1) t[p>>1] = t[p]+t[p^1];
}
int query(int l, int r) {
  int res=0;
  for(l+=n, r+=n; l<r; l>>=1, r>>=1){
    if(l&1) res += t[l++];
    if(r&1) res += t[--r];
  }
  return res;
}

Lazy Propagation:
// ---- Problem-specific: redefine these for each problem ----
struct Node {
    // e.g. long long sum = 0;
};
struct Tag {
    bool has_update = false;
    // e.g. long long val = 0;
};
Node merge(const Node& l, const Node& r);
void apply(Node& node, const Tag& tag, int len);
void compose(Tag& parent, const Tag& child);
Node makeLeaf(long long val);   // single array value -> Node
// -------------------------------------------------------------

struct SegTree {
    int n;
    vector<Node> t;
    vector<Tag> lazy;

    template<typename T>
    SegTree(vector<T>& a) : n((int)a.size()) {
        t.assign(4 * n, Node());
        lazy.assign(4 * n, Tag());
        if (n > 0) build(a, 1, 0, n - 1);
    }

    template<typename T>
    void build(vector<T>& a, int v, int tl, int tr) {
        if (tl == tr) {
            t[v] = makeLeaf(a[tl]);
            return;
        }
        int tm = (tl + tr) / 2;
        build(a, v*2, tl, tm);
        build(a, v*2+1, tm+1, tr);
        t[v] = merge(t[v*2], t[v*2+1]);
    }

    void push(int v, int tl, int tr) {
        if (!lazy[v].has_update) return;
        int tm = (tl + tr) / 2;
        apply(t[v*2], lazy[v], tm - tl + 1);
        compose(lazy[v*2], lazy[v]);
        lazy[v*2].has_update = true;
        apply(t[v*2+1], lazy[v], tr - tm);
        compose(lazy[v*2+1], lazy[v]);
        lazy[v*2+1].has_update = true;
        lazy[v] = Tag();
    }

    void update(int v, int tl, int tr, int l, int r, const Tag& tag) {
        if (l > r) return;
        if (l == tl && r == tr) {
            apply(t[v], tag, tr - tl + 1);
            compose(lazy[v], tag);
            lazy[v].has_update = true;
            return;
        }
        push(v, tl, tr);
        int tm = (tl + tr) / 2;
        update(v*2, tl, tm, l, min(r, tm), tag);
        update(v*2+1, tm+1, tr, max(l, tm+1), r, tag);
        t[v] = merge(t[v*2], t[v*2+1]);
    }

    void update(int l, int r, const Tag& tag) { update(1, 0, n-1, l, r, tag); }

    Node query(int v, int tl, int tr, int l, int r) {
        if (l == tl && r == tr) return t[v];
        push(v, tl, tr);
        int tm = (tl + tr) / 2;
        if (r <= tm) return query(v*2, tl, tm, l, r);
        if (l > tm) return query(v*2+1, tm+1, tr, l, r);
        return merge(query(v*2, tl, tm, l, tm), query(v*2+1, tm+1, tr, tm+1, r));
    }

    Node query(int l, int r) { return query(1, 0, n-1, l, r); }
};
