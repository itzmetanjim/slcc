static char* slcc_indent_transform(const char *src, int srclen, int *outlen){
    char *out = tcc_malloc(srclen+1);
    memcpy(out, src, srclen);
    out[srclen]=0;
    *outlen=srclen;
    return out;
}
