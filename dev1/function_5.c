void show_help_children(const AVClass *class, int flags)
{
    void *iter = NULL;
    const AVClass *child;
    if (class->option) {
        av_opt_show2(&class, NULL, flags, 0);
        printf("\n");
    }

    while (child = av_opt_child_class_iterate(class, &iter))
        show_help_children(child, flags);
}