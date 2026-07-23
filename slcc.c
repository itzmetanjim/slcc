static int slcc_st4(const char *line, int len){
    int st = 0, i;
    for (i = 0; i < len; i++) {
        if (line[i] == ' ') st++;
        else if (line[i] == '\t') st += 4;
        else break;
    }
    return st;
}
static int slcc_lb(const int *arr, int n, int val){
    int lo = 0, hi = n, mid;
    while (lo < hi) {
        mid = (lo + hi) / 2;
        if (arr[mid] < val) lo = mid + 1;
        else hi = mid;
    }
    return lo;
}
static char *slcc_indent_transform(const char *src, int srclen, int *outlen){
    int ts_cap = 64, ts_len = 1;
    int *ts = tcc_malloc(ts_cap * sizeof(int));
    int out_cap = srclen * 2 + 256;
    char *out = tcc_malloc(out_cap);
    int out_pos = 0;
    int cur_start, cur_len, nxt_start, nxt_len;
    int has_nxt, st4_nxt, st4_cur;
    int i, idx, before, n_braces;
    int in_bcomment = 0;
    const char *cur;
    ts[0] = 0;
#define OUT_APPEND(ptr, n) do { \
    while (out_pos + (n) > out_cap) { \
        out_cap *= 2; \
        out = tcc_realloc(out, out_cap); \
    } \
    memcpy(out + out_pos, (ptr), (n)); \
    out_pos += (n); \
} while(0)
#define OUT_CHAR(ch) do { \
    if (out_pos >= out_cap) { \
        out_cap *= 2; \
        out = tcc_realloc(out, out_cap); \
    } \
    out[out_pos++] = (ch); \
} while(0)
    if (srclen <= 0) {
        *outlen = 0;
        tcc_free(ts);
        out[0] = 0;
        return out;
    }
    cur_start = 0;
    cur_len = 0;
    while (cur_len < srclen && src[cur_len] != '\n')
        cur_len++;
    if (cur_len > 0 && src[cur_len - 1] == '\r')
        cur_len--;
    nxt_start = cur_len;
    if (nxt_start < srclen && src[nxt_start] == '\n')
        nxt_start++;
    if (nxt_start < srclen) {
        has_nxt = 1;
        nxt_len = 0;
        while (nxt_len < srclen - nxt_start && src[nxt_start + nxt_len] != '\n')
            nxt_len++;
        if (nxt_len > 0 && src[nxt_start + nxt_len - 1] == '\r')
            nxt_len--;
        st4_nxt = slcc_st4(src + nxt_start, nxt_len);
    } else {
        has_nxt = 0;
        st4_nxt = 0;
    }
    for (;;) {
        cur = src + cur_start;
        if (cur_len > 0 && cur[0] == '#') {
            OUT_APPEND(cur, cur_len);
            if (has_nxt) OUT_CHAR('\n');
            goto nextL;
        }
        st4_cur = slcc_st4(cur, cur_len);
        if (ts_len > 1) {
            idx = slcc_lb(ts, ts_len, st4_cur);
            if (idx < ts_len - 1) {
                n_braces = ts_len - idx - 1;
                ts_len = idx + 1;
                for (i = 0; i < n_braces; i++)
                    OUT_CHAR('}');
            }
        }
        if (in_bcomment) goto OL;
        if (cur_len > 0 && cur[cur_len - 1] == ':') {
            int bc = in_bcomment, in_lc = 0;
            for (i = 0; i < cur_len - 1; i++) {
                if (!bc && !in_lc && cur[i] == '/' && cur[i+1] == '/') { in_lc = 1; }
                else if (!in_lc && !bc && cur[i] == '/' && cur[i+1] == '*') { bc = 1; i++; }
                else if (bc && cur[i] == '*' && cur[i+1] == '/') { bc = 0; i++; }
            }
            if (bc || in_lc) goto OL;
            {
                int pd = 0, hq = 0;
                for (i = 0; i < cur_len - 1; i++) {
                    if (cur[i] == '(') pd++;
                    else if (cur[i] == ')') pd--;
                    else if (cur[i] == '?' && pd == 0) hq = 1;
                }
                if (hq) goto OL;
            }
            before = cur_len - 1;
            while (before > 0 && cur[before - 1] == ' ')
                before--;
            if ((before > 0 && cur[before - 1] == ')') ||
                (before >= 4 && cur[before-4] == 'e' && cur[before-3] == 'l' &&
                                cur[before-2] == 's' && cur[before-1] == 'e')) {
                if (!has_nxt) {
                    OUT_APPEND(cur, cur_len);
                    if (has_nxt) OUT_CHAR('\n');
                    goto nextL;
                }
                OUT_APPEND(cur, cur_len - 1);
                OUT_CHAR('{');
                if (ts_len >= ts_cap) {
                    ts_cap *= 2;
                    ts = tcc_realloc(ts, ts_cap * sizeof(int));
                }
                ts[ts_len++] = st4_nxt;
                if (has_nxt) OUT_CHAR('\n');
                goto nextL;
            }
        }
OL:
        OUT_APPEND(cur, cur_len);
        if (has_nxt) OUT_CHAR('\n');
nextL:
        for (i = 0; i < cur_len - 1; i++) {
            if (!in_bcomment && cur[i] == '/' && cur[i+1] == '/') break;
            if (!in_bcomment && cur[i] == '/' && cur[i+1] == '*')
                in_bcomment = 1;
            else if (in_bcomment && cur[i] == '*' && cur[i+1] == '/')
                in_bcomment = 0;
        }
        if (!has_nxt) break;
        cur_start = nxt_start;
        cur_len = nxt_len;
        nxt_start = cur_start + cur_len;
        if (nxt_start < srclen && src[nxt_start] == '\n')
            nxt_start++;
        if (nxt_start < srclen) {
            has_nxt = 1;
            nxt_len = 0;
            while (nxt_len < srclen - nxt_start && src[nxt_start + nxt_len] != '\n')
                nxt_len++;
            if (nxt_len > 0 && src[nxt_start + nxt_len - 1] == '\r')
                nxt_len--;
            st4_nxt = slcc_st4(src + nxt_start, nxt_len);
        } else {
            has_nxt = 0;
        }
    }
    if (out_pos > 0 && out[out_pos - 1] != '\n')
        OUT_CHAR('\n');
    for (i = 1; i < ts_len; i++)
        OUT_CHAR('}');
    tcc_free(ts);
    *outlen = out_pos;
    out[out_pos] = 0;
    return out;
#undef OUT_APPEND
#undef OUT_CHAR
}
