array<int, 100> t, d;

func int push(int l, int r, int v) {
    t[v] += (r - l) * d[v];

    if (l + 1 != r) {
        d[2*v] += d[v];
        d[2*v + 1] += d[v];
    }
    d[v] = 0;

    return 0;
}

func int upd(int ql, int qr, int x, int l, int r, int v) {
    push(l, r, v);
    if (r <= ql or qr <= l) return 0;
    if (ql <= l and r <= qr) {
        d[v] = x;
        push(l, r, v);
        return 0;
    }

    int m = (l + r) / 2;
    upd(ql, qr, x, l, m, 2*v);
    upd(ql, qr, x, m, r, 2*v + 1);
    t[v] = t[2*v] + t[2*v + 1];
    return 0;
}

func int get(int ql, int qr, int l, int r, int v) {
    push(l, r, v);
    if (r <= ql or qr <= l) return 0;
    if (ql <= l and r <= qr) return t[v];

    int m = (l + r) / 2;
    return get(ql, qr, l, m, 2*v) + get(ql, qr, m, r, 2*v + 1);
}

func int main() {
    int n;
    scan(n);

    int q;
    scan(q);

    for (int i = 0; i < q; ++i) {
        int t;
        scan(t);

        int l, r;
        scan(l), scan(r);

        if (t == 1) {
            int x;
            scan(x);

            upd(l - 1, r, x, 0, n, 1);
        } else {
            print(get(l - 1, r, 0, n, 1));
            print('\n');
        }
    }

    return 0;
}
