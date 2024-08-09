int write_option(void *optctx, const OptionDef *po, const char *opt,
                        const char *arg, const OptionDef *defs)
{

    void *dst = po->flags & OPT_FLAG_OFFSET ?
                (uint8_t *)optctx + po->u.off : po->u.dst_ptr;
    char *arg_allocated = NULL;

    SpecifierOptList *sol = NULL;
    double num;
    int ret = 0;

    if (*opt == '/') {
        opt++;

        if (po->type == OPT_TYPE_BOOL) {
            av_log(NULL, AV_LOG_FATAL,
                   "Requested to load an argument from file for a bool option '%s'\n",
                   po->name);
            return AVERROR(EINVAL);
        }

        arg_allocated = file_read(arg);
        if (!arg_allocated) {
            av_log(NULL, AV_LOG_FATAL,
                   "Error reading the value for option '%s' from file: %s\n",
                   opt, arg);
            return AVERROR(EINVAL);
        }

        arg = arg_allocated;
    }

    if (po->flags & OPT_FLAG_SPEC) {
        char *p = strchr(opt, ':');
        char *str;

        sol = dst;
        ret = GROW_ARRAY(sol->opt, sol->nb_opt);
        if (ret < 0)
            goto finish;

        str = av_strdup(p ? p + 1 : "");
        if (!str) {
            ret = AVERROR(ENOMEM);
            goto finish;
        }
        sol->opt[sol->nb_opt - 1].specifier = str;
        dst = &sol->opt[sol->nb_opt - 1].u;
    }

    if (po->type == OPT_TYPE_STRING) {
        char *str;
        if (arg_allocated) {
            str           = arg_allocated;
            arg_allocated = NULL;
        } else
            str = av_strdup(arg);
        av_freep(dst);

        if (!str) {
            ret = AVERROR(ENOMEM);
            goto finish;
        }

        *(char **)dst = str;
    } else if (po->type == OPT_TYPE_BOOL || po->type == OPT_TYPE_INT) {
        ret = parse_number(opt, arg, OPT_TYPE_INT64, INT_MIN, INT_MAX, &num);
        if (ret < 0)
            goto finish;

        *(int *)dst = num;
    } else if (po->type == OPT_TYPE_INT64) {
        ret = parse_number(opt, arg, OPT_TYPE_INT64, INT64_MIN, (double)INT64_MAX, &num);
        if (ret < 0)
            goto finish;

        *(int64_t *)dst = num;
    } else if (po->type == OPT_TYPE_TIME) {
        ret = av_parse_time(dst, arg, 1);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Invalid duration for option %s: %s\n",
                   opt, arg);
            goto finish;
        }
    } else if (po->type == OPT_TYPE_FLOAT) {
        ret = parse_number(opt, arg, OPT_TYPE_FLOAT, -INFINITY, INFINITY, &num);
        if (ret < 0)
            goto finish;

        *(float *)dst = num;
    } else if (po->type == OPT_TYPE_DOUBLE) {
        ret = parse_number(opt, arg, OPT_TYPE_DOUBLE, -INFINITY, INFINITY, &num);
        if (ret < 0)
            goto finish;

        *(double *)dst = num;
    } else {
        av_assert0(po->type == OPT_TYPE_FUNC && po->u.func_arg);

        ret = po->u.func_arg(optctx, opt, arg);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR,
                   "Failed to set value '%s' for option '%s': %s\n",
                   arg, opt, av_err2str(ret));
            goto finish;
        }
    }
    if (po->flags & OPT_EXIT) {
        ret = AVERROR_EXIT;
        goto finish;
    }

    if (sol) {
        sol->type = po->type;
        sol->opt_canon = (po->flags & OPT_HAS_CANON) ?
                         find_option(defs, po->u1.name_canon) : po;
    }

finish:
    av_freep(&arg_allocated);
    return ret;
}