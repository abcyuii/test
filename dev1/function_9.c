int opt_has_arg(const OptionDef *o)
{
    if (o->type == OPT_TYPE_BOOL)
        return 0;
    if (o->type == OPT_TYPE_FUNC)
        return !!(o->flags & OPT_FUNC_ARG);
    return 1;
}