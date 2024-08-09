void show_help_options(const OptionDef *options, const char *msg, int req_flags,
                       int rej_flags)
{
    const OptionDef *po;
    int first;

    first = 1;
    for (po = options; po->name; po++) {
        char buf[128];

        if (((po->flags & req_flags) != req_flags) ||
            (po->flags & rej_flags))
            continue;

        if (first) {
            printf("%s\n", msg);
            first = 0;
        }
        av_strlcpy(buf, po->name, sizeof(buf));

        if (po->flags & OPT_FLAG_PERSTREAM)
            av_strlcat(buf, "[:<stream_spec>]", sizeof(buf));
        else if (po->flags & OPT_FLAG_SPEC)
            av_strlcat(buf, "[:<spec>]", sizeof(buf));

        if (po->argname)
            av_strlcatf(buf, sizeof(buf), " <%s>", po->argname);

        printf("-%-17s  %s\n", buf, po->help);
    }
    printf("\n");
}