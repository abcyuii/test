OptionDef *find_option(const OptionDef *po, const char *name)
{
    if (*name == '/')
        name++;

    while (po->name) {
        const char *end;
        if (av_strstart(name, po->name, &end) && (!*end || *end == ':'))
            break;
        po++;
    }
    return po;
}